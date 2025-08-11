#pragma once
#include "LightComponent.h"

class UPointLightComponent :public ULightComponentBase
{

    DECLARE_CLASS(UPointLightComponent, ULightComponentBase)
public:
    UPointLightComponent();

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;
    virtual void SetProperties(const TMap<FString, FString>& Properties) override;

    const FPointLightInfo& GetPointLightInfo() const;
    void SetPointLightInfo(const FPointLightInfo& InPointLightInfo);

    float GetRadius() const;
    void SetRadius(float InRadius);

    FLinearColor GetLightColor() const;
    void SetLightColor(const FLinearColor& InColor);


    float GetIntensity() const;
    void SetIntensity(float InIntensity);

    int GetType() const;
    void SetType(int InType);

private:
    FPointLightInfo PointLightInfo;
};


