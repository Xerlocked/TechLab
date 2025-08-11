#include "ADodge.h"

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleSDK.h"
#include "vehicle/PxVehicleUpdate.h"
#include "vehicle/PxVehicleDrive4W.h"
#include "vehicle/PxVehicleUtilSetup.h"

#include "LaunchEngineLoop.h"
#include "Components/PrimitiveComponents/MeshComponents/StaticMeshComponents/StaticMeshComponent.h"
#include "Engine/FLoaderOBJ.h"
#include "Engine/World.h"
#include "Delegates/Delegate.impl.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Classes/Components/Mesh/StaticMesh.h"
#include "PhysicsEngine/PhysScene_PhysX.h"
#include "PhysicsEngine/PhysXSDKManager.h"

using namespace physx;

// 주행 가능한 바닥 평면 생성 함수
void ADodge::CreateDrivablePlane(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{
    // 바닥 평면 액터 생성
   // 1. 박스 지면의 크기 정의 (반경 기준: halfExtents)
    // 예: 가로 1000, 세로 1000, 두께 20 크기의 박스
    const physx::PxVec3 groundBoxHalfExtents(500.0f, 500.0f, 10.0f);

    // 2. 박스 지면의 위치 및 방향 설정
    // 박스의 윗면이 Z=0에 오도록 하려면, 박스 중심의 Z좌표는 -groundBoxHalfExtents.z 가 되어야 합니다.
    physx::PxTransform groundPose(
        physx::PxVec3(0.0f, 0.0f, -groundBoxHalfExtents.z), // 박스 중심 위치
        physx::PxQuat(physx::PxIdentity)                    // 회전 없음
    );

    // 3. PxRigidStatic 액터 생성 (움직이지 않는 고정된 박스)
    physx::PxRigidStatic* groundActor = gPhysics->createRigidStatic(groundPose);
    if (!groundActor) {
        // 오류 처리: 액터 생성 실패
        std::cerr << "Failed to create ground PxRigidStatic actor!" << std::endl;
        return;
    }

    // 4. 박스 모양(PxShape) 생성
    // gMaterial은 미리 생성되어 있어야 합니다 (예: BeginPlay에서 this->mMaterial = PxPhysicsSDK->createMaterial(...);)
    physx::PxShape* boxShape = gPhysics->createShape(
        physx::PxBoxGeometry(groundBoxHalfExtents),
        *gMaterial // 공용 머티리얼 사용
    );
    if (!boxShape) {
        // 오류 처리: 셰이프 생성 실패
        std::cerr << "Failed to create ground PxBoxGeometry shape!" << std::endl;
        groundActor->release(); // 이미 생성된 액터는 정리
        return;
    }

    // 5. 필터 데이터 설정
    // 레이캐스트가 이 바닥을 '주행 가능 표면'으로 인식하도록 설정
    physx::PxFilterData groundQueryFilterData;
    groundQueryFilterData.word0 = DRIVABLE_SURFACE; // 이 셰이프의 타입은 '주행 가능 표면'

    // 물리적 충돌을 위한 필터 데이터 설정
    physx::PxFilterData groundSimFilterData;
    groundSimFilterData.word0 = DRIVABLE_SURFACE;   // 이 셰이프의 타입
    groundSimFilterData.word1 = VEHICLE;            // '차량' 타입의 셰이프와 충돌하도록 설정
                                                    // (VEHICLE은 차량 셰이프의 word0에 설정된 값)

    boxShape->setQueryFilterData(groundQueryFilterData);   // 레이캐스트 쿼리용
    boxShape->setSimulationFilterData(groundSimFilterData); // 물리 시뮬레이션(충돌)용

    // 6. 액터에 셰이프 부착
    groundActor->attachShape(*boxShape);

    // 7. 셰이프 참조 카운트 감소 (액터가 소유권을 가짐)
    boxShape->release();

    // 8. 씬에 액터 추가
    gScene->addActor(*groundActor);
}

// 간단한 차량 설정 함수 (PhysX 4.1 기준)
void ADodge::SetupVehicle(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{
    physx::PxInitVehicleSDK(*gPhysics);
    physx::PxVehicleSetBasisVectors(physx::PxVec3(0, 0, 1), physx::PxVec3(1, 0, 0));
    physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);

    physx::PxTransform startPose(physx::PxVec3(0, 0, 4.0f), physx::PxQuat(physx::PxIdentity));
    physx::PxRigidDynamic* vehicleActor = gPhysics->createRigidDynamic(startPose);
    VehicleActor = vehicleActor; 

    const float chassisMass = 1500.0f;
    const physx::PxVec3 chassisBoxHalfExtents(8.0f, 5.0f, 0.4f);
    physx::PxShape* chassisShape = gPhysics->createShape(physx::PxBoxGeometry(chassisBoxHalfExtents), *gMaterial);

    physx::PxFilterData chassisFilterData;
    chassisFilterData.word0 = VEHICLE; 
    chassisShape->setSimulationFilterData(chassisFilterData); 
    chassisShape->setQueryFilterData(chassisFilterData);      
    vehicleActor->attachShape(*chassisShape);
    chassisShape->release(); 

    vehicleActor->setMass(chassisMass);
    vehicleActor->setMassSpaceInertiaTensor(physx::PxVec3( 
        (chassisBoxHalfExtents.y * chassisBoxHalfExtents.y + chassisBoxHalfExtents.z * chassisBoxHalfExtents.z) * chassisMass / 3.0f,
        (chassisBoxHalfExtents.x * chassisBoxHalfExtents.x + chassisBoxHalfExtents.z * chassisBoxHalfExtents.z) * chassisMass / 3.0f,
        (chassisBoxHalfExtents.x * chassisBoxHalfExtents.x + chassisBoxHalfExtents.y * chassisBoxHalfExtents.y) * chassisMass / 3.0f
    ));
    
    const physx::PxU32 numWheels = 4; 
    const float wheelRadius = 1.0f;
    const float wheelMass = 120.0f;
    const float wheelWidth = 0.5f;
    const float sprungMass = chassisMass / numWheels;
    
    physx::PxVec3 WheelOffsets[numWheels] = {
        physx::PxVec3( chassisBoxHalfExtents.x - wheelRadius,  chassisBoxHalfExtents.y - wheelWidth, -wheelRadius - 0.2f), // 앞오른쪽
        physx::PxVec3( chassisBoxHalfExtents.x - wheelRadius, -chassisBoxHalfExtents.y + wheelWidth, -wheelRadius - 0.2f), // 앞왼쪽
        physx::PxVec3(-chassisBoxHalfExtents.x + wheelRadius,  chassisBoxHalfExtents.y - wheelWidth, -wheelRadius - 0.2f), // 뒤오른쪽
        physx::PxVec3(-chassisBoxHalfExtents.x + wheelRadius, -chassisBoxHalfExtents.y + wheelWidth, -wheelRadius - 0.2f), // 뒤왼쪽
    };
    
    for (const auto& WheelOffset : WheelOffsets)
    {
        physx::PxShape* wheelShape = gPhysics->createShape(physx::PxSphereGeometry(wheelRadius), *gMaterial); 
        wheelShape->setLocalPose(physx::PxTransform(WheelOffset, PxQuat(PxPi / 2, PxVec3(0, 1, 0))));

        physx::PxFilterData wheelFilterData;
        wheelFilterData.word0 = VEHICLE; 
        wheelFilterData.word1 = DRIVABLE_SURFACE;
        wheelShape->setSimulationFilterData(wheelFilterData);
        wheelShape->setQueryFilterData(wheelFilterData); 

        vehicleActor->attachShape(*wheelShape);
        wheelShape->release();
    }

    vehicleActor->setCMassLocalPose(physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f), physx::PxQuat(physx::PxIdentity)));
    const physx::PxVec3 chassisCMOffset = vehicleActor->getCMassLocalPose().p;
    
    physx::PxVehicleWheelsSimData* wheelsSimData = physx::PxVehicleWheelsSimData::allocate(numWheels);
    
    for (physx::PxU32 i = 0; i < numWheels; i++)
    {
        physx::PxVehicleWheelData wheelData;
        wheelData.mRadius = wheelRadius;
        wheelData.mMass = wheelMass;
        wheelData.mMOI = 0.5f * wheelMass * wheelRadius * wheelRadius; 
        wheelData.mWidth = wheelWidth;
        wheelData.mDampingRate = 0.25f;
        wheelData.mMaxSteer = (i < 2) ? (physx::PxPi / 6.0f) : 0.0f;

        physx::PxVehicleSuspensionData suspData;
        suspData.mSpringStrength = 10000.0f;
        suspData.mSpringDamperRate = 1500.0f;
        suspData.mMaxCompression = 0.3f;
        suspData.mMaxDroop = 0.1f;
        suspData.mSprungMass = sprungMass;

        physx::PxVehicleTireData tireData;
        tireData.mLongitudinalStiffnessPerUnitGravity = 1000.0f; 
        tireData.mLatStiffX = 10.0f; 
        tireData.mLatStiffY = 1000.0f;
        tireData.mFrictionVsSlipGraph[0][0] = 0.0f; tireData.mFrictionVsSlipGraph[0][1] = 1.0f; 
        tireData.mFrictionVsSlipGraph[1][0] = 0.5f; tireData.mFrictionVsSlipGraph[1][1] = 0.85f;
        tireData.mFrictionVsSlipGraph[2][0] = 1.0f; tireData.mFrictionVsSlipGraph[2][1] = 0.7f;

        wheelsSimData->setWheelData(i, wheelData);
        wheelsSimData->setSuspensionData(i, suspData);
        wheelsSimData->setTireData(i, tireData);

        wheelsSimData->setWheelCentreOffset(i, WheelOffsets[i] - chassisCMOffset);
        wheelsSimData->setTireForceAppPointOffset(i, WheelOffsets[i] - chassisCMOffset); 
        wheelsSimData->setSuspTravelDirection(i, physx::PxVec3(0, 0, -1)); 

        physx::PxFilterData wheelQueryFilterData;
        wheelQueryFilterData.word0 = 0;
        wheelQueryFilterData.word1 = DRIVABLE_SURFACE; 
        wheelsSimData->setSceneQueryFilterData(i, wheelQueryFilterData);
        wheelsSimData->setWheelShapeMapping(i, 1+ i); 
    }

    physx::PxVehicleDriveSimData4W driveSimData;

    physx::PxVehicleEngineData engineData;
    engineData.mPeakTorque = 300.0f; 
    engineData.mMaxOmega = 600.0f;   
    engineData.mTorqueCurve.addPair(0.0f, 0.8f); 
    engineData.mTorqueCurve.addPair(0.5f, 1.0f);
    engineData.mTorqueCurve.addPair(1.0f, 0.8f);
    driveSimData.setEngineData(engineData);

    physx::PxVehicleGearsData gearsData;
    gearsData.mRatios[physx::PxVehicleGearsData::eFIRST] = 3.0f;
    gearsData.mRatios[physx::PxVehicleGearsData::eSECOND] = 1.8f;
    gearsData.mRatios[physx::PxVehicleGearsData::eTHIRD] = 1.2f;
    gearsData.mRatios[physx::PxVehicleGearsData::eREVERSE] = -2.5f;
    gearsData.mNbRatios = 3; 
    driveSimData.setGearsData(gearsData);

    physx::PxVehicleClutchData clutchData;
    clutchData.mStrength = 20.0f;
    driveSimData.setClutchData(clutchData);

    physx::PxVehicleAckermannGeometryData ackermannData; 
    ackermannData.mAccuracy = 1.0f;
    ackermannData.mAxleSeparation = physx::PxAbs(WheelOffsets[0].x - WheelOffsets[2].x);
    ackermannData.mFrontWidth = physx::PxAbs(WheelOffsets[0].y - WheelOffsets[1].y);
    ackermannData.mRearWidth = physx::PxAbs(WheelOffsets[2].y - WheelOffsets[3].y);
    driveSimData.setAckermannGeometryData(ackermannData);

    physx::PxVehicleDifferential4WData diffData; 
    diffData.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD; 
    diffData.mFrontRearSplit = 0.45f; 
    driveSimData.setDiffData(diffData);

    gVehicle4W = physx::PxVehicleDrive4W::allocate(numWheels);
    gVehicle4W->setup(gPhysics, vehicleActor, *wheelsSimData, driveSimData, 0);
    gVehicle4W->mDriveDynData.setUseAutoGears(true);
    gVehicle4W->setToRestState();

    wheelsSimData->free(); 

    gScene->addActor(*vehicleActor);
}

