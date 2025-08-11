#pragma once
#include "stdafx.h"
#include "Framework/Core/CEngineStatics.h"

class UWorld;
struct PrimitiveData;
namespace jsonWrite {
	class JSON;
}
namespace json {
	class JSON;
}

class DataManager
{
public:
	static DataManager* Instance() 
	{
		if (_instance == nullptr)
			_instance = new DataManager();
		return _instance;
	};

	static void Release() 
	{
		delete _instance;
		_instance = nullptr;
	}

	void SaveWorldToJson(UWorld* world, const FString& fileName);
	TArray<PrimitiveData> LoadWorldFromJson(const FString& fileName);

	FString GenerateWorldJson(UWorld* world); 
	

	jsonWrite::JSON SerializeFVector(const FVector& vec);
	FVector DeserializeFVector(json::JSON& jsonArray);

private:
	DataManager() = default;
	~DataManager() = default;
	// Safe code for Singleton
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;



	static DataManager* _instance;
	const FString scenePath = "Scenes/";
};
