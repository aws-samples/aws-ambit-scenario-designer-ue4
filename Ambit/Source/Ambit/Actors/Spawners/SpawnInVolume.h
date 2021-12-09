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

#pragma once

#include "CoreMinimal.h"
#include "SpawnerBase.h"

#include "Ambit/Actors/SpawnerConfigs/SpawnInVolumeConfig.h"

#include "SpawnInVolume.generated.h"

struct FSpawnInVolumeConfig;
/**
 * This class provides random generation of obstacles within a bounding plane.
 */
UCLASS()
class AMBIT_API ASpawnInVolume : public ASpawnerBase
{
    GENERATED_BODY()
public:
    // Sets default values for this actor's properties
    ASpawnInVolume();

    UPROPERTY(VisibleAnywhere, Category = "Ambit Spawner")
    class UBoxComponent* Box;

    /**
     * If no surfaces are specified in Match By, when true, 
     * Ambit Spawner will snap obstacles to surfaces below.
     */
    UPROPERTY(EditAnywhere, Category = "Ambit Spawner")
    bool bSnapToSurfaceBelow = false;

    /**
     * See https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/PostEditChangeProperty/
     * Additionally, checks if bounding box is not flat.
     */
    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * @inheritDoc
     */
    TSharedPtr<FSpawnInVolumeConfig> GetConfiguration() const;

    /**
     * @inheritDoc
     */
    void Configure(const TSharedPtr<FSpawnInVolumeConfig>& Config);

private:
    // Spawns actors as specified by this object's parameters.
    TMap<FString, TArray<FTransform>> GenerateActors() override;
};
