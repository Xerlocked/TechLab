#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <utility>
#include <string>
#include <d3d11.h>

// Unreal Engine 스타일의 컨테이너 정의
template <typename T>
using TArray = std::vector<T>;

template <typename T>
using TLinkedList = std::list<T>;

// 정책 클래스를 사용하여 std::set 또는 std::unordered_set 선택
template <typename T, typename Policy = std::unordered_set<T>>
using TSet = Policy;

// 정책 클래스를 사용하여 std::map 또는 std::unordered_map 선택
template <typename Key, typename Value, typename Policy = std::unordered_map<Key, Value>>
using TMap = Policy;

template <typename T>
using TQueue = std::queue<T>;

template <typename T>
using TStack = std::stack<T>;

template <typename T1, typename T2>
using TPair = std::pair<T1, T2>;

using FString = std::string;

typedef unsigned int uint32;
typedef int int32;