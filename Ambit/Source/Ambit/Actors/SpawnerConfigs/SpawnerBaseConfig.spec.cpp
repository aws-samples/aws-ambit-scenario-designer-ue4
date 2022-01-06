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

#include "SpawnerBaseConfig.h"

#include "Json.h"
#include "Misc/AutomationTest.h"
#include "UObject/SoftObjectPath.h"

#include "Ambit/Utils/MatchBy.h"

#include <AmbitUtils/JsonHelpers.h>

BEGIN_DEFINE_SPEC(SpawnerBaseConfigSpec, "Ambit.Unit.AmbitSpawnerBaseConfig",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    FSpawnerBaseConfig Config;
    TSharedPtr<FJsonObject> Json;

END_DEFINE_SPEC(SpawnerBaseConfigSpec)

void SpawnerBaseConfigSpec::Define()
{
    Describe("SerializeToJson()", [this]()
    {
        BeforeEach([this]()
        {
            // Create a fresh SpawnerBaseConfiguration instance.
            Config = FSpawnerBaseConfig{};
        });

        Describe("the returned JsonObject", [this]()
        {
            It("when MatchBy value is EMatchBy::NameOrTags, " "JSON has a MatchBy value of 'NameOrTags'", [this]()
            {
                Config.MatchBy = NameOrTags;

                const FString Value = Config.SerializeToJson()->GetStringField("MatchBy");

                TestEqual("MatchBy", Value, "NameOrTags");
            });

            It("when MatchBy value is EMatchBy::NameAndTags, " "JSON has a MatchBy value of 'NameAndTags'", [this]()
            {
                Config.MatchBy = NameAndTags;

                const FString Value = Config.SerializeToJson()->GetStringField("MatchBy");

                TestEqual("MatchBy", Value, "NameAndTags");
            });

            It("has SurfaceNamePattern value", [this]()
            {
                const FString ExpectedValue = "Sample name pattern";
                Config.SurfaceNamePattern = ExpectedValue;

                const FString Value = Config.SerializeToJson()->GetStringField("SurfaceNamePattern");

                TestEqual("SurfaceNamePattern", Value, ExpectedValue);
            });

            It("given one SurfaceTags item, JSON SurfaceTags contains that tag", [this]()
            {
                Config.SurfaceTags.Add(FName("TagA"));

                TArray<TSharedPtr<FJsonValue>> TagsAsJsonValues = Config.SerializeToJson()->
                                                                         GetArrayField("SurfaceTags");

                TestEqual("tag", TagsAsJsonValues[0]->AsString(), "TagA");
            });

            It("given multiple SurfaceTags items, JSON SurfaceTags contains all tags", [this]()
            {
                Config.SurfaceTags.Add(FName("TagA"));
                Config.SurfaceTags.Add(FName("TagB"));

                TArray<TSharedPtr<FJsonValue>> TagsAsJsonValues = Config.SerializeToJson()->
                                                                         GetArrayField("SurfaceTags");

                const int TagCount = TagsAsJsonValues.Num();
                if (TagCount != 2)
                {
                    const FString Message = FString::Printf(TEXT("incorrect array length %d"), TagCount);
                    AddError(Message);
                }
                else
                {
                    TestEqual("tag 0", TagsAsJsonValues[0]->AsString(), "TagA");
                    TestEqual("tag 1", TagsAsJsonValues[1]->AsString(), "TagB");
                }
            });

            It("has SpawnerLocation value", [this]()
            {
                const FVector ExpectedLocation(0, 0, 40.0);
                Config.SpawnerLocation = ExpectedLocation;

                const FVector Result = FJsonHelpers::DeserializeToVector3(
                    Config.SerializeToJson()->GetArrayField("SpawnerLocation"));

                TestEqual("SpawnerLocation", Result, ExpectedLocation);
            });

            It("has SpawnerRotation value", [this]()
            {
                const FRotator ExpectedRotation(0, 30, 0);
                Config.SpawnerRotation = ExpectedRotation;

                const FRotator Result = FJsonHelpers::DeserializeToRotation(
                    Config.SerializeToJson()->GetArrayField("SpawnerRotation"));

                TestEqual("SpawnerRotation", Result, ExpectedRotation);
            });

            It("has DensityMin value", [this]()
            {
                const double ExpectedValue = 0.345;
                Config.DensityMin = ExpectedValue;

                const double Result = Config.SerializeToJson()->GetNumberField("DensityMin");

                TestEqual("density", Result, ExpectedValue);
            });

            It("has DensityMax value", [this]()
            {
                const double ExpectedValue = 0.123;
                Config.DensityMax = ExpectedValue;

                const double Result = Config.SerializeToJson()->GetNumberField("DensityMax");

                TestEqual("density", Result, ExpectedValue);
            });

            It("has RotationMin value", [this]()
            {
                const double ExpectedValue = 0.345;
                Config.RotationMin = ExpectedValue;

                const double Result = Config.SerializeToJson()->GetNumberField("RotationMin");

                TestEqual("rotation", Result, ExpectedValue);
            });

            It("has RotationMax value", [this]()
            {
                const double ExpectedValue = 0.123;
                Config.RotationMax = ExpectedValue;

                const double Result = Config.SerializeToJson()->GetNumberField("RotationMax");

                TestEqual("rotation", Result, ExpectedValue);
            });

            It("has AddPhysics value", [this]()
            {
                Config.bAddPhysics = true;

                const bool Result = Config.SerializeToJson()->GetBoolField("AddPhysics");

                TestTrue("add physics", Result);
            });

            It("has ActorsToSpawn value when it has one valid element", [this]()
            {
                // Set a known AActor subclass as the ActorsToSpawn value.
                const FString ExpectedValue = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
                const FSoftClassPath ClassPath(ExpectedValue);
                TArray<TSubclassOf<AActor>> ExpectedActors;
                const TSubclassOf<AActor>& ExpectedActor = ClassPath.TryLoadClass<UObject>();
                ExpectedActors.Add(ExpectedActor);
                Config.ActorsToSpawn = ExpectedActors;

                const TArray<TSharedPtr<FJsonValue>> Result = Config.SerializeToJson()->GetArrayField("ActorsToSpawn");

                TestEqual("class name", Result[0]->AsString(), ExpectedValue);
            });

            It("sets ActorsToSpawn json to null if ActorsToSpawn has one nullptr element", [this]()
            {
                AddExpectedError("An element of the ActorsToSpawn set is not specified.",
                                 EAutomationExpectedErrorFlags::Exact, 1);
                TArray<TSubclassOf<AActor>> ExpectedActors;
                ExpectedActors.Add(nullptr);
                Config.ActorsToSpawn = ExpectedActors;

                TestTrue("class name", Config.SerializeToJson()->HasTypedField<EJson::Null>("ActorsToSpawn"));
            });

            It("does not add array field for ActorsToSpawn in Json if said element is nullptr", [this]()
            {
                AddExpectedError("An element of the ActorsToSpawn set is not specified.",
                                 EAutomationExpectedErrorFlags::Exact, 1);

                const FString ExpectedValue = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
                const FSoftClassPath ClassPath(ExpectedValue);
                TArray<TSubclassOf<AActor>> ExpectedActors;
                const TSubclassOf<AActor>& ExpectedActor = ClassPath.TryLoadClass<UObject>();
                ExpectedActors.Add(ExpectedActor);
                ExpectedActors.Add(nullptr);
                Config.ActorsToSpawn = ExpectedActors;

                TestTrue("class name", Config.SerializeToJson()->HasTypedField<EJson::Null>("ActorsToSpawn"));
            });

            It("has ActorsToSpawn value when it has multiple valid elements", [this]()
            {
                // Set known AActor subclasses as the ActorsToSpawn value.

                TArray<TSubclassOf<AActor>> ExpectedActors;
                TSet<FString> Expected;
                const FString ExpectedValueOne = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
                const FSoftClassPath ClassPathOne(ExpectedValueOne);
                const TSubclassOf<AActor> ExpectedActorOne = ClassPathOne.TryLoadClass<UObject>();
                const FString ExpectedValueTwo = "/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere_C";
                const FSoftClassPath ClassPathTwo(ExpectedValueTwo);
                const TSubclassOf<AActor> ExpectedActorTwo = ClassPathTwo.TryLoadClass<UObject>();

                ExpectedActors.Add(ExpectedActorOne);
                ExpectedActors.Add(ExpectedActorTwo);
                Expected.Add(ExpectedValueOne);
                Expected.Add(ExpectedValueTwo);

                Config.ActorsToSpawn = ExpectedActors;

                const TArray<TSharedPtr<FJsonValue>> ResultJson = Config.SerializeToJson()->GetArrayField(
                    "ActorsToSpawn");

                TSet<FString> Result;
                for (const auto& Json : ResultJson)
                {
                    Result.Add(Json->AsString());
                }

                TestTrue("class name", Result.Includes(Expected) && Expected.Includes(Result));
            });

            It("when ActorsToSpawn is null, JSON ActorsToSpawn is null", [this]()
            {
                const TSharedPtr<FJsonValue> Result = Config.SerializeToJson()->GetField<EJson::None>("ActorsToSpawn");

                TestTrue("is null", Result->Type == EJson::Null);
            });

            It("has RemoveOverlaps", [this]()
            {
                Config.bRemoveOverlaps = false;
                const bool Result = Config.SerializeToJson()->GetBoolField("RemoveOverlaps");

                TestFalse("remove overlaps", Result);
            });

            It("has RandomSeed value", [this]()
            {
                const int ExpectedValue = 5;
                Config.RandomSeed = ExpectedValue;

                const int Result = Config.SerializeToJson()->GetNumberField("RandomSeed");

                TestEqual("random seed", Result, ExpectedValue);
            });
        });
    });

    Describe("DeserializeFromJson()", [this]()
    {
        BeforeEach([this]()
        {
            // Create fresh config value.
            Config = FSpawnerBaseConfig{};

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
                "    'RandomSeed': 5"
                "}")
                .Replace(TEXT("'"), TEXT("\""));
            Json = FJsonHelpers::DeserializeJson(JsonString);
        });

        It("when JSON MatchBy is 'NameOrTags', sets MatchBy correctly", [this]()
        {
            Json->SetStringField("MatchBy", "NameOrTags");

            Config.DeserializeFromJson(Json);

            TestEqual("MatchBy", Config.MatchBy, EMatchBy::NameOrTags);
        });

        It("when JSON MatchBy is 'NameAndTags', sets MatchBy correctly", [this]()
        {
            Json->SetStringField("MatchBy", "NameAndTags");

            Config.DeserializeFromJson(Json);

            TestEqual("MatchBy", Config.MatchBy, EMatchBy::NameAndTags);
        });

        It("sets SurfaceNamePattern", [this]()
        {
            const FString ExpectedValue = "SampleName";
            Json->SetStringField("SurfaceNamePattern", ExpectedValue);

            Config.DeserializeFromJson(Json);

            TestEqual("SurfaceNamePattern", Config.SurfaceNamePattern, ExpectedValue);
        });

        It("given one surface tag, sets SurfaceTags correctly", [this]()
        {
            TArray<TSharedPtr<FJsonValue>> TagsJson;
            TagsJson.Add(MakeShareable(new FJsonValueString("TagA")));
            Json->SetArrayField("SurfaceTags", TagsJson);

            Config.DeserializeFromJson(Json);

            const int32 TagCount = Config.SurfaceTags.Num();
            if (TagCount != 1)
            {
                FString Message = "Expected SurfaceTags count of 1 but was ";
                Message.AppendInt(TagCount);
                AddError(Message);
            }
            else
            {
                TestEqual("first tag", Config.SurfaceTags[0], FName("TagA"));
            }
        });

        It("given multiple surface tags, sets SurfaceTags correctly", [this]()
        {
            TArray<TSharedPtr<FJsonValue>> TagsJson;
            TagsJson.Add(MakeShareable(new FJsonValueString("TagA")));
            TagsJson.Add(MakeShareable(new FJsonValueString("TagB")));
            Json->SetArrayField("SurfaceTags", TagsJson);

            Config.DeserializeFromJson(Json);

            const int32 TagCount = Config.SurfaceTags.Num();
            if (TagCount != 2)
            {
                FString Message = "Expected SurfaceTags count of 2 but was ";
                Message.AppendInt(TagCount);
                AddError(Message);
            }
            else
            {
                TestEqual("first tag", Config.SurfaceTags[0], FName("TagA"));
                TestEqual("second tag", Config.SurfaceTags[1], FName("TagB"));
            }
        });

        It("when called multiple times, SurfaceTags should not accumulate tags", [this]()
        {
            TArray<TSharedPtr<FJsonValue>> TagsJson;
            TagsJson.Add(MakeShareable(new FJsonValueString("TagA")));
            Json->SetArrayField("SurfaceTags", TagsJson);

            Config.DeserializeFromJson(Json);
            // Intentional second call.
            Config.DeserializeFromJson(Json);

            TestEqual("tag count", Config.SurfaceTags.Num(), 1);
        });

        It("sets SpawnerLocation value", [this]()
        {
            const FVector ExpectedLocation(100, 100, 0);
            Json->SetArrayField("SpawnerLocation", FJsonHelpers::SerializeVector3(ExpectedLocation));
            Config.DeserializeFromJson(Json);
            TestEqual("SpawnerLocation", Config.SpawnerLocation, ExpectedLocation);
        });

        It("sets SpawnerRotation value", [this]()
        {
            const FRotator ExpectedRotation(0, 30, 0);
            Json->SetArrayField("SpawnerRotation", FJsonHelpers::SerializeRotation(ExpectedRotation));
            Config.DeserializeFromJson(Json);
            TestEqual("SpawnerRotation", Config.SpawnerRotation, ExpectedRotation);
        });

        It("sets DensityMin", [this]()
        {
            Json->SetNumberField("DensityMin", 0.1);

            Config.DeserializeFromJson(Json);

            TestEqual("DensityMin", Config.DensityMin, 0.1f);
        });

        It("sets DensityMax", [this]()
        {
            Json->SetNumberField("DensityMax", 0.9);

            Config.DeserializeFromJson(Json);

            TestEqual("DensityMax", Config.DensityMax, 0.9f);
        });

        It("sets RotationMin", [this]()
        {
            Json->SetNumberField("RotationMin", 45.0);

            Config.DeserializeFromJson(Json);

            TestEqual("RotationMin", Config.RotationMin, 45.0f);
        });

        It("sets RotationMax", [this]()
        {
            Json->SetNumberField("RotationMax", 90.0);

            Config.DeserializeFromJson(Json);

            TestEqual("RotationMax", Config.RotationMax, 90.0f);
        });

        It("sets AddPhysics", [this]()
        {
            Json->SetBoolField("AddPhysics", true);
            Config.DeserializeFromJson(Json);
            TestTrue("AddPhysics", Config.bAddPhysics);
        });

        It("when JSON ActorsToSpawn is valid array with one element," " sets ActorsToSpawn correctly", [this]()
        {
            // Get a reference to a sample AActor subclass.
            const FString ActorPath = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
            const FSoftClassPath ClassPath(ActorPath);
            const TSubclassOf<AActor> ExpectedValue = ClassPath.TryLoadClass<UObject>();
            TArray<TSubclassOf<AActor>> ExpectedActors;
            TArray<TSharedPtr<FJsonValue>> ExpectedJson;
            ExpectedJson.Add(MakeShareable(new FJsonValueString(ActorPath)));
            ExpectedActors.Add(ExpectedValue);
            Json->SetArrayField("ActorsToSpawn", ExpectedJson);

            Config.DeserializeFromJson(Json);

            TestEqual("ActorsToSpawn", Config.ActorsToSpawn.Num(), ExpectedActors.Num());
            for (int32 i = 0; i < ExpectedActors.Num(); i++)
            {
                TestEqual("ActorsToSpawn", Config.ActorsToSpawn[i], ExpectedActors[i]);
            }
        });

        It("when JSON ActorsToSpawn is valid array with multiple elements," " sets ActorsToSpawn correctly", [this]()
        {
            // Get a reference to a sample AActor subclass.
            const FString ActorPathOne = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
            const FSoftClassPath ClassPathOne(ActorPathOne);
            const TSubclassOf<AActor> ExpectedValueOne = ClassPathOne.TryLoadClass<UObject>();
            const FString ActorPathTwo = "/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere_C";
            const FSoftClassPath ClassPathTwo(ActorPathTwo);
            const TSubclassOf<AActor> ExpectedValueTwo = ClassPathTwo.TryLoadClass<UObject>();
            TArray<TSubclassOf<AActor>> ExpectedActors;
            TArray<TSharedPtr<FJsonValue>> ExpectedJson;
            ExpectedJson.Add(MakeShareable(new FJsonValueString(ActorPathOne)));
            ExpectedActors.Add(ExpectedValueOne);
            ExpectedJson.Add(MakeShareable(new FJsonValueString(ActorPathTwo)));
            ExpectedActors.Add(ExpectedValueTwo);
            Json->SetArrayField("ActorsToSpawn", ExpectedJson);

            Config.DeserializeFromJson(Json);
            TestEqual("ActorsToSpawn", Config.ActorsToSpawn.Num(), ExpectedActors.Num());
            for (int32 i = 0; i < ExpectedActors.Num(); i++)
            {
                TestEqual("ActorsToSpawn", Config.ActorsToSpawn[i], ExpectedActors[i]);
            }
        });

        It("when called multiple times should not accumulate ActorsToSpawn", [this]()
        {
            // Get a reference to a sample AActor subclass.
            const FString ActorPath = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
            TArray<TSharedPtr<FJsonValue>> ExpectedJson;
            ExpectedJson.Add(MakeShareable(new FJsonValueString(ActorPath)));
            Json->SetArrayField("ActorsToSpawn", ExpectedJson);

            Config.DeserializeFromJson(Json);
            // Intentional second call
            Config.DeserializeFromJson(Json);

            TestEqual("ActorsToSpawn", Config.ActorsToSpawn.Num(), 1);
        });

        It("sets RemoveOverlaps", [this]()
        {
            Json->SetBoolField("RemoveOverlaps", false);
            Config.DeserializeFromJson(Json);

            TestFalse("RemoveOverlaps", Config.bRemoveOverlaps);
        });

        It("sets RandomSeed", [this]()
        {
            Json->SetNumberField("RandomSeed", 12);

            Config.DeserializeFromJson(Json);

            TestEqual("RandomSeed", Config.RandomSeed, 12);
        });
    });
}
