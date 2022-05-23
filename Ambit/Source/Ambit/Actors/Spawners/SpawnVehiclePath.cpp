//   Copyright 2022 Amazon.com, Inc. or its affiliates. All Rights Reserved.
//  
//   Licensed under the Apache License, Version 2.0 (the "License").
//   You may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//  
//       http://www.apache.org/licenses/LICENSE-2.0
//  
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#pragma once

#include "SpawnVehiclePath.h"

#include "DrawDebugHelpers.h"
#include "Components/BillboardComponent.h"
#include "Components/SplineComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "Ambit/Actors/SpawnedObjectConfigs/SpawnedVehiclePathConfig.h"
#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/AmbitWorldHelpers.h"
#include "Ambit/Vehicle/AmbitWheeledVehicleAIController.h"
#include "AmbitUtils/MenuHelpers.h"

ASpawnVehiclePath::ASpawnVehiclePath()
{
    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
    Spline->bHiddenInGame = true;

    RootComponent = Spline;

    //TO DO: Replace this with Path Generator specific icon
    IconComponent = CreateDefaultSubobject<UBillboardComponent>("Icon");
    const auto& IconAsset = ConstructorHelpers::FObjectFinder<UTexture2D>(AmbitIcon::KPath);
    IconComponent->Sprite = IconAsset.Object;
    IconComponent->SetRelativeScale3D(FVector(0.5f));
    IconComponent->SetupAttachment(RootComponent);
}

bool ASpawnVehiclePath::HasActorsToSpawn() const
{
    // TODO: Replace with a more cached way in the future. 
    const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
        Spline, DistanceBetweenWaypoints * 100);

    return Transforms.Num() > 0 && !VehicleToSpawn.Get()->GetPathName().IsEmpty();
}

bool ASpawnVehiclePath::AreParametersValid() const
{
    if (!IsValid(VehicleToSpawn))
    {
        const FString& Message = "Vehicle to spawn is not specified, which is not allowed.";
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
        return false;
    }
    if (DistanceBetweenWaypoints * 100 > Spline->GetSplineLength()) //Convert m to cm
    {
        const FString& Message =
                "Distance between generated Waypoints is greater than the total length of the path, which is not allowed";
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
        return false;
    }
    return true;
}

void ASpawnVehiclePath::BeginPlay()
{
    Super::BeginPlay();
    SpawnVehicle();
}

void ASpawnVehiclePath::SpawnVehicle() const
{
    if (!AreParametersValid())
    {
        UE_LOG(LogAmbit, Warning, TEXT("Parameters are invalid."))
        return;
    }

    const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
        Spline, DistanceBetweenWaypoints * 100);

    if (ShowDebugWaypoint)
    {
        for (const FTransform& Transform : Transforms)
        {
            DrawDebugSphere(GetWorld(), Transform.GetLocation(), 30, 16, FColor::Red, true);
        }
    }

    UWorld* World = GetWorld();

    const FTransform& InitTransform = Transforms[0];
    const FVector& InitLocation = InitTransform.GetLocation();
    const FRotator& InitRotation = InitTransform.Rotator();

    FActorSpawnParameters SpawnInfo;
    //Vehicle will auto position itself as long as the spawn point is not below the surface
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AWheeledVehicle* SpawnedVehicle = CastChecked<AWheeledVehicle>(World->SpawnActor(
        VehicleToSpawn.Get(), &InitLocation, &InitRotation, SpawnInfo));

    //Remove any controller if predefined come with the Vehicle
    AController* OldController = SpawnedVehicle->GetController();
    if (IsValid(OldController))
    {
        OldController->UnPossess();
        OldController->Destroy();
    }

    AAmbitWheeledVehicleAIController* NewController = CastChecked<AAmbitWheeledVehicleAIController>(
        World->SpawnActor(AAmbitWheeledVehicleAIController::StaticClass(), &InitLocation, &InitRotation));
    NewController->Possess(SpawnedVehicle);
    NewController->SetVehicleRoute(Transforms);
    //Convert km/h to cm/s due to physics engine using cm/s as unit
    NewController->SetSpeedLimit(SpeedLimit * 250.f / 9.f);
    NewController->SetLoopedPath(Spline->IsClosedLoop());
}

void ASpawnVehiclePath::GenerateSpawnedObjectConfiguration()
{
    USpawnedVehiclePathConfig* Config = NewObject<USpawnedVehiclePathConfig>();

    const FString& PathName = VehicleToSpawn.Get()->GetPathName();

    Config->SpawnedVehicle = PathName;

    Config->SpeedLimit = SpeedLimit * 250.f / 9.f;

    const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
        Spline, DistanceBetweenWaypoints * 100);

    TArray<FVector> Waypoints;
    for (const FTransform& Transform : Transforms)
    {
        Waypoints.Emplace(Transform.GetLocation());
    }

    Config->Waypoints = Waypoints;

    auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);
    OnSpawnedObjectConfigCompleted.ExecuteIfBound(FinalConfig, true);
}

TSharedPtr<FSpawnVehiclePathConfig> ASpawnVehiclePath::GetConfiguration() const
{
    TSharedPtr<FSpawnVehiclePathConfig> Config = MakeShareable(new FSpawnVehiclePathConfig);
    TArray<FSplinePoint> SplinePoints;
    const int32 SplinePointCount = Spline->GetNumberOfSplinePoints();
    for (int32 i = 0; i < SplinePointCount; i++)
    {
        const FVector& ArriveVector = Spline->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
        const float InputKey = Spline->GetInputKeyAtDistanceAlongSpline(Spline->GetDistanceAlongSplineAtSplinePoint(i));
        const FVector& LeaveVector = Spline->GetLeaveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
        const FVector& Location = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
        const FRotator& Rotation = Spline->GetRotationAtSplineInputKey(i, ESplineCoordinateSpace::Local);
        const FVector& Scale = Spline->GetScaleAtSplinePoint(i);
        const TEnumAsByte<ESplinePointType::Type>& Type = Spline->GetSplinePointType(i);

        FSplinePoint Point(InputKey, Location, ArriveVector, LeaveVector, Rotation, Scale, Type);
        SplinePoints.Add(Point);
    }
    Config->SplinePoints = SplinePoints;
    Config->CloseLoop = Spline->IsClosedLoop();

    Config->SpeedLimit = SpeedLimit;
    Config->DistanceBetweenWaypoints = DistanceBetweenWaypoints;
    Config->ShowDebugWaypoint = ShowDebugWaypoint;
    Config->VehicleToSpawn = VehicleToSpawn;

    Config->SpawnerLocation = this->GetActorLocation();
    Config->SpawnerRotation = this->GetActorRotation();
    return Config;
}

void ASpawnVehiclePath::Configure(const TSharedPtr<FSpawnVehiclePathConfig>& Config)
{
    Spline->ClearSplinePoints();
    Spline->AddPoints(Config->SplinePoints);
    Spline->SetClosedLoop(Config->CloseLoop);

    SpeedLimit = Config->SpeedLimit;
    DistanceBetweenWaypoints = Config->DistanceBetweenWaypoints;
    ShowDebugWaypoint = Config->ShowDebugWaypoint;
    VehicleToSpawn = Config->VehicleToSpawn;
}
