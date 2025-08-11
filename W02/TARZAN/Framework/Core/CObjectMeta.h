#pragma once
class CObjectMeta {
private:
	const FString _className;
	CObjectMeta* _parent;

public:
	CObjectMeta(const FString className, CObjectMeta* parent = nullptr) : _className(className), _parent(parent) {};
};