// 서스펜션 레이캐스트 배치 쿼리 설정 (PhysX 4.1)
void ADodge::SetupSuspensionRaycastBatchQuery(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{
    // 1. PxVehicleSuspensionRaycasts가 사용할 결과 버퍼 할당
    //    (각 바퀴에 대한 PxWheelQueryResult 정보를 저장)
    gVehicleWheelQueryResults[0].wheelQueryResults = new physx::PxWheelQueryResult[4];
    gVehicleWheelQueryResults[0].nbWheelQueryResults = 4; // 바퀴 수 설정

    // 2. PxBatchQuery가 사용할 원시 레이캐스트 결과 버퍼 할당
    gRawRaycastQueryResults = new physx::PxRaycastQueryResult[4];
    gRawRaycastHits = new physx::PxRaycastHit[4]; // 각 쿼리 결과당 하나의 히트 정보 저장

    // 각 PxRaycastQueryResult가 PxRaycastHit 버퍼의 해당 요소를 가리키도록 설정
    for (physx::PxU32 i = 0; i < 4; ++i)
    {
        gRawRaycastQueryResults[i].touches = &gRawRaycastHits[i]; // PxRaycastHit 저장 위치 연결
        gRawRaycastQueryResults[i].nbTouches = 0;                // 초기에는 히트 없음
    }

    // 3. PxBatchQueryDesc 설정
    physx::PxBatchQueryDesc sqDesc(4, 0, 0); // (최대 레이캐스트, 오버랩, 스윕 수)

    // PxBatchQuery가 원시 레이캐스트 결과를 저장할 메모리 영역 지정
    sqDesc.queryMemory.userRaycastResultBuffer = gRawRaycastQueryResults;
    // 터치 버퍼도 동일하게 사용 가능 (가장 가까운 블로킹 히트만 필요한 경우)
    sqDesc.queryMemory.userRaycastTouchBuffer = gRawRaycastHits;
    sqDesc.queryMemory.raycastTouchBufferSize = 4;
    
    sqDesc.preFilterShader = nullptr;

    // 5. PxBatchQuery 객체 생성
    gSuspensionRaycastBatchQuery = gScene->createBatchQuery(sqDesc);
    if (!gSuspensionRaycastBatchQuery)
    {
        std::cerr << "gScene->createBatchQuery failed!" << std::endl;
    }
}

// 마찰 쌍 설정 함수
void ADodge::SetupFrictionPairs()
{
    // 1개의 타이어 타입, 1개의 표면 타입 가정
    const physx::PxU32 numTireTypes = 1;
    const physx::PxU32 numSurfaceTypes = 1;

    physx::PxMaterial* surfaceMaterials[numSurfaceTypes];
    surfaceMaterials[0] = gMaterial; // 바닥 평면에 사용된 공용 머티리얼

    physx::PxVehicleDrivableSurfaceType drivableSurfaceTypes[numSurfaceTypes];
    drivableSurfaceTypes[0].mType = DRIVABLE_SURFACE; // FilterGroup에서 정의한 값

    gFrictionPairs = physx::PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(numTireTypes, numSurfaceTypes);
    gFrictionPairs->setup(numTireTypes, numSurfaceTypes, (const physx::PxMaterial**)surfaceMaterials, drivableSurfaceTypes);

    // 타이어 타입 0과 표면 타입 0 (DRIVABLE_SURFACE) 간의 마찰 계수 설정
    gFrictionPairs->setTypePairFriction(0, 0, 1.1f); // (tireType, surfaceType, frictionValue)
}

void ADodge::CleanupVehicleSDK()
{
    if (gVehicle4W)
    {
        gVehicle4W->free();
        gVehicle4W = NULL;
    }

    if (gSuspensionRaycastBatchQuery)
    {
        gSuspensionRaycastBatchQuery->release();
        gSuspensionRaycastBatchQuery = NULL;
    }

    if (gFrictionPairs)
    {
        gFrictionPairs->release();
        gFrictionPairs = NULL;
    }

    physx::PxCloseVehicleSDK();
}

void ADodge::UpdateVehicle(float DeltaTime, const physx::PxScene* PxScene)
{
        // 필수 객체들이 유효한지 확인
    if (!gVehicle4W || !gSuspensionRaycastBatchQuery || !gFrictionPairs || !PxScene || !VehicleActor)
    {
        return;
    }

    // 1. 차량 입력 설정 (멤버 변수 mThrottleInput, mSteerInput 사용)
    //    실제 게임에서는 키보드, 컨트롤러 입력 등을 받아 이 변수들을 업데이트합니다.
    // 예시: 항상 약간의 가속과 직진
    // mThrottleInput = 0.3f; // 테스트용 가속 값
    // mSteerInput = 0.0f;    // 테스트용 조향 값 (0: 직진)

    gVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, ThrottleInput);
    gVehicle4W->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, 0.0f);
    
    // 조향 처리 (양수: 오른쪽, 음수: 왼쪽)
    if (SteerInput > 0.001f) // 작은 Deadzone 고려
    {
        gVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, SteerInput);
        gVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, 0.0f);
    }
    else if (SteerInput < -0.001f)
    {
        gVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, 0.0f);
        gVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, -SteerInput);
    }
    else
    {
        gVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, 0.0f);
        gVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, 0.0f);
    }
    // 필요에 따라 브레이크 (eANALOG_INPUT_BRAKE), 핸드브레이크 (eANALOG_INPUT_HANDBRAKE) 입력 설정


    // 2. 서스펜션 레이캐스트 실행
    // PxVehicleWheels 포인터 배열 준비 (현재 차량 1대)
    physx::PxVehicleWheels* vehicles[1] = {gVehicle4W};
    
    // 2. 서스펜션 레이캐스트를 위한 배치 쿼리 실행
    gSuspensionRaycastBatchQuery->execute();

    // 3. 배치 쿼리 결과를 사용하여 서스펜션 상태 업데이트
    //    사용자님이 제공한 함수 원형에 맞춰 호출합니다.
    //    이 함수는 mRawRaycastQueryResults를 읽어서 차량 내부의 서스펜션 데이터를 업데이트합니다.
    PxVehicleSuspensionRaycasts(
        gSuspensionRaycastBatchQuery, // 배치 쿼리 객체 (결과를 읽기 위함 또는 NULL일 수 있음)
        1,                            // nbVehicles
        vehicles,                     // PxVehicleWheels** 배열
        4,         // 처리할 sceneQueryResults의 수 (총 바퀴 수)
        gRawRaycastQueryResults       // 입력: PxBatchQuery 실행 후 채워진 PxRaycastQueryResult 배열
                                      // 이 버퍼는 SetupSuspensionRaycastBatchQuery에서 설정됨
    );
    
    gVehicle4W->mWheelsDynData.setWheelRotationSpeed(0, 10.0f);

    // 3. 차량 상태 업데이트 (PhysX 4.1 방식)
    // PxVehicleUpdates 함수는 중력 값, 마찰 쌍 정보, 차량 배열, 휠 쿼리 결과 등을 필요로 합니다.
    const physx::PxVec3 gravity = PxScene->getGravity();
    physx::PxVehicleUpdates(DeltaTime, gravity, *gFrictionPairs, 1, vehicles, gVehicleWheelQueryResults);

    for (int i = 0; i < 4; ++i)
    {
        auto& q = gVehicleWheelQueryResults[0].wheelQueryResults[i];
        if (!q.isInAir)
        {
            std::cout << "Wheel[" << i << "] inAir: " << q.isInAir << ", suspOffset: " << q.suspJounce << std::endl;
        }
    }
    
    // 4. (선택 사항) PhysX 액터의 트랜스폼을 언리얼 액터의 트랜스폼으로 동기화
    if (VehicleActor)
    {
        const physx::PxTransform PxActorGlobalPose = VehicleActor->getGlobalPose();
        
    }
}


