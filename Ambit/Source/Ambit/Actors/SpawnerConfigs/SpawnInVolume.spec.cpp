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

#include "Json.h"
#include "SpawnInVolumeConfig.h"
#include "Misc/AutomationTest.h"

#include <AmbitUtils/JsonHelpers.h>

BEGIN_DEFINE_SPEC(SpawnInVolumeConfigSpec, "Ambit.Unit.SpawnInVolumeConfig",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    FSpawnInVolumeConfig Config;
    TSharedPtr<FJsonObject> Json;

END_DEFINE_SPEC(SpawnInVolumeConfigSpec)

void SpawnInVolumeConfigSpec::Define()
{
    Describe("SerializeToJson()", [this]()
    {
        BeforeEach([this]()
        {
            // Create a fresh SpawnInVolumeConfiguration instance.
            Config = FSpawnInVolumeConfig{};
        });

        Describe("the returned JsonObject", [this]()
        {
            It("JSON has correct fields for Box Extent", [this]()
            {
                const FVector Expected(150.0, 150.0, 0);
                Config.BoxExtent = Expected;
                const TArray<TSharedPtr<FJsonValue>> Array = Config.SerializeToJson()->GetArrayField("BoxExtent");
                const FVector Actual = FJsonHelpers::DeserializeToVector3(Array);
                TestEqual("Correct BoxExtent", Actual, Expected);
            });

            It("JSON has correct field for bSnapToSurfaceBelow", [this]()
            {
                Config.bSnapToSurfaceBelow = true;
                TestTrue("Correct SnapToSurfaceBelow", Config.SerializeToJson()->GetBoolField("SnapToSurfaceBelow"));
            });
        });
    });

    Describe("DeserializeFromJson()", [this]()
    {
        BeforeEach([this]()
        {
            // Create fresh config value.
            Config = FSpawnInVolumeConfig{};

            // Create a sample SpawnerBaseConfiguration JSON object.
            // NOTE: We're wrapping JSON keys and strings in single quotes to improve
            // readability. Then we replace all single quotes with double quotes (per the
            // JSON standard) before parsing the string to a JSON object.
            const FString JsonString = FString(
                "{"
                "    'SpawnerLocation': [0, 0, 0],"
                "    'SpawnerRotation': [0, 0, 0],"
                "    'MatchBy': 'NameOrTags',"
                "    'SurfaceNamePattern': 'MyPattern',"
                "    'SurfaceTags': [],"
                "    'DensityMin': 0.1,"
                "    'DensityMax': 0.4,"
                "    'RotationMin': 45.0,"
                "    'RotationMax': 90.0,"
                "    'AddPhysics': false,"
                "    'ActorsToSpawn': null,"
                "    'RemoveOverlaps': true,"
                "    'RandomSeed': 5,"
                "    'SnapToSurfaceBelow': false,"
                "    'BoxExtent': [100.0, 100.0, 0]"
                "}")
                .Replace(TEXT("'"), TEXT("\""));
            Json = FJsonHelpers::DeserializeJson(JsonString);
        });

        It("when JSON has BoxExtent, sets BoxExtent correctly", [this]()
        {
            const FVector Expected(250.0, 250.0, 0);
            Json->SetArrayField("BoxExtent", FJsonHelpers::SerializeVector3(Expected));
            Config.DeserializeFromJson(Json);
            TestEqual("BoxExtent", Config.BoxExtent, Expected);
        });

        It("when JSON has SnapToSurfaceBelow, sets bSnapToSurfaceBelow correctly", [this]()
        {
            Json->SetBoolField("SnapToSurfaceBelow", true);
            Config.DeserializeFromJson(Json);
            TestTrue("SnapToSurfaceBelow", Config.bSnapToSurfaceBelow);
        });
    });
}
