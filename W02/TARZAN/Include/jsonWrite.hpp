#pragma once

#include <cstdint>
#include <cmath>
#include <cctype>
#include <string>
#include <deque>
#include <initializer_list>
#include <ostream>
#include <iostream>
#include <utility> // std::pair

namespace jsonWrite {

    using std::deque;
    using std::string;
    using std::initializer_list;
    using std::pair;

    namespace {
        string json_escape(const string& str) {
            string output;
            for (unsigned i = 0; i < str.length(); ++i)
                switch (str[i]) {
                case '\"': output += "\\\""; break;
                case '\\': output += "\\\\"; break;
                case '\b': output += "\\b";  break;
                case '\f': output += "\\f";  break;
                case '\n': output += "\\n";  break;
                case '\r': output += "\\r";  break;
                case '\t': output += "\\t";  break;
                default: output += str[i]; break;
                }
            return output;
        }
    }

    class JSON {
        union BackingData {
            BackingData(double d) : Float(d) {}
            BackingData(long   l) : Int(l) {}
            BackingData(bool   b) : Bool(b) {}
            BackingData(string s) : String(new string(s)) {}
            BackingData() : Int(0) {}

            // 객체는 순서 보존을 위해 deque<pair<string, JSON>> 사용
            deque<pair<string, JSON>>* Object;
            deque<JSON>* List;
            string* String;
            double              Float;
            long                Int;
            bool                Bool;
        } Internal;
    public:
        enum class Class {
            Null,
            Object,
            Array,
            String,
            Floating,
            Integral,
            Boolean
        };

        JSON() : Internal(), Type(Class::Null) {}

        // 복사 생성자 (객체 타입 처리 포함)
        JSON(const JSON& other) : Type(other.Type) {
            switch (other.Type) {
            case Class::Object:
                Internal.Object = new deque<pair<string, JSON>>(*other.Internal.Object);
                break;
            case Class::Array:
                Internal.List = new deque<JSON>(*other.Internal.List);
                break;
            case Class::String:
                Internal.String = new string(*other.Internal.String);
                break;
            case Class::Floating:
                Internal.Float = other.Internal.Float;
                break;
            case Class::Integral:
                Internal.Int = other.Internal.Int;
                break;
            case Class::Boolean:
                Internal.Bool = other.Internal.Bool;
                break;
            default:
                break;
            }
        }

        JSON& operator=(const JSON& other) {
            if (this == &other) return *this;
            ClearInternal();
            Type = other.Type;
            switch (other.Type) {
            case Class::Object:
                Internal.Object = new deque<pair<string, JSON>>(*other.Internal.Object);
                break;
            case Class::Array:
                Internal.List = new deque<JSON>(*other.Internal.List);
                break;
            case Class::String:
                Internal.String = new string(*other.Internal.String);
                break;
            case Class::Floating:
                Internal.Float = other.Internal.Float;
                break;
            case Class::Integral:
                Internal.Int = other.Internal.Int;
                break;
            case Class::Boolean:
                Internal.Bool = other.Internal.Bool;
                break;
            default:
                break;
            }
            return *this;
        }

        ~JSON() {
            ClearInternal();
        }

        // 기본 자료형에 대한 생성자
        template <typename T>
        JSON(T b, typename std::enable_if<std::is_same<T, bool>::value>::type* = 0)
            : Internal(b), Type(Class::Boolean) {
        }

        template <typename T>
        JSON(T i, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type* = 0)
            : Internal((long)i), Type(Class::Integral) {
        }

