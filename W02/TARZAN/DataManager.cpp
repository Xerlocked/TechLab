#include "stdafx.h"
#include "DataManager.h"
#include "UWorld.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Include/jsonWrite.hpp"
#include "Include/json.hpp"

DataManager* DataManager::_instance = nullptr;

bool CreateDirectoryIfNotExists(const FString& path)
{
    return CreateDirectoryA(path.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError();
}

void DataManager::SaveWorldToJson(UWorld* world, const FString& fileName)
{
    FString saveFileName = fileName;
    if (saveFileName.find(".Scene") == FString::npos)
        saveFileName += ".Scene";

    FString fullPath = scenePath + saveFileName;

    FString jsonData = GenerateWorldJson(world); // 월드 데이터를 jsonWrite::JSON 문자열로 변환

    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for saving: " << fullPath << std::endl;
        return;
    }

    file << jsonData; // jsonWrite::JSON 데이터 저장
    file.close();
}

TArray<PrimitiveData> DataManager::LoadWorldFromJson(const FString& fileName)
{
    TArray<PrimitiveData> primitives;

    FString loadFileName = fileName;
    if (loadFileName.find(".Scene") == FString::npos)
        loadFileName += ".Scene";

    FString fullPath = scenePath + loadFileName;

    std::ifstream file(fullPath);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open file: " << fullPath << std::endl;
        UE_LOG(L"Failed to open file");
        return primitives;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    json::JSON jsonData = json::JSON::Load(buffer.str());

    if (!jsonData.hasKey("Primitives")) 
    {
        std::cerr << "Invalid JSON format: No 'Primitives' key found" << std::endl;
        return primitives;
    }

    json::JSON primitivesJson = jsonData["Primitives"];
    for (auto& item : primitivesJson.ObjectRange()) 
    {
        json::JSON obj = item.second;

        PrimitiveData primitive;
        primitive.Type = obj["Type"].ToString();
        primitive.Location = DeserializeFVector(obj["Location"]);
        primitive.Rotation = DeserializeFVector(obj["Rotation"]);
        primitive.Scale = DeserializeFVector(obj["Scale"]);

        primitives.push_back(primitive);
    }

    return primitives;
}

FString DataManager::GenerateWorldJson(UWorld* world)
{
    if (!world) return "";

    jsonWrite::JSON jsonData = jsonWrite::Object();

    jsonData["Version"] = 1;
    jsonData["NextUUID"] = CEngineStatics::TotalAllocationCount;
    int index = 0;

    jsonWrite::JSON primitivesJson = jsonWrite::Object();
    for (auto* comp : world->GetActors())
    {
        if (!comp) continue;

        UPrimitiveComponent* primitiveComp = dynamic_cast<UPrimitiveComponent*>(comp);
        if (primitiveComp == nullptr) continue;

        FVector loc = primitiveComp->GetRelativeLocation();
        FVector rot = primitiveComp->GetRelativeRotation();
        FVector scale = primitiveComp->GetRelativeScale3D();

        FString type = typeid(*primitiveComp).name();
        if (dynamic_cast<USphereComponent*>(comp)) type = "Sphere";
        else if (dynamic_cast<UCubeComponent*>(comp)) type = "Cube";
        else if (dynamic_cast<UPlaneComponent*>(comp)) type = "Plane";
        else continue;

        jsonWrite::JSON primitive = jsonWrite::Object();
        primitive["Location"] = SerializeFVector(loc);
        primitive["Rotation"] = SerializeFVector(rot);
        primitive["Scale"] = SerializeFVector(scale);
        primitive["Type"] = type;

        primitivesJson[std::to_string(index++)] = primitive;
    }

    jsonData["Primitives"] = primitivesJson;

    return jsonData.dump();
}

jsonWrite::JSON DataManager::SerializeFVector(const FVector& vec)
{
    return jsonWrite::Array(vec.x, vec.y, vec.z);
}

FVector DataManager::DeserializeFVector(json::JSON& jsonObject)
{
    return FVector(
        jsonObject[0].ToFloat(),
        jsonObject[1].ToFloat(),
        jsonObject[2].ToFloat()
    );
}