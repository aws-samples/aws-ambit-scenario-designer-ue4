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
#include "GameFramework/Actor.h"

#include "Ambit/Actors/SpawnerConfigs/SpawnerBaseConfig.h"
#include "Ambit/Actors/Spawners/AmbitSpawner.h"

#include "MockableSpawner.generated.h"

class USpawnedObjectConfig;

/**
 * A class meant for usage in testing only.
 */
UCLASS()
class AMBIT_API AMockableSpawner : public AActor, public IAmbitSpawner
{
    GENERATED_BODY()
public:
    TFunction<bool()> LambdaHasActorToSpawn;
    TFunction<TSharedPtr<FSpawnerBaseConfig>()> LambdaGetConfiguration;
    TFunction<void(TSharedPtr<FSpawnerBaseConfig> Config)> LambdaGetConfigure;
    // Should call EmitConfigCompleted
    TFunction<void()> LambdaGenerateSpawnedObjectConfiguration;
    // Should call EmitConfigCompleted
    TFunction<void(int32 Seed)> LambdaGenerateSpawnedObjectConfigurationSeeded;

    void EmitConfigCompleted(TScriptInterface<IConfigJsonSerializer>& Config, bool bIsSuccess) const
    {
        OnSpawnedObjectConfigCompleted.ExecuteIfBound(Config, bIsSuccess);
    };

    /**
     * @inheritDoc
     */
    bool HasActorsToSpawn() const override
    {
        return LambdaHasActorToSpawn();
    };

    /**
     * @inheritDoc
     */
    TSharedPtr<FSpawnerBaseConfig> GetConfiguration() const
    {
        return LambdaGetConfiguration();
    }

    /**
     * @inheritDoc
     */
    void Configure(const TSharedPtr<FSpawnerBaseConfig>& Config) const
    {
        LambdaGetConfigure(Config);
    }

    /**
     * @inheritDoc
     */
    void GenerateSpawnedObjectConfiguration() override
    {
        LambdaGenerateSpawnedObjectConfiguration();
    }

    /**
     * @inheritDoc
     */
    void GenerateSpawnedObjectConfiguration(int32 Seed) override
    {
        LambdaGenerateSpawnedObjectConfigurationSeeded(Seed);
    }

private:
};