        template <typename T>
        JSON(T f, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
            : Internal((double)f), Type(Class::Floating) {
        }

        template <typename T>
        JSON(T s, typename std::enable_if<std::is_convertible<T, string>::value>::type* = 0)
            : Internal(string(s)), Type(Class::String) {
        }

        JSON(std::nullptr_t) : Internal(), Type(Class::Null) {}

        static JSON Make(Class type) {
            JSON ret; ret.SetType(type);
            return ret;
        }

        // 객체(Object) 타입 접근자
        JSON& operator[](const string& key) {
            SetType(Class::Object);
            for (auto& pairElem : *Internal.Object) {
                if (pairElem.first == key)
                    return pairElem.second;
            }
            Internal.Object->push_back({ key, JSON() });
            return Internal.Object->back().second;
        }

        // 배열(Array) 인덱스 접근자
        JSON& operator[](unsigned index) {
            SetType(Class::Array);
            if (index >= Internal.List->size())
                Internal.List->resize(index + 1);
            return (*Internal.List)[index];
        }

        bool hasKey(const string& key) const {
            if (Type == Class::Object) {
                for (const auto& pairElem : *Internal.Object) {
                    if (pairElem.first == key)
                        return true;
                }
            }
            return false;
        }

        int size() const {
            if (Type == Class::Object)
                return Internal.Object->size();
            else if (Type == Class::Array)
                return Internal.List->size();
            else
                return -1;
        }

        string dump(int depth = 1, string tab = "  ") const {
            string pad = "";
            for (int i = 0; i < depth; ++i)
                pad += tab;
            switch (Type) {
            case Class::Null:
                return "null";
            case Class::Object: {
                string s = "{\n";
                bool first = true;
                for (const auto& p : *Internal.Object) {
                    if (!first)
                        s += ",\n";
                    s += pad + "\"" + p.first + "\" : " + p.second.dump(depth + 1, tab);
                    first = false;
                }
                s += "\n" + pad.substr(0, pad.size() - tab.size()) + "}";
                return s;
            }
            case Class::Array: {
                string s = "[";
                bool first = true;
                for (const auto& elem : *Internal.List) {
                    if (!first)
                        s += ", ";
                    s += elem.dump(depth + 1, tab);
                    first = false;
                }
                s += "]";
                return s;
            }
            case Class::String:
                return "\"" + json_escape(*Internal.String) + "\"";
            case Class::Floating:
                return std::to_string(Internal.Float);
            case Class::Integral:
                return std::to_string(Internal.Int);
            case Class::Boolean:
                return Internal.Bool ? "true" : "false";
            default:
                return "";
            }
        }

        friend std::ostream& operator<<(std::ostream&, const JSON&);

        // 멤버 함수 append: 배열에 요소 추가
        template <typename T>
        void append(T arg) {
            SetType(Class::Array);
            Internal.List->push_back(arg);
        }

        template <typename T, typename... U>
        void append(T arg, U... args) {
            append(arg);
            append(args...);
        }

    private:
        void SetType(Class type) {
            if (type == Type)
                return;
            ClearInternal();
            switch (type) {
            case Class::Null:
                Internal.Object = nullptr;
                break;
            case Class::Object:
                Internal.Object = new deque<pair<string, JSON>>();
                break;
            case Class::Array:
                Internal.List = new deque<JSON>();
                break;
            case Class::String:
                Internal.String = new string();
                break;
            case Class::Floating:
                Internal.Float = 0.0;
                break;
            case Class::Integral:
                Internal.Int = 0;
                break;
            case Class::Boolean:
                Internal.Bool = false;
                break;
            }
            Type = type;
        }

        void ClearInternal() {
            switch (Type) {
            case Class::Object: delete Internal.Object; break;
            case Class::Array:  delete Internal.List;   break;
            case Class::String: delete Internal.String; break;
            default:
                break;
            }
        }

        Class Type = Class::Null;
    };

    JSON Array() {
        return JSON::Make(JSON::Class::Array);
    }

    template <typename... T>
    JSON Array(T... args) {
        JSON arr = JSON::Make(JSON::Class::Array);
        (arr.append(args), ...);
        return arr;
    }

    JSON Object() {
        return JSON::Make(JSON::Class::Object);
    }

    std::ostream& operator<<(std::ostream& os, const JSON& json) {
        os << json.dump();
        return os;
    }

} // End Namespace json