ADodge::ADodge()
    : Super()
{
    // SetActorLabel(TEXT("OBJ_DODGE"));
    UStaticMeshComponent* MeshComp = GetStaticMeshComponent();
    UStaticMesh* StaticMesh = UAssetManager::Get().Get<UStaticMesh>(TEXT("Dodge"));
    //FManagerOBJ::CreateStaticMesh("Assets/Dodge/Dodge.obj");
    MeshComp->SetStaticMesh(StaticMesh);
    FunctionRegistry()->RegisterMemberFunction<ThisClass>("TestRotate", &ADodge::TestRotate);
    FunctionRegistry()->RegisterMemberFunction<ThisClass>("TestTranslate", &ADodge::TestTranslate);

}


void ADodge::BeginPlay()
{
    Super::BeginPlay();
    for (auto& function : FunctionRegistry()->GetRegisteredFunctions())
    {
        std::cout << function.Key.ToString();
    }
    // TestDelegate.AddUObject(this, &ADodge::test);
    // TestDelegate.AddLambda([this]
    // {
    //     SetActorLocation(GetActorLocation() + FVector(0.1,0,0));
    // });

    PxPhysics& Physics           = *FPhysXSDKManager::GetInstance().PxSDKInstance;
    PxScene* Scene               = GetWorld()->GetPhysicsScene()->GetPxScene();
    gMaterial = Physics.createMaterial(0.5f, 0.5f, 0.6f);

    CreateDrivablePlane(&Physics, Scene);
    SetupVehicle(&Physics, Scene);
    mQueryFilterCallback.setVehicleActorToIgnore(VehicleActor);
    SetupSuspensionRaycastBatchQuery(&Physics, Scene);
    SetupFrictionPairs();

    
    // PxPhysics& Physics           = *FPhysXSDKManager::GetInstance().PxSDKInstance;
    // PxScene* Scene               = GetWorld()->GetPhysicsScene()->GetPxScene();
    //
    // // 2. Vehicle SDK 초기화
    // physx::PxInitVehicleSDK(Physics);
    // physx::PxVehicleSetBasisVectors(physx::PxVec3(0, 0, 1), physx::PxVec3(1, 0, 0));
    // physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);
    //
    // // 3. 휠 및 차체 물리 속성 정의
    // const PxU32 NumWheels       = 4;
    // const PxReal WheelRadius     = 0.50f;
    // const PxVec3  ChassisExtents(2.0f, 1.0f, 0.5f);
    //
    // // 4. ChassisData 설정
    // PxVehicleChassisData ChassisData;
    // ChassisData.mMass           = 1500.0f;
    // ChassisData.mMOI            = PxVec3(1500.0f, 1500.0f, 1500.0f);
    // ChassisData.mCMOffset       = PxVec3(0.0f, 0.5f, 0.0f);
    //
    // // 5. 공통 Material 생성
    // PxMaterial* WheelMaterial    = Physics.createMaterial(0.8f, 0.8f, 0.6f);
    // PxMaterial* ChassisMaterial  = Physics.createMaterial(0.6f, 0.7f, 0.5f);
    //
    // // 6. Primitive Geometry 정의
    // PxSphereGeometry WheelGeom(WheelRadius);
    // PxBoxGeometry    ChassisGeom(ChassisExtents);
    //
    // // 7. 필터 데이터 설정
    // PxFilterData WheelFilterData;  
    // WheelFilterData.word0         = 0x0001; // VehicleWheelGroup
    // WheelFilterData.word1         = 0xFFFF; // CollideWithAll
    //
    // PxFilterData ChassisFilterData;
    // ChassisFilterData.word0       = 0x0002; // VehicleChassisGroup
    // ChassisFilterData.word1       = 0xFFFF; // CollideWithAll
    //
    // // 8. Vehicle Actor 생성
    // VehicleActor = Physics.createRigidDynamic(PxTransform(PxIdentity));
    //
    // const PxReal WheelXOffset = 1.5f;      // 전후 거리
    // const PxReal WheelYOffset = 1.0f;      // 좌우 거리
    // const PxReal WheelZOffset = -WheelRadius * 0.8f; // 높이
    //
    // // 9. Wheel Shape 추가
    // for (PxU32 WheelIndex = 0; WheelIndex < NumWheels; ++WheelIndex)
    // {
    //     PxShape* WheelShape = PxRigidActorExt::createExclusiveShape(
    //         *VehicleActor,
    //         WheelGeom,
    //         *WheelMaterial
    //     );
    //
    //     // 로컬 위치는 차량 설계에 따라 조정
    //     PxReal WheelX = (WheelIndex / 2 == 0 ?  WheelXOffset : -WheelXOffset);  // 앞(+) / 뒤(-)
    //     PxReal WheelY = (WheelIndex % 2 == 0 ?  WheelYOffset : -WheelYOffset);  // 오른쪽(+) / 왼쪽(-)
    //     PxReal WheelZ = WheelZOffset;                                           // 위
    //
    //     WheelShape->setLocalPose(PxTransform(PxVec3(WheelX, WheelY, WheelZ), PxQuat(PxIdentity)));
    //
    //     WheelShape->setQueryFilterData(WheelFilterData);
    //     WheelShape->setSimulationFilterData(WheelFilterData);
    // }
    //
    // // 10. Chassis Shape 추가
    // PxShape* ChassisShape = PxRigidActorExt::createExclusiveShape(
    //     *VehicleActor,
    //     ChassisGeom,
    //     *ChassisMaterial
    // );
    // ChassisShape->setLocalPose(
    //     PxTransform(
    //         PxVec3(0.0, 0.0f, ChassisExtents.z),
    //         PxQuat(PxIdentity)
    //     )
    // );
    // ChassisShape->setQueryFilterData(ChassisFilterData);
    // ChassisShape->setSimulationFilterData(ChassisFilterData);
    //
    // // 11. 물리 속성 적용
    // VehicleActor->setMass(ChassisData.mMass);
    // VehicleActor->setMassSpaceInertiaTensor(ChassisData.mMOI);
    // VehicleActor->setCMassLocalPose(
    //     PxTransform(ChassisData.mCMOffset, PxQuat(PxIdentity))
    // );
    //
    // // 12. 씬에 Actor 추가
    // Scene->addActor(*VehicleActor);



    
}

