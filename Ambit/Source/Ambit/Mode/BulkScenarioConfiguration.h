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
#include "IScenarioParameter.h"
#include "PedestrianTraffic.h"
#include "TimeOfDayTypes.h"
#include "VehicleTraffic.h"
#include "WeatherTypes.h"

#include <AmbitUtils/ConfigJsonSerializer.h>

struct FScenarioDefinition;
class FJsonObject;

/**
 * This struct is used as an intermediate between the Ambit UI and Serialization
 */
struct FBulkScenarioConfiguration : public FConfigJsonSerializer
{
    virtual ~FBulkScenarioConfiguration() override = default;

    const static FString KCurrentVersion;

    FString ConfigurationName;
    FString BatchName;
    FTimeOfDayTypes TimeOfDayTypes;
    FWeatherTypes WeatherTypes;
    FPedestrianTraffic PedestrianDensity;
    FVehicleTraffic VehicleDensity;
    int32 NumberOfPermutations;
    TSharedPtr<FJsonObject> AllSpawnersConfigs;

    TSharedPtr<FJsonObject> SerializeToJson() const override;

    void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) override;

    /**
     * Generates scenarios that represent all possible permutations given the configuration
     * parameters (like PedestrianTraffic, VehicleTraffic, Weather)
     */
    TArray<FScenarioDefinition> GenerateScenarios();

private:
    /**
     * Uses Depth First Search to get all scenario definition from an array IScenarioParameter objects
     * and saves in the TArray<FScenarioDefinition>& Scenarios.
     */
    void GetAllPermutationScenarios(TArray<IScenarioParameter*> VariantParameters,
                                    TArray<FScenarioDefinition>& Scenarios, int Depth,
                                    FScenarioDefinition& ScenarioDefinition);
};
