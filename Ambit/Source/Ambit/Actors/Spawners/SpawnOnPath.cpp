//   Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include "SpawnOnPath.h"

#include "EngineUtils.h"
#include "Components/BillboardComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

#include "Ambit/AmbitModule.h"
#include "Ambit/Actors/SpawnerConfigs/SpawnOnPathConfig.h"
#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/AmbitWorldHelpers.h"

ASpawnOnPath::ASpawnOnPath()
{
    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
    Spline->bShouldVisualizeScale = true;
    Spline->bHiddenInGame = true;
    Spline->SetUnselectedSplineSegmentColor(FColor::Red);
    Spline->SetGenerateOverlapEvents(false);
    RootComponent = Spline;

    const auto& IconAsset = ConstructorHelpers::FObjectFinder<UTexture2D>(AmbitIcon::KPath);
    // TODO: Replace with unique Spline spawner icon
    IconComponent->Sprite = IconAsset.Object;
    IconComponent->SetRelativeScale3D(FVector(0.5f));
    IconComponent->SetupAttachment(RootComponent);
}

void ASpawnOnPath::PostEditChangeProperty(
    FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    // If the game is running, regenerate actors whenever the user changes parameters.
    if (GetWorld()->bBegunPlay)
    {
        GenerateActors();
    }
}

TSharedPtr<FSpawnOnPathConfig>
ASpawnOnPath::GetConfiguration() const
{
    TSharedPtr<FSpawnOnPathConfig> Config =
            Super::GetConfiguration<FSpawnOnPathConfig>();
    Config->bSnapToSurfaceBelow = bSnapToSurfaceBelow;
    TArray<FSplinePoint> SplinePoints;
    const int32 SplinePointCount = Spline->GetNumberOfSplinePoints();
    for (int32 i = 0; i < SplinePointCount; i++)
    {
        const FVector& ArriveVector = Spline->GetArriveTangentAtSplinePoint(i,
                                                                            ESplineCoordinateSpace::Local);
        const float InputKey = Spline->GetInputKeyAtDistanceAlongSpline(
            Spline->GetDistanceAlongSplineAtSplinePoint(i));
        const FVector& LeaveVector = Spline->GetLeaveTangentAtSplinePoint(i,
                                                                          ESplineCoordinateSpace::Local);
        const FVector& Location = Spline->GetLocationAtSplinePoint(i,
                                                                   ESplineCoordinateSpace::Local);
        const FRotator& Rotation = Spline->GetRotationAtSplineInputKey(i,
                                                                       ESplineCoordinateSpace::Local);
        const FVector& Scale = Spline->GetScaleAtSplinePoint(i);
        const TEnumAsByte<ESplinePointType::Type>& Type = Spline->GetSplinePointType(i);

        FSplinePoint Point(InputKey, Location, ArriveVector,
                           LeaveVector, Rotation, Scale, Type);
        SplinePoints.Add(Point);
    }
    Config->SplinePoints = SplinePoints;
    return Config;
}

void ASpawnOnPath::Configure(const
    TSharedPtr<FSpawnOnPathConfig>& Config)
{
    Super::Configure<FSpawnOnPathConfig>(Config);
    Spline->ClearSplinePoints();
    Spline->AddPoints(Config->SplinePoints);
    bSnapToSurfaceBelow = Config->bSnapToSurfaceBelow;
}

TMap<FString, TArray<FTransform>> ASpawnOnPath::GenerateActors()
{
    TMap<FString, TArray<FTransform>> SpawnedObjects;
    if (!AreParametersValid())
    {
        UE_LOG(LogAmbit, Warning, TEXT("%s: Parameters are invalid."), *this->GetActorLabel())
        return SpawnedObjects;
    }

    UE_LOG(LogAmbit, Display, TEXT("%s: Generating actors."), *this->GetActorLabel())

    // Remove any previously spawned actors.
    DestroyGeneratedActors();

    const TArray<AActor*>& SurfacesToHit = AmbitWorldHelpers::GetActorsByMatchBy(
        MatchBy, SurfaceNamePattern, SurfaceTags);

    const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
        Spline, SurfacesToHit, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax,
        RotationMin, RotationMax, bFollowSplineRotation);

    if (Transforms.Num() > 0)
    {
        SpawnActorsAtTransforms(Transforms, SpawnedObjects);
    }

    return SpawnedObjects;
}