void ADodge::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TestDelegate.Broadcast();
    //
    // const PxVec3 ForwardVector = PxVec3(1.0f, 0.0f, 0.0f);
    // const PxReal  ThrottleForce = 1.0f; // 필요에 따라 조절

    // 2) 힘 적용 (가속도로 동작)
    // VehicleActor->addForce(ForwardVector * ThrottleForce, PxForceMode::eACCELERATION);

    AutoBehaviorTime += DeltaTime;
    if (AutoBehaviorTime <= 3.0f) // 0~3초: 앞으로 직진
    {
        ThrottleInput = 0.5f; // 50% 가속
        SteerInput = 0.0f;    // 직진
    }
    else if (AutoBehaviorTime <= 6.0f) // 3~6초: 앞으로 가면서 오른쪽으로 조향
    {
        ThrottleInput = 0.4f; // 약간 감속하며 조향
        SteerInput = 0.7f;    // 오른쪽으로 70% 조향
    }
    else if (AutoBehaviorTime <= 9.0f) // 6~9초: 앞으로 가면서 왼쪽으로 조향
    {
        ThrottleInput = 0.4f;
        SteerInput = -0.7f;   // 왼쪽으로 70% 조향
    }
    else if (AutoBehaviorTime <= 11.0f) // 9~11초: 다시 직진 (약간 감속)
    {
        ThrottleInput = 0.2f;
        SteerInput = 0.0f;
    }
    else if (AutoBehaviorTime <= 12.0f) // 11~12초: 정지 (브레이크)
    {
        ThrottleInput = -1.0f; // 강한 브레이크 (또는 PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE 사용)
        // mVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, 1.0f);
        // mThrottleInput = 0.0f; // 가속 페달에서 발 떼기
        SteerInput = 0.0f;
    }
    else // 12초 이후: 입력 없음 (정지 상태 유지 또는 다음 패턴)
    {
        ThrottleInput = 0.0f;
        SteerInput = 0.0f;
        // mVehicle4W->mDriveDynData.setAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, 1.0f); // 계속 정지
    }
    
    // PhysX 씬이 유효한 경우 차량 업데이트 함수 호출
    UWorld* World = GetWorld();
    if (World && World->GetPhysicsScene())
    {
        FPhysScene* PhysScene = World->GetPhysicsScene();
        if (PhysScene)
        {
            physx::PxScene* PxScene = PhysScene->GetPxScene();
            if (PxScene)
            {
                UpdateVehicle(DeltaTime, PxScene); // 이전 답변에서 제공된 UpdateVehicle 함수
            }
        }
    }
    
}

