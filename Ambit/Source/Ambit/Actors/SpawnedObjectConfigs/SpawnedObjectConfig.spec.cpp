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

#include "SpawnedObjectConfig.h"

#include "Json.h"
#include "Misc/AutomationTest.h"

#include <AmbitUtils/JsonHelpers.h>


BEGIN_DEFINE_SPEC(SpawnedObjectConfigSpec, "Ambit.Unit.SpawnedObjectConfig",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    USpawnedObjectConfig* Config;
    TSharedPtr<FJsonObject> Json;

END_DEFINE_SPEC(SpawnedObjectConfigSpec)

void SpawnedObjectConfigSpec::Define()
{
    Describe("SerializeToJson()", [this]()
    {
        BeforeEach([this]()
        {
            // Create a fresh AmbitSpawnedObjectConfiguration instance.
            Config = NewObject<USpawnedObjectConfig>();
        });

        Describe("the returned JsonObject", [this]()
        {
            It("sets values correctly when there is one ActorToSpawn pathname and one transform", [this]()
            {
                TMap<FString, TArray<FTransform>> Expected;
                const FString& Path = "TestPath";
                TArray<FTransform> Transforms;
                Transforms.Add(FTransform());
                Expected.Add(Path, Transforms);

                Config->SpawnedObjects = Expected;
                const TArray<TSharedPtr<FJsonValue>> ResultsArray = Config->SerializeToJson()->GetArrayField(
                    "SpawnedObjects");
                const auto& Object = ResultsArray[0]->AsObject();
                TestEqual("Path Name", Object->GetStringField("ActorToSpawn"), Path);
                TestTrue("Location", Transforms[0].GetLocation().Equals(
                             FJsonHelpers::DeserializeToVector3(Object->GetArrayField("Location"))));
                TestTrue("Rotation", Transforms[0].Rotator().Equals(
                             FJsonHelpers::DeserializeToRotation(Object->GetArrayField("Rotation"))));
            });

            It("sets values correctly when there is one ActorToSpawn pathname and multiple transforms", [this]()
            {
                TMap<FString, TArray<FTransform>> Expected;
                const FString& Path = "TestPath";
                TArray<FTransform> Transforms;
                const FTransform TransformTwo(FRotator(), FVector(100, 100, 0));
                Transforms.Add(FTransform());
                Transforms.Add(TransformTwo);
                Expected.Add(Path, Transforms);

                Config->SpawnedObjects = Expected;
                const TArray<TSharedPtr<FJsonValue>> ResultsArray = Config->SerializeToJson()->GetArrayField(
                    "SpawnedObjects");
                for (int32 i = 0; i < ResultsArray.Num(); i++)
                {
                    const TSharedPtr<FJsonObject>& Object = ResultsArray[i]->AsObject();
                    TestEqual("Path Name", Object->GetStringField("ActorToSpawn"), Path);
                    TestTrue("Location", Transforms[i].GetLocation().Equals(
                                 FJsonHelpers::DeserializeToVector3(Object->GetArrayField("Location"))));
                    TestTrue("Rotation", Transforms[i].Rotator().Equals(
                                 FJsonHelpers::DeserializeToRotation(Object->GetArrayField("Rotation"))));
                }
            });

            It("sets values correctly when there are multiple ActorToSpawn pathnames and multiple transforms", [this]()
            {
                TMap<FString, TArray<FTransform>> Expected;
                const FString& Path = "TestPath";
                const FString& OtherPath = "AnotherTestPath";
                TArray<FTransform> TransformsOne;
                const FTransform TransformTwo(FRotator(), FVector(100, 100, 0));
                TransformsOne.Add(FTransform());
                TArray<FTransform> TransformsTwo;
                TransformsTwo.Add(TransformTwo);

                Expected.Add(Path, TransformsOne);
                Expected.Add(OtherPath, TransformsTwo);

                Config->SpawnedObjects = Expected;

                const TArray<TSharedPtr<FJsonValue>> ResultsArray = Config->SerializeToJson()->GetArrayField(
                    "SpawnedObjects");
                for (const TSharedPtr<FJsonValue>& Result : ResultsArray)
                {
                    const TSharedPtr<FJsonObject>& Object = Result->AsObject();
                    TestTrue("Path Name", Expected.Contains(Object->GetStringField("ActorToSpawn")));
                    const TArray<FTransform>& ExpectedTransforms = *Expected.Find(
                        Object->GetStringField("ActorToSpawn"));
                    TestTrue("Location", ExpectedTransforms[0].GetLocation().Equals(
                                 FJsonHelpers::DeserializeToVector3(Object->GetArrayField("Location"))));
                    TestTrue("Rotation", ExpectedTransforms[0].Rotator().Equals(
                                 FJsonHelpers::DeserializeToRotation(Object->GetArrayField("Rotation"))));
                }
            });
        });
    });
}
