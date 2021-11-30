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

#include <AmbitUtils/ConfigJsonSerializer.h>

#include "Ambit/Utils/MatchBy.h"

class FJsonObject;

/**
 * This struct describes the base level configuration of an object that inherits from AmbitSpawnerBase
 * and is meant to be further inherited for classes that have more than these.
 */
struct AMBIT_API FSpawnerBaseConfig : FConfigJsonSerializer
{
    FVector SpawnerLocation;
    FRotator SpawnerRotation;

    TEnumAsByte<EMatchBy> MatchBy = EMatchBy::NameAndTags;

    // The search string used to find actors representing surfaces to spawn onto.
    FString SurfaceNamePattern = "";

    // The tag name used to find actors representing surfaces to spawn onto.
    TArray<FName> SurfaceTags;

    // The minimum number of items to spawn per square meter on average.
    float DensityMin = 0.05f;

    // The maximum number of items to spawn per square meter on average.
    float DensityMax = 0.2f;

    // The minimum degree of rotation.
    float RotationMin = 0.f;

    // The maximum degree of rotation.
    float RotationMax = 360.f;

    // Whether to add physics to spawned obstacles
    bool bAddPhysics = false;

    // The actors that will be spawned randomly
    TArray<TSubclassOf<class AActor>> ActorsToSpawn;

    // Whether the spawner should remove overlapping obstacles.
    bool bRemoveOverlaps = true;

    // Change this value to generate different random arrangements.
    int32 RandomSeed = 0;

    ~FSpawnerBaseConfig()
    {
    }

    virtual TSharedPtr<FJsonObject> SerializeToJson() const override;
    virtual void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) override;
};
