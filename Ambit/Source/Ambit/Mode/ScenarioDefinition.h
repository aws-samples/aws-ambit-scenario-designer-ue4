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

#include "AmbitWeatherParameters.h"

#include <AmbitUtils/ConfigJsonSerializer.h>

class FJsonObject;

/**
 * This struct is used as an intermediate between the Ambit UI and Serialization
 */
struct FScenarioDefinition : public FConfigJsonSerializer
{
    const static FString KCurrentVersion;

    FString ScenarioName;
    FString Location;
    float TimeOfDay;
    FAmbitWeatherParameters AmbitWeatherParameters;
    float PedestrianDensity;
    float VehicleDensity;
    int32 Seed = INDEX_NONE;
    TSharedPtr<FJsonObject> AllSpawnersConfigs;

    ~FScenarioDefinition() = default;

    TSharedPtr<FJsonObject> SerializeToJson() const override;

    void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) override;
};
