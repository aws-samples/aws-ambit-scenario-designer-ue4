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

#include "SpawnInVolume.h"

#include "EngineUtils.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/ConstructorHelpers.h"

#include <AmbitUtils/MenuHelpers.h>

#include "Ambit/AmbitModule.h"
#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/AmbitWorldHelpers.h"

ASpawnInVolume::ASpawnInVolume()
{
    Box = CreateDefaultSubobject<UBoxComponent>("Box");
    Box->bHiddenInGame = true;
    Box->SetBoxExtent(FVector(150, 150, 0));
    Box->SetGenerateOverlapEvents(false);
    RootComponent = Box;

    const auto& IconAsset = ConstructorHelpers::FObjectFinder<UTexture2D>(AmbitIcon::KPath);
    // TODO: Replace with unique Bounding Spawner icon
    IconComponent->Sprite = IconAsset.Object;
    IconComponent->SetRelativeScale3D(FVector(0.5f));
    IconComponent->SetupAttachment(RootComponent);
}

void ASpawnInVolume::PostEditChangeProperty(
    FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FRotator& BoxRotation = Box->GetComponentTransform().Rotator();
    if (!FMath::IsNearlyEqual(BoxRotation.Roll, 0.f) ||
        !FMath::IsNearlyEqual(BoxRotation.Pitch, 0.f))
    {
        const FString& Message = FString::Printf(
            TEXT("The bound specifier of %s is not flat, which is not allowed. Please set roll (x) and pitch (y) values to 0."),
            *this->GetActorLabel());
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
    }
    // If the game is running, regenerate actors whenever the user changes parameters.
    if (GetWorld()->bBegunPlay)
    {
        GenerateActors();
    }
}

TSharedPtr<FSpawnInVolumeConfig>
ASpawnInVolume::GetConfiguration() const
{
    TSharedPtr<FSpawnInVolumeConfig> Config =
        Super::GetConfiguration<FSpawnInVolumeConfig>();
    Config->BoxExtent = Box->GetScaledBoxExtent();
    Config->bSnapToSurfaceBelow = bSnapToSurfaceBelow;
    return Config;
}

void ASpawnInVolume::Configure(
    const TSharedPtr<FSpawnInVolumeConfig>& Config)
{
    Super::Configure<FSpawnInVolumeConfig>(Config);
    Box->SetBoxExtent(Config->BoxExtent);
    bSnapToSurfaceBelow = Config->bSnapToSurfaceBelow;
}

TMap<FString, TArray<FTransform>> ASpawnInVolume::GenerateActors()
{
    TMap<FString, TArray<FTransform>> SpawnedObject;

    if (!AreParametersValid())
    {
        UE_LOG(LogAmbit, Warning, TEXT("%s: Parameters are invalid."), *this->GetActorLabel());
        return SpawnedObject;
    }

    const FRotator& BoxRotation = Box->GetComponentTransform().Rotator();
    if (!FMath::IsNearlyEqual(BoxRotation.Roll, 0.f) ||
        !FMath::IsNearlyEqual(BoxRotation.Pitch, 0.f))
    {
        const FString& Message = FString::Printf(
            TEXT("The bound specifier of %s is not flat, which is not allowed. Please set roll (x) and pitch (y) values to 0."),
            *this->GetActorLabel());
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
        return SpawnedObject;
    }
    UE_LOG(LogAmbit, Display, TEXT("%s: Generating actors."), *this->GetActorLabel())

    // Remove any previously spawned actors.
    DestroyGeneratedActors();

    const TArray<AActor*>& SurfacesToHit = AmbitWorldHelpers::GetActorsByMatchBy(
        MatchBy, SurfaceNamePattern, SurfaceTags);

    // Checks if the box component is not two-dimensional
    if (!FMath::IsNearlyEqual(Box->GetScaledBoxExtent().Z, 0.f))
    {
        UE_LOG(LogAmbit, Warning,
               TEXT("%s: The bound specifier is not a plane. SpawnInVolume will use the top of the box."),
            *this->GetActorLabel());
    }

    const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
        Box, SurfacesToHit, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax,
        RotationMin, RotationMax);

    if (Transforms.Num() > 0)
    {
        SpawnActorsAtTransforms(Transforms, SpawnedObject);
    }
    return SpawnedObject;
}

