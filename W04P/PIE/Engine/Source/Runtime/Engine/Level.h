#pragma once

#include "UObject/ObjectMacros.h"
#include "UObject/ObjectFactory.h"

class AActor;
class UObject;

class ULevel : public UObject
{
    DECLARE_CLASS(ULevel, UObject)

public:
    ULevel();
    virtual ~ULevel();

    virtual void Initialize();
    virtual void Tick(float DeltaTime);
    virtual void Release();

    void AddActor(AActor* Actor);
    void RemoveActor(AActor* Actor);
    TArray<AActor*> GetActors();

    // EditorManager 같은데로 보내기
    AActor* GetSelectedActor() const { return SelectedActor; }
    void SetPickedActor(AActor* InActor)
    {
        SelectedActor = InActor;
    }

    /**
     * Level에 Actor를 Spawn합니다.
     * @tparam T AActor를 상속받은 클래스
     * @return Spawn된 Actor의 포인터
     */
    template <typename T>
        requires std::derived_from<T, AActor>
    T* SpawnActor()
    {
        T* NewActor = FObjectFactory::ConstructObject<T>();
        AddActor(NewActor);
        // TODO: 일단 AddComponent에서 Component마다 초기화
        // 추후에 RegisterComponent() 만들어지면 주석 해제
        // Actor->InitializeComponents();
        //ActorsArray.Add(NewActor);
        PendingBeginPlayActors.Add(NewActor);
        return NewActor;
    }

protected:
    TArray<class AActor*> Actors;

private:
    /** Actor가 Spawn되었고, 아직 BeginPlay가 호출되지 않은 Actor들 */
    TArray<AActor*> PendingBeginPlayActors;
    AActor* SelectedActor = nullptr;
};

