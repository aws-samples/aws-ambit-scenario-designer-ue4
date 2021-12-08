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

#include "SpawnOnSurface.h"

#include "EngineUtils.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

#include "Ambit/AmbitModule.h"
#include "Ambit/Actors/SpawnerConfigs/SpawnerBaseConfig.h"
#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/AmbitWorldHelpers.h"

ASpawnOnSurface::ASpawnOnSurface()
{
    const auto& IconAsset = ConstructorHelpers::FObjectFinder<UTexture2D>(AmbitIcon::KPath);
    IconComponent->Sprite = IconAsset.Object;
    IconComponent->SetRelativeScale3D(FVector(0.5f));

    RootComponent = IconComponent;
}

void ASpawnOnSurface::PostEditChangeProperty(
    FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // If the game is running, regenerate actors whenever the user changes parameters.
    if (GetWorld()->bBegunPlay)
    {
        GenerateActors();
    }
}

TSharedPtr<FSpawnerBaseConfig> ASpawnOnSurface::GetConfiguration() const
{
    return Super::GetConfiguration<FSpawnerBaseConfig>();
}

void ASpawnOnSurface::Configure(const
    TSharedPtr<FSpawnerBaseConfig>& Config)
{
    Super::Configure<FSpawnerBaseConfig>(Config);
}

TMap<FString, TArray<FTransform>> ASpawnOnSurface::GenerateActors()
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

    const TArray<AActor*>& SurfaceActors = AmbitWorldHelpers::GetActorsByMatchBy(
        MatchBy, SurfaceNamePattern, SurfaceTags);

    UE_LOG(LogAmbit, Display, TEXT("%s: Matching surface actors: %i"), *this->GetActorLabel(),
           SurfaceActors.Num());

    const TArray<FTransform>& Transforms =
            AmbitWorldHelpers::GenerateRandomLocationsFromActors(SurfaceActors, RandomSeed,
                                                                 DensityMin, DensityMax, RotationMin, RotationMax);

    if (Transforms.Num() > 0)
    {
        SpawnActorsAtTransforms(Transforms, SpawnedObjects);
    }

    return SpawnedObjects;
}