void ADodge::Destroyed()
{
    Super::Destroyed();
    if (gVehicleWheelQueryResults[0].wheelQueryResults)
    {
        delete[] gVehicleWheelQueryResults[0].wheelQueryResults;
        gVehicleWheelQueryResults[0].wheelQueryResults = nullptr;
    }
    if (gRawRaycastQueryResults)
    {
        delete[] gRawRaycastQueryResults;
        gRawRaycastQueryResults = nullptr;
    }
    if (gRawRaycastHits)
    {
        delete[] gRawRaycastHits;
        gRawRaycastHits = nullptr;
    }
    if (gMaterial)
    {
        gMaterial->release();
        gMaterial = nullptr;
    }
}

void ADodge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

bool ADodge::Destroy()
{
    CleanupVehicleSDK();
    return Super::Destroy();
}

void ADodge::TestTranslate()
{
    SetActorLocation(GetActorLocation() + FVector(0.01,0,0));
}

void ADodge::TestRotate()
{
    SetActorRotation(GetActorRotation() + FRotator(0.01,0,0));
}

void ADodge::PostDuplicate()
{
    Super::PostDuplicate();
    // TODO: PIE world 받아오는 다른 방법 생각해보기 지금은 하드코딩
    // 아직 Duplicate 중이라 GetWorld가 Editor World를 뱉음
    // TestDelegate = TestDelegate.Duplicate(GetWorld()->GetPIEWorld()->GetLevel()->GetDuplicatedObjects());
    TestDelegate = TestDelegate.Duplicate(GetWorld()->GetLevel()->GetDuplicatedObjects());
}

