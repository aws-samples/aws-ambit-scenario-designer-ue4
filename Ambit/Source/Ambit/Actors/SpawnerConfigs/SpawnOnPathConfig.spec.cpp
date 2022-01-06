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

#include "SpawnOnPathConfig.h"

#include "Json.h"
#include "Misc/AutomationTest.h"

#include <AmbitUtils/JsonHelpers.h>

BEGIN_DEFINE_SPEC(SpawnOnPathSpec, "Ambit.Unit.SpawnOnPath",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    FSpawnOnPathConfig Config;
    TSharedPtr<FJsonObject> Json;

END_DEFINE_SPEC(SpawnOnPathSpec)


void SpawnOnPathSpec::Define()
{
    Describe("SerializeToJson()", [this]()
    {
        BeforeEach([this]()
        {
            // Create a fresh SpawnOnPathConfigConfiguration instance.
            Config = FSpawnOnPathConfig{};
        });

        Describe("the returned JsonObject", [this]()
        {
            It("JSON has correct field for bSnapToSurfaceBelow", [this]()
            {
                AddExpectedError("No spline point data to serialize.", EAutomationExpectedErrorFlags::Exact, 1);
                Config.bSnapToSurfaceBelow = true;
                TestTrue("Correct SnapToSurfaceBelow", Config.SerializeToJson()->GetBoolField("SnapToSurfaceBelow"));
            });

            It("JSON has correct fields for Spline Transforms", [this]()
            {
                TArray<FSplinePoint> SplinePoints;
                const FVector Tangents(450, 0, 0);
                const FVector LocationOne(0);
                const FRotator Rotation(0);
                const FVector Scale(1);

                SplinePoints.Add(FSplinePoint(0, LocationOne, Tangents, Tangents, Rotation, Scale,
                                              ESplinePointType::Curve));
                SplinePoints.Add(
                    FSplinePoint(1, Tangents, Tangents, Tangents, Rotation, Scale, ESplinePointType::Curve));

                Config.SplinePoints = SplinePoints;

                const TSharedPtr<FJsonObject> ConfigJson = Config.SerializeToJson();
                TestTrue("Has SplinePoints field", ConfigJson->HasField("SplinePoints"));

                FSpawnOnPathConfig NewConfig = FSpawnOnPathConfig{};
                NewConfig.DeserializeFromJson(ConfigJson);

                for (int32 i = 0; i < Config.SplinePoints.Num(); i++)
                {
                    const FSplinePoint& Actual = NewConfig.SplinePoints[i];
                    const FSplinePoint& Expected = Config.SplinePoints[i];
                    TestEqual("InputKey", Actual.InputKey, Expected.InputKey);
                    TestEqual("Location", Actual.Position, Expected.Position);
                    TestEqual("ArriveTangent", Actual.ArriveTangent, Expected.ArriveTangent);
                    TestEqual("LeaveTangent", Actual.LeaveTangent, Expected.LeaveTangent);
                    TestEqual("Rotation", Actual.Rotation, Expected.Rotation);
                    TestEqual("Scale", Actual.Scale, Expected.Scale);
                    TestEqual("Type", Actual.Type, Expected.Type);
                }
            });
        });
    });

    Describe("DeserializeFromJson()", [this]()
    {
        BeforeEach([this]()
        {
            // Create fresh config value.
            Config = FSpawnOnPathConfig{};

            // Create a sample SpawnOnPathConfiguration JSON object.
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
                "    'SplinePoints': ["
                "        {"
                "            'SplinePointInputKey': 0,"
                "            'SplinePointRelativeLocation': [0, 0, 0],"
                "            'SplinePointArriveTangent': [100, 0, 0],"
                "            'SplinePointLeaveTangent': [100, 0, 0],"
                "            'SplinePointRelativeRotation': [0, 0, 0],"
                "            'SplinePointRelativeScale': [1, 1, 1],"
                "            'SplinePointType': 'Curve'"
                "        },"
                "        {"
                "            'SplinePointInputKey': 1,"
                "            'SplinePointRelativeLocation': [100, 0, 0],"
                "            'SplinePointArriveTangent': [100, 0, 0],"
                "            'SplinePointLeaveTangent': [100, 0, 0],"
                "            'SplinePointRelativeRotation': [0, 0, 0],"
                "            'SplinePointRelativeScale': [1, 1, 1],"
                "            'SplinePointType': 'Curve'"
                "        }"
                "    ]"
                "}")
                .Replace(TEXT("'"), TEXT("\""));
            Json = FJsonHelpers::DeserializeJson(JsonString);
        });

        It("when JSON has SnapToSurfaceBelow, sets bSnapToSurfaceBelow correctly", [this]()
        {
            Json->SetBoolField("SnapToSurfaceBelow", true);
            Config.DeserializeFromJson(Json);
            TestTrue("SnapToSurfaceBelow", Config.bSnapToSurfaceBelow);
        });

        It("sets SplinePoints correctly", [this]()
        {
            const FVector ExpectedTangents(100, 0, 0);
            const FVector ExpectedScale(1);
            const FRotator ExpectedRotation(0);
            const TEnumAsByte<ESplinePointType::Type>& ExpectedType = ESplinePointType::Curve;
            Config.DeserializeFromJson(Json);
            for (const FSplinePoint& Point : Config.SplinePoints)
            {
                if (Point.InputKey == 0)
                {
                    TestEqual("Location", Point.Position, FVector(0));
                }
                if (Point.InputKey == 1)
                {
                    TestEqual("Location", Point.Position, ExpectedTangents);
                }
                TestEqual("ArriveTangent", Point.ArriveTangent, ExpectedTangents);
                TestEqual("LeaveTangent", Point.LeaveTangent, ExpectedTangents);
                TestEqual("Rotation", Point.Rotation, ExpectedRotation);
                TestEqual("Scale", Point.Scale, ExpectedScale);
                TestEqual("Type", Point.Type, ExpectedType);
            }
        });

        It("when called multiple times, SplinePoints should not accumulate points", [this]()
        {
            Config.DeserializeFromJson(Json);
            // Intentional second call.
            Config.DeserializeFromJson(Json);

            TestEqual("spline point count", Config.SplinePoints.Num(), 2);
        });
    });
}
