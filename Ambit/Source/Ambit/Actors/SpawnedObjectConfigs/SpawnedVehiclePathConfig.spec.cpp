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

#include "Json.h"
#include "Misc/AutomationTest.h"

#include <AmbitUtils/JsonHelpers.h>

BEGIN_DEFINE_SPEC(SpawnedVehiclePathConfigSpec, "Ambit.Unit.SpawnedVehiclePathConfig",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    USpawnedVehiclePathConfig* Config;

END_DEFINE_SPEC(SpawnedVehiclePathConfigSpec)

void SpawnedVehiclePathConfigSpec::Define()
{
    Describe("SerializeToJson()", [this]()
    {
        BeforeEach([this]()
        {
            Config = NewObject<USpawnedVehiclePathConfig>();
        });

        Describe("the returned JsonObject", [this]()
        {
            It("sets spawned vehicle value correctly", [this]()
            {
                const FString& Path = "TestPath";
                TArray<FVector> Locations;
                Locations.Add(FVector());

                Config->SpawnedVehicle = Path;
                Config->Waypoints = Locations;
                Config->SpeedLimit = 800;
                const FString& ResultPath = Config->SerializeToJson()->GetStringField("VehicleToSpawn");
                TestEqual("Vehicle Asset Path", ResultPath, Path);
            });

            It("sets speed limit value correctly", [this]()
            {
                const FString& Path = "TestPath";
                TArray<FVector> Locations;
                Locations.Add(FVector());

                Config->SpawnedVehicle = Path;
                Config->Waypoints = Locations;
                Config->SpeedLimit = 800;
                const float Result = Config->SerializeToJson()->GetNumberField("SpeedLimit");
                TestEqual("Speed Limit", Result, 800.f);
            });

            It("sets waypoints values correctly", [this]()
            {
                const FString& Path = "TestPath";
                TArray<FVector> Locations;
                const FVector Vector2(100, 100, 0);
                Locations.Add(FVector());
                Locations.Add(Vector2);

                Config->SpawnedVehicle = Path;
                Config->Waypoints = Locations;
                Config->SpeedLimit = 800;
                const TArray<TSharedPtr<FJsonValue>> ResultsArray = Config->SerializeToJson()->GetArrayField(
                    "Waypoints");
                for (int32 i = 0; i < ResultsArray.Num(); i++)
                {
                    const TArray<TSharedPtr<FJsonValue>>& Location = ResultsArray[i]->AsArray();
                    TestTrue("Waypoint", Locations[i].Equals(FJsonHelpers::DeserializeToVector3(Location)));
                }
            });
        });
    });
}
