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

#include <AmbitUtils/ConfigJsonSerializer.h>

#include "SpawnedVehiclePathConfig.generated.h"

class FJsonObject;

/**
 * This class describes the configuration of the spawned vehicle and path
 */
UCLASS()
class AMBIT_API USpawnedVehiclePathConfig : public UObject, public IConfigJsonSerializer
{
    GENERATED_BODY()
public:
    /**
     * List of waypoints representing the vehicle running path
     */
    TArray<FVector> Waypoints;

    /*
     * Asset path to the spawned vehicle
     */
    FString SpawnedVehicle;

    /**
     * Speed limit of the vehicle in km/h
     */
    float SpeedLimit;

    ~USpawnedVehiclePathConfig() = default;

    /**
     * @inheritDoc
     */
    TSharedPtr<FJsonObject> SerializeToJson() const override;

    /**
     * @inheritDoc
     */
    void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) override;

    /**
     * @inheritDoc
     */
    FString GetOutputConfigurationName() const override
    {
        return JsonConstants::AmbitPathGenerator::KAmbitVehiclePathGenerator;
    }
};
