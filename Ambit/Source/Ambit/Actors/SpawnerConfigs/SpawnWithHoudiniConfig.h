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

#include "Ambit/Utils/MatchBy.h"

#include <AmbitUtils/ConfigJsonSerializer.h>

class FJsonObject;

/**
 * This struct describes the configuration of an SpawnWithHoudini instance.
 * Houdini doesn't expose the class that contains the location of the HDA that is being spawned,
 * so it can't be serialized.
 */
struct AMBIT_API FSpawnWithHoudiniConfig : FConfigJsonSerializer
{
    /**
     * Spawner's location in the world.
     */
    FVector SpawnerLocation;
    /**
     * Spawner's rotation.
     */
    FRotator SpawnerRotation;

    /**
     * The Match By Tag of the Spawner.
     */
    TEnumAsByte<EMatchBy> MatchBy = EMatchBy::NameAndTags;

    /**
     * The search string used to find actors representing surfaces to spawn onto.
     */
    FString SurfaceNamePattern = "";

    /**
     * The tag name used to find actors representing surfaces to spawn onto.
     */
    TArray<FName> SurfaceTags;

    /**
     * The minimum number of items to spawn per square meter on average.
     */
    float DensityMin = 0.05f;

    /**
     * The maximum number of items to spawn per square meter on average.
     */
    float DensityMax = 0.2f;

    /**
     * Change this value to generate different random arrangements.
     */
    int32 RandomSeed = 0;

    /**
     * Destructor for FSpawnWithHoudiniConfig.
     */
    ~FSpawnWithHoudiniConfig() = default;

    /**
     * @inheritDoc
     */
    TSharedPtr<FJsonObject> SerializeToJson() const override;

    /**
     * @inheritDoc
     */
    void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) override;
};
