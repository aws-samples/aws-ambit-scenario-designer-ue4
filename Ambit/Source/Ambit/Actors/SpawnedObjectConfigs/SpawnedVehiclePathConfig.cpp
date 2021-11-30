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

#include "SpawnedVehiclePathConfig.h"

#include "Dom/JsonObject.h"

#include <AmbitUtils/JsonHelpers.h>

#include "Ambit/Mode/Constant.h"

TSharedPtr<FJsonObject>
USpawnedVehiclePathConfig::SerializeToJson() const
{
    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);

    if (Waypoints.Num() > 0 && !SpawnedVehicle.IsEmpty())
    {
        Json->SetStringField(
            JsonConstants::AmbitPathGenerator::KVehicleToSpawnKey, SpawnedVehicle);
        //unit is cm/s
        Json->SetNumberField(
            JsonConstants::AmbitPathGenerator::KSpeedLimit, SpeedLimit);

        TArray<TSharedPtr<FJsonValue>> WaypointsJson;
        for (const FVector& Waypoint : Waypoints)
        {
            WaypointsJson.Add(
                MakeShareable(new FJsonValueArray(FJsonHelpers::SerializeVector3(Waypoint))));
        }

        Json->SetArrayField(JsonConstants::AmbitPathGenerator::KWaypoints,
                            WaypointsJson);
    }

    return Json;
}

void USpawnedVehiclePathConfig::DeserializeFromJson(
    TSharedPtr<FJsonObject> JsonObject)
{
    // Does nothing since this class is not supposed to be deserialized
}
