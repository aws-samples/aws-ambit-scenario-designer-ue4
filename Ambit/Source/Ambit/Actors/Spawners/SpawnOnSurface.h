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

#include "Ambit/Actors/SpawnerConfigs/SpawnerBaseConfig.h"

#include "SpawnOnSurface.generated.h"

/**
 * This class provides random generation of obstacles on specified surfaces
 * matched by name/tags.
 */
UCLASS()
class AMBIT_API ASpawnOnSurface : public ASpawnerBase
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASpawnOnSurface();

    /**
     * See https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/PostEditChangeProperty/
     */
    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * @inheritDoc
     */
    TSharedPtr<FSpawnerBaseConfig> GetConfiguration() const;

    /**
     * @inheritDoc
     */
    void Configure(const TSharedPtr<FSpawnerBaseConfig>& Config);


private:
    // Spawns actors as specified by this object's parameters.
    TMap<FString, TArray<FTransform>> GenerateActors() override;
};
