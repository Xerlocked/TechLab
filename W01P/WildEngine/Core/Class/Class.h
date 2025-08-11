#pragma once
#include <cstring>


class UClass
{
public:
    const char* ClassName;
    UClass* ParentClass;

    UClass(const char* InClassName, UClass* InParent = nullptr)
        : ClassName(InClassName), ParentClass(InParent) {
    }

    bool IsChildOf(UClass* BaseClass) const {
        for (const UClass* Current = this; Current; Current = Current->ParentClass) {
            if (Current == BaseClass) return true;
        }
        return false;
    }
};
