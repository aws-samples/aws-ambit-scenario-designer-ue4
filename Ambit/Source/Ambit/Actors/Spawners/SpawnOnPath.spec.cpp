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

#include "SpawnOnPath.h"

#include "Components/SplineComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/AutomationTest.h"

#include "Ambit/Actors/SpawnerConfigs/SpawnOnPathConfig.h"

BEGIN_DEFINE_SPEC(SpawnOnPathConfig, "Ambit.Unit.SpawnOnPathConfig",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    ASpawnOnPath* Spawner;
    TSharedPtr<FSpawnOnPathConfig> Config;

END_DEFINE_SPEC(SpawnOnPathConfig)

void SpawnOnPathConfig::Define()
{
    BeforeEach([this]()
    {
        // Create a fresh SpawnOnPath instance.
        UWorld* World = GEngine->GetWorldContexts()[0].World();
        const FActorSpawnParameters SpawnParams;
        Spawner = World->SpawnActor<ASpawnOnPath>(SpawnParams);
    });

    Describe("GetConfiguration()", [this]()
    {
        Describe("the returned configuration object", [this]()
        {
            It("has correct bSnapToSurfaceBelow", [this]()
            {
                Spawner->bSnapToSurfaceBelow = true;
                Config = Spawner->GetConfiguration();

                TestTrue("bSnapToSurfaceBelow", Config->bSnapToSurfaceBelow);
            });

            It("has correct spawner location and rotation", [this]()
            {
                const FVector Location(10, 10, 0);
                const FRotator Rotation(0, 90, 0);
                Spawner->SetActorLocationAndRotation(Location, Rotation);
                Config = Spawner->GetConfiguration();

                TestEqual("Location", Config->SpawnerLocation, Location);
                TestEqual("Rotation", Config->SpawnerRotation, Rotation);
            });

            It("when SpawnOnPathConfigConfiguration.MatchBy is 'NameOrTags', has correct value", [this]()
            {
                Spawner->MatchBy = NameOrTags;
                Config = Spawner->GetConfiguration();

                TestEqual("MatchBy", Config->MatchBy, EMatchBy::NameOrTags);
            });

            It("when SpawnOnPathConfigConfiguration.MatchBy is 'NameAndTags', has correct value", [this]()
            {
                Spawner->MatchBy = NameAndTags;
                Config = Spawner->GetConfiguration();

                TestEqual("MatchBy", Config->MatchBy, EMatchBy::NameAndTags);
            });

            It("has correct SurfaceNamePattern", [this]()
            {
                Spawner->SurfaceNamePattern = "SampleName";
                Config = Spawner->GetConfiguration();

                TestEqual("SurfaceName", Config->SurfaceNamePattern, "SampleName");
            });

            It("has correct SurfaceTags", [this]()
            {
                Spawner->SurfaceTags.Add(FName("TagA"));
                Spawner->SurfaceTags.Add(FName("TagB"));
                Config = Spawner->GetConfiguration();

                TestTrue("contains TagA", Config->SurfaceTags.Contains(FName("TagA")));
                TestTrue("contains TagB", Config->SurfaceTags.Contains(FName("TagB")));
            });

            It("has correct DensityMin", [this]()
            {
                Spawner->DensityMin = 0.1;
                Config = Spawner->GetConfiguration();

                TestEqual("DensityMin", Config->DensityMin, 0.1f);
            });

            It("has correct DensityMax", [this]()
            {
                Spawner->DensityMax = 0.9;
                Config = Spawner->GetConfiguration();

                TestEqual("DensityMax", Config->DensityMax, 0.9f);
            });

            It("has correct RotationMin", [this]()
            {
                Spawner->RotationMin = 90.0;
                Config = Spawner->GetConfiguration();

                TestEqual("RotationMin", Config->RotationMin, 90.0f);
            });

            It("has correct RotationMax", [this]()
            {
                Spawner->RotationMax = 270.0;
                Config = Spawner->GetConfiguration();

                TestEqual("RotationMax", Config->RotationMax, 270.0f);
            });

            It("has correct AddPhysics", [this]()
            {
                Spawner->bAddPhysics = true;
                Config = Spawner->GetConfiguration();

                TestTrue("AddPhysics", Config->bAddPhysics);
            });

            It("has correct ActorsToSpawn when it has only one value", [this]()
            {
                // Set a known AActor subclass as the ActorsToSpawn value.
                const FString ExpectedValue = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
                const FSoftClassPath ClassPath(ExpectedValue);

                TArray<TSubclassOf<AActor>> ExpectedActors;
                const TSubclassOf<AActor> ExpectedActor = ClassPath.TryLoadClass<UObject>();
                ExpectedActors.Add(ExpectedActor);
                Spawner->ActorsToSpawn = ExpectedActors;
                Config = Spawner->GetConfiguration();

                TestEqual("ActorsToSpawn", Config->ActorsToSpawn.Num(), ExpectedActors.Num());
                for (int32 i = 0; i < ExpectedActors.Num(); i++)
                {
                    TestEqual("ActorsToSpawn", Config->ActorsToSpawn[i], ExpectedActors[i]);
                }
            });

            It("has correct ActorsToSpawn when it has multiple values", [this]()
            {
                // Set known AActor subclasses as the ActorsToSpawn value.
                const FString ExpectedValueOne = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
                const FSoftClassPath ClassPathOne(ExpectedValueOne);

                TArray<TSubclassOf<AActor>> ExpectedActors;
                const TSubclassOf<AActor> ExpectedActorOne = ClassPathOne.TryLoadClass<UObject>();
                ExpectedActors.Add(ExpectedActorOne);
                const FString ExpectedValueTwo = "/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere_C";
                const FSoftClassPath ClassPathTwo(ExpectedValueTwo);
                const TSubclassOf<AActor> ExpectedActorTwo = ClassPathTwo.TryLoadClass<UObject>();
                ExpectedActors.Add(ExpectedActorTwo);
                Spawner->ActorsToSpawn = ExpectedActors;
                Config = Spawner->GetConfiguration();

                TestEqual("ActorsToSpawn", Config->ActorsToSpawn.Num(), ExpectedActors.Num());
                for (int32 i = 0; i < ExpectedActors.Num(); i++)
                {
                    TestEqual("ActorsToSpawn", Config->ActorsToSpawn[i], ExpectedActors[i]);
                }
            });

            It("has correct RemoveOverlaps", [this]()
            {
                Spawner->bRemoveOverlaps = false;
                Config = Spawner->GetConfiguration();

                TestFalse("RemoveOverlaps", Config->bRemoveOverlaps);
            });

            It("has correct RandomSeed", [this]()
            {
                Spawner->RandomSeed = 12;
                Config = Spawner->GetConfiguration();

                TestEqual("RandomSeed", Config->RandomSeed, 12);
            });

            It("has correct Spline Points", [this]()
            {
                TArray<FVector> Points;

                const FVector LocationOne(0, 0, 0);
                const FVector LocationTwo(500, 45, 0);

                Points.Add(LocationOne);
                Points.Add(LocationTwo);

                Spawner->Spline->ClearSplinePoints();
                Spawner->Spline->SetSplinePoints(Points, ESplineCoordinateSpace::Local);
                Config = Spawner->GetConfiguration();

                int32 i = 0;
                for (const FSplinePoint& Point : Config->SplinePoints)
                {
                    TestTrue("SplinePoint", Point.Position.Equals(Points[i]));
                    i++;
                }
            });
        });
    });

    Describe("Configure()", [this]()
    {
        BeforeEach([this]()
        {
            Config = MakeShareable(new FSpawnOnPathConfig);
        });

        It("sets bSnapToSurfaceBelow", [this]()
        {
            Config->bSnapToSurfaceBelow = true;
            Spawner->Configure(Config);
            TestTrue("bSnapToSurfaceBelow", Spawner->bSnapToSurfaceBelow);
        });

        It("sets MatchBy", [this]()
        {
            Config->MatchBy = EMatchBy::NameOrTags;

            Spawner->Configure(Config);

            TestEqual("MatchBy", Spawner->MatchBy, EMatchBy::NameOrTags);
        });

        It("sets SurfaceNamePattern", [this]()
        {
            Config->SurfaceNamePattern = "SampleName";

            Spawner->Configure(Config);

            TestEqual("SurfaceNamePattern", Spawner->SurfaceNamePattern, "SampleName");
        });

        It("sets SurfaceTags", [this]()
        {
            Config->SurfaceTags.Add("TagA");
            Config->SurfaceTags.Add("TagB");

            Spawner->Configure(Config);

            const int32 TagCount = Spawner->SurfaceTags.Num();
            if (TagCount != 2)
            {
                const FString Message = "Expected tag count of 2 but was " + FString::FromInt(TagCount);
                AddError(Message);
            }
            else
            {
                TestEqual("tag 0", Spawner->SurfaceTags[0], FName("TagA"));
                TestEqual("tag 1", Spawner->SurfaceTags[1], FName("TagB"));
            }
        });

        It("sets DensityMin", [this]()
        {
            Config->DensityMin = 0.1;

            Spawner->Configure(Config);

            TestEqual("DensityMin", Spawner->DensityMin, 0.1f);
        });

        It("sets DensityMax", [this]()
        {
            Config->DensityMax = 0.9;

            Spawner->Configure(Config);

            TestEqual("DensityMax", Spawner->DensityMax, 0.9f);
        });

        It("sets RotationMin", [this]()
        {
            Config->RotationMin = 90.0;

            Spawner->Configure(Config);

            TestEqual("RotationMin", Spawner->RotationMin, 90.0f);
        });

        It("sets RotationMax", [this]()
        {
            Config->RotationMax = 270.0;

            Spawner->Configure(Config);

            TestEqual("RotationMax", Spawner->RotationMax, 270.0f);
        });

        It("sets AddPhysics", [this]()
        {
            Config->bAddPhysics = true;
            Spawner->Configure(Config);

            TestTrue("AddPhysics", Spawner->bAddPhysics);
        });

        It("sets ActorsToSpawn when it has one value", [this]()
        {
            // Set a known AActor subclass as the ActorsToSpawn value.
            const FString ExpectedValue = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
            const FSoftClassPath ClassPath(ExpectedValue);
            TArray<TSubclassOf<AActor>> ExpectedActors;
            const TSubclassOf<AActor>& ExpectedActor = ClassPath.TryLoadClass<UObject>();
            ExpectedActors.Add(ExpectedActor);
            Config->ActorsToSpawn = ExpectedActors;

            Spawner->Configure(Config);
            TestEqual("ActorsToSpawn", Spawner->ActorsToSpawn.Num(), ExpectedActors.Num());
            for (int32 i = 0; i < ExpectedActors.Num(); i++)
            {
                TestEqual("ActorsToSpawn", Spawner->ActorsToSpawn[i], ExpectedActors[i]);
            }
        });

        It("sets correct ActorsToSpawn when it has multiple values", [this]()
        {
            // Set known AActor subclasses as the ActorsToSpawn value.
            const FString ExpectedValueOne = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
            const FSoftClassPath ClassPathOne(ExpectedValueOne);

            TArray<TSubclassOf<AActor>> ExpectedActors;
            const TSubclassOf<AActor> ExpectedActorOne = ClassPathOne.TryLoadClass<UObject>();
            ExpectedActors.Add(ExpectedActorOne);
            const FString ExpectedValueTwo = "/Engine/EngineSky/BP_Sky_Sphere.BP_Sky_Sphere_C";
            const FSoftClassPath ClassPathTwo(ExpectedValueTwo);
            const TSubclassOf<AActor> ExpectedActorTwo = ClassPathTwo.TryLoadClass<UObject>();
            ExpectedActors.Add(ExpectedActorTwo);

            Config->ActorsToSpawn = ExpectedActors;
            Spawner->Configure(Config);
            TestEqual("ActorsToSpawn", Spawner->ActorsToSpawn.Num(), ExpectedActors.Num());
            for (int32 i = 0; i < ExpectedActors.Num(); i++)
            {
                TestEqual("ActorsToSpawn", Spawner->ActorsToSpawn[i], ExpectedActors[i]);
            }
        });

        It("sets RemoveOverlaps", [this]()
        {
            Config->bRemoveOverlaps = false;
            Spawner->Configure(Config);

            TestFalse("RemoveOverlaps", Spawner->bRemoveOverlaps);
        });

        It("sets RandomSeed", [this]()
        {
            Config->RandomSeed = 42;

            Spawner->Configure(Config);

            TestEqual("RandomSeed", Spawner->RandomSeed, 42);
        });

        It("sets spline points", [this]()
        {
            TArray<FSplinePoint> SplinePoints;
            const FVector Tangents(450, 0, 0);
            const FVector LocationOne(0);
            const FVector LocationTwo(200, 0, 0);
            const FRotator Rotation(0);
            const FVector Scale(1);

            SplinePoints.Add(FSplinePoint(0, LocationOne, Tangents, Tangents, Rotation, Scale,
                                          ESplinePointType::CurveCustomTangent));
            SplinePoints.Add(FSplinePoint(1, LocationTwo, Tangents, Tangents, Rotation, Scale,
                                          ESplinePointType::CurveCustomTangent));

            Config->SplinePoints = SplinePoints;

            Spawner->Configure(Config);

            int32 i = 0;
            const USplineComponent* Spline = Spawner->Spline;
            for (const FSplinePoint& Point : Config->SplinePoints)
            {
                TestEqual("InputKey", Spline->GetInputKeyAtDistanceAlongSpline(
                              Spline->GetDistanceAlongSplineAtSplinePoint(i)), Point.InputKey);
                TestEqual("Location", Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local),
                          Point.Position);
                TestEqual("ArriveTangent", Spline->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),
                          Point.ArriveTangent);
                TestEqual("LeaveTangent", Spline->GetLeaveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local),
                          Point.LeaveTangent);
                TestEqual("Rotation", Spline->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::Local),
                          Point.Rotation);
                TestEqual("Scale", Spline->GetScaleAtSplinePoint(i), Point.Scale);
                TestEqual("Type", Spline->GetSplinePointType(i), Point.Type);
                i++;
            }
        });
    });

    AfterEach([this]()
    {
        Spawner->Destroy();
        Spawner = nullptr;
    });
}