void ADodge::LoadAndConstruct(const TArray<std::unique_ptr<FActorComponentInfo>>& InfoArray)
{
    AActor::LoadAndConstruct(InfoArray);
}

FActorInfo ADodge::GetActorInfo()
{
    return AActor::GetActorInfo();
}

PxRigidDynamic* ADodge::createVehicleActor
(const PxVehicleChassisData& chassisData,
 PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels, const PxFilterData& wheelSimFilterData,
 PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, const PxFilterData& chassisSimFilterData,
 PxPhysics& physics)
{
        //We need a rigid body actor for the vehicle.
        //Don't forget to add the actor to the scene after setting up the associated vehicle.
        PxRigidDynamic* vehActor = physics.createRigidDynamic(PxTransform(PxIdentity));

        //Wheel and chassis query filter data.
        //Optional: cars don't drive on other cars.

        //Add all the wheel shapes to the actor.
        for(PxU32 i = 0; i < numWheels; i++)
        {
            PxFilterData wheelQryFilterData;
            PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
                PxShape* wheelShape=PxRigidActorExt::createExclusiveShape(*vehActor, geom, *wheelMaterials[i]);
                wheelShape->setQueryFilterData(wheelQryFilterData);
                wheelShape->setSimulationFilterData(wheelSimFilterData);
                wheelShape->setLocalPose(PxTransform(PxIdentity));
        }

        //Add the chassis shapes to the actor.
        for(PxU32 i = 0; i < numChassisMeshes; i++)
        {
            PxFilterData chassisQryFilterData;
            PxShape* chassisShape=PxRigidActorExt::createExclusiveShape(*vehActor, PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
                chassisShape->setQueryFilterData(chassisQryFilterData);
                chassisShape->setSimulationFilterData(chassisSimFilterData);
                chassisShape->setLocalPose(PxTransform(PxIdentity));
        }

        vehActor->setMass(chassisData.mMass);
        vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
        vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset,PxQuat(PxIdentity)));

        return vehActor;
}