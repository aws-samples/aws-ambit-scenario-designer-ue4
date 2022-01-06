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

#include "SpawnWithHoudini.h"

#include "HoudiniAsset.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"

#include "Ambit/Utils/AmbitWorldHelpers.h"


BEGIN_DEFINE_SPEC(SpawnWithHoudiniSpec, "Ambit.Integration.SpawnWithHoudini",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    UWorld* World;
    AStaticMeshActor* SurfaceActor;
    FString SurfaceActorName;
    FString Tag;
    ASpawnWithHoudini* Spawner;

END_DEFINE_SPEC(SpawnWithHoudiniSpec)

void SpawnWithHoudiniSpec::Define()
{
    BeforeEach([this]()
    {
        World = FAutomationEditorCommonUtils::CreateNewMap();
        Spawner = World->SpawnActor<ASpawnWithHoudini>();

        const FString Path = "StaticMesh'/Engine/BasicShapes/Plane.Plane'";
        FActorSpawnParameters Params;
        SurfaceActorName = "Plane";
        Params.Name = FName(*SurfaceActorName);

        UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *Path, nullptr, LOAD_None, nullptr);

        const FVector Location(0, 0, 0);
        const FRotator Rotation(0, 0, 0);
        const FTransform Transform(Rotation, Location);

        SurfaceActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Params);

        SurfaceActor->SetActorLabel(SurfaceActorName);
        SurfaceActor->SetMobility(EComponentMobility::Movable);
        SurfaceActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
        Tag = "SurfaceTag";
        SurfaceActor->Tags.Add(FName(Tag));
        const FVector Scale3D(5, 5, 5);
        SurfaceActor->SetActorScale3D(Scale3D);
    });

    Describe("GenerateObstacles() and PopulateParameters()", [this]()
    {
        Describe("When no HDA Selected", [this]()
        {
            It("Should return no spawned actors with no configuration to get SpawnedActor location", [this]()
            {
                Spawner->GenerateObstacles();
                TestEqual("The array of UHoudiniPublicAPIAssetWrapper is empty since there are no Actor spawned.",
                          Spawner->GetSpawnedActors().Num(), 0);
            });

            It(
                "Should return no spawned actors with configuration that has surface actor name when MatchBy is NameOrTags",
                [this]()
                {
                    Spawner->MatchBy = NameOrTags;
                    Spawner->SurfaceNamePattern = SurfaceActorName;
                    Spawner->GenerateObstacles();

                    TestEqual("The array of UHoudiniPublicAPIAssetWrapper is empty since there are no Actor spawned.",
                              Spawner->GetSpawnedActors().Num(), 0);
                });

            It(
                "Should return no spawned actors with configuration that has surface actor name and tag when MatchBy is NameAndTags",
                [this]()
                {
                    Spawner->MatchBy = NameAndTags;
                    Spawner->SurfaceNamePattern = SurfaceActorName;
                    Spawner->SurfaceTags.Add(FName(Tag));
                    Spawner->GenerateObstacles();

                    TestEqual("The array of UHoudiniPublicAPIAssetWrapper is empty since there are no Actor spawned.",
                              Spawner->GetSpawnedActors().Num(), 0);
                });

            AfterEach([this]()
            {
                Spawner->ResetObstacleSpawner();
            });
        });

        Describe("When one HDA is selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
            });

            It("Should return SpawnedActors when configuration has surface actor name when MatchBy is NameOrTags",
               [this]()
               {
                   Spawner->GenerateObstacles();

                   TestEqual("The array of UHoudiniPublicAPIAssetWrapper is not empty.",
                             Spawner->HoudiniAssetDetails[0].SpawnedActors.Num(), 2);
               });

            It(
                "Should not return SpawnedActors when no configuration doesn't have surface actor name when MatchBy is NameOrTags",
                [this]()
                {
                    Spawner->SurfaceNamePattern = "";
                    Spawner->SurfaceTags.Empty();
                    Spawner->GenerateObstacles();

                    TestEqual("The array of UHoudiniPublicAPIAssetWrapper is empty.",
                              Spawner->HoudiniAssetDetails[0].SpawnedActors.Num(), 0);
                });

            Describe("PopulateParameters() (When loading with Houdini)", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Spawner->PreInstantiationDone = Done;
                    Spawner->GenerateObstacles();
                });

                It("Should generate ParamsToRandomProperly for valid HoudiniAssetDetails", [this]()
                {
                    TestEqual("HoudiniAssetDetails ParamsToRandom contains only one item",
                              Spawner->HoudiniAssetDetails[0].ParamsToRandom.Num(), 1);
                    TestTrue("HoudiniAssetDetails ParamsToRandom's item is 'random_seed'",
                             Spawner->HoudiniAssetDetails[0].ParamsToRandom.Contains("random_seed"));
                });
            });
        });

        Describe("When multiple HDA are selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                Spawner->RandomSeed = 3; // 3 is chosen as the first seed that spawns both HDA

                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
                FHoudiniLoadableAsset NewAsset;
                NewAsset.HDAToLoad = SampleCubeHDA;
                Spawner->HoudiniAssetDetails.Add(NewAsset);
            });

            It("Will have the correct output for item's SpawnedActors when multiple HoudiniAssetDetails are valid",
               [this]()
               {
                   Spawner->GenerateObstacles();

                   TestTrue("First HoudiniAssetDetails contains items.",
                            Spawner->HoudiniAssetDetails[0].SpawnedActors.Num() > 0);
                   TestTrue("First HoudiniAssetDetails contains items.",
                            Spawner->HoudiniAssetDetails[1].SpawnedActors.Num() > 0);
               });

            Describe("PopulateParameters() (When loading with Houdini)", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Spawner->PreInstantiationDone = Done;

                    Spawner->GenerateObstacles();
                });

                It("Will generate ParamsToRandomProperly for valid HoudiniAssetDetails", [this]()
                {
                    TestEqual("HoudiniAssetDetails ParamsToRandom contains only one item",
                              Spawner->HoudiniAssetDetails[1].ParamsToRandom.Num(), 1);
                    TestTrue("HoudiniAssetDetails ParamsToRandom's item is 'random_seed'",
                             Spawner->HoudiniAssetDetails[1].ParamsToRandom.Contains("random_seed"));
                });
            });
        });

        AfterEach([this]()
        {
            Spawner->ResetObstacleSpawner();
        });
    });

    Describe("GetSpawnedActors()", [this]()
    {
        Describe("When one HDA is selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
            });
            It("Will have correct number of actors in list", [this]()
            {
                Spawner->GenerateObstacles();

                const int32 InnerSpawnedActorCount = Spawner->HoudiniAssetDetails[0].SpawnedActors.Num();

                TestEqual("GetSpawnedActor reflects AssetDetails SpawnedActors", Spawner->GetSpawnedActors().Num(),
                          InnerSpawnedActorCount);
                TestEqual("GetSpawnedActor has the correct number of actors", Spawner->GetSpawnedActors().Num(), 2);
            });
        });

        Describe("When multiple HDA are selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                Spawner->RandomSeed = 3; // 3 is chosen as the first seed that spawns both HDA

                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
                FHoudiniLoadableAsset NewAsset;
                NewAsset.HDAToLoad = SampleCubeHDA;
                Spawner->HoudiniAssetDetails.Add(NewAsset);
            });

            It("Will have the correct output for item's SpawnedActors when multiple HoudiniAssetDetails are valid",
               [this]()
               {
                   Spawner->GenerateObstacles();

                   const int32 InnerSpawnedActorCount = Spawner->HoudiniAssetDetails[0].SpawnedActors.Num() + Spawner->
                           HoudiniAssetDetails[1].SpawnedActors.Num();

                   TestEqual("GetSpawnedActor reflects AssetDetails SpawnedActors", Spawner->GetSpawnedActors().Num(),
                             InnerSpawnedActorCount);
                   TestEqual("GetSpawnedActor has the correct number of actors", Spawner->GetSpawnedActors().Num(), 2);
               });
        });

        AfterEach([this]()
        {
            Spawner->ResetObstacleSpawner();
        });
    });

    Describe("RandomizeActor()", [this]()
    {
        Describe("When a single HDA is selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                Spawner->RandomSeed = 1;

                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
            });

            Describe("When randomizing without modifying parameters", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Spawner->PostProcessingDone = Done;

                    Spawner->GenerateObstacles();
                });

                It("Will generate the same parameter every time", [this]()
                {
                    const float ExpectedNumber = 78.2120209f;
                    float OutSeedNumber;
                    const TWeakObjectPtr<UHoudiniPublicAPIAssetWrapper> FirstAsset = Spawner->HoudiniAssetDetails[0].
                            SpawnedActors[0];
                    const bool bLoaded = FirstAsset->GetFloatParameterValue("random_seed", OutSeedNumber);

                    TestTrue("Asset parameter was loaded properly", bLoaded);
                    TestEqual("Seed value is expected number", OutSeedNumber, ExpectedNumber);
                });
            });

            Describe("When randomizing with removing parameters", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Spawner->PostProcessingDone = Done;

                    // Set to fake, as to not allow params to re-generate.
                    Spawner->HoudiniAssetDetails[0].ParamsToRandom.Add("fake");
                    Spawner->GenerateObstacles();
                });

                It("Will generate the base parameter regardless", [this]()
                {
                    const float ExpectedNumber = 0.550000012;
                    float OutSeedNumber;
                    const TWeakObjectPtr<UHoudiniPublicAPIAssetWrapper> FirstAsset = Spawner->HoudiniAssetDetails[0].
                            SpawnedActors[0];
                    const bool bLoaded = FirstAsset->GetFloatParameterValue("random_seed", OutSeedNumber);

                    TestTrue("Asset parameter was loaded properly", bLoaded);
                    TestEqual("Seed value is expected number", OutSeedNumber, ExpectedNumber);
                });
            });

            AfterEach([this]()
            {
                Spawner->ResetObstacleSpawner();
            });
        });

        Describe("When multiple HDA are selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                Spawner->RandomSeed = 3; // 3 is chosen as the first seed that spawns both HDA

                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
                FHoudiniLoadableAsset NewAsset;
                NewAsset.HDAToLoad = SampleCubeHDA;
                Spawner->HoudiniAssetDetails.Add(NewAsset);
            });
            Describe("When randomizing without modifying parameters", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Spawner->PostProcessingDone = Done;

                    Spawner->GenerateObstacles();
                });

                It("Will generate the same parameter every time", [this]()
                {
                    const float ExpectedNumber = 38.0790367f;
                    float OutSeedNumber;
                    const TWeakObjectPtr<UHoudiniPublicAPIAssetWrapper> FirstAsset = Spawner->HoudiniAssetDetails[1].
                            SpawnedActors[0];
                    const bool bLoaded = FirstAsset->GetFloatParameterValue("random_seed", OutSeedNumber);

                    TestTrue("Asset parameter was loaded properly", bLoaded);
                    TestEqual("Seed value is expected number", OutSeedNumber, ExpectedNumber);
                });
            });

            Describe("When randomizing with removing parameters", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Spawner->PostProcessingDone = Done;

                    // Set to fake, as to not allow params to re-generate.
                    Spawner->HoudiniAssetDetails[1].ParamsToRandom.Add("fake");
                    Spawner->GenerateObstacles();
                });

                It("Will generate the base parameter when modified", [this]()
                {
                    const float ExpectedNumber = 0.550000012f;

                    // We do not modify the first set of objects, so their numbers should not be the ExpectedNumber.
                    const bool AnyExpected = Spawner->HoudiniAssetDetails[0].SpawnedActors.ContainsByPredicate(
                        [ExpectedNumber](TWeakObjectPtr<UHoudiniPublicAPIAssetWrapper> Asset)
                        {
                            float OutSeedNumber;
                            Asset->GetFloatParameterValue("random_seed", OutSeedNumber);

                            return OutSeedNumber == ExpectedNumber;
                        });

                    // We remove the randomization from this set, so all of their numbers should be the default value.
                    const bool AnyNotExpected = Spawner->HoudiniAssetDetails[1].SpawnedActors.ContainsByPredicate(
                        [ExpectedNumber](TWeakObjectPtr<UHoudiniPublicAPIAssetWrapper> Asset)
                        {
                            float OutSeedNumber;
                            Asset->GetFloatParameterValue("random_seed", OutSeedNumber);

                            return OutSeedNumber != ExpectedNumber;
                        });

                    TestFalse("Unmodified list should have randomized parameters", AnyExpected);
                    TestFalse("Modified list should not have randomized parameters", AnyNotExpected);
                });
            });
        });

        AfterEach([this]()
        {
            Spawner->ResetObstacleSpawner();
        });
    });

    Describe("ClearObstacles()", [this]()
    {
        Describe("When a single HDA is selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                Spawner->RandomSeed = 1;

                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
                Spawner->GenerateObstacles();
            });

            It("Should reset when all actors were available", [this]()
            {
                Spawner->ClearObstacles();

                TestEqual("Spawned Actors should reset to 0", Spawner->HoudiniAssetDetails[0].SpawnedActors.Num(), 0);
            });

            It("Should reset when all actors even when not available (Destroyed by Houdini Deletion)", [this]()
            {
                Spawner->HoudiniAssetDetails[0].SpawnedActors[1]->DeleteInstantiatedAsset();
                Spawner->HoudiniAssetDetails[0].SpawnedActors[0]->DeleteInstantiatedAsset();
                Spawner->ClearObstacles();

                TestEqual("Spawned Actors should reset to 0", Spawner->HoudiniAssetDetails[0].SpawnedActors.Num(), 0);
            });

            It("Should reset when all actors even when not available (Destroyed by UE Deletion)", [this]()
            {
                const EMatchBy MatchBy = EMatchBy::NameOrTags;
                const FString SurfaceNamePattern = "sample_cube";
                const TArray<FName> SurfaceTags;

                const TArray<AActor*> SurfaceActors = AmbitWorldHelpers::GetActorsByMatchBy(
                    MatchBy, SurfaceNamePattern, SurfaceTags);
                for (auto* Actor : SurfaceActors)
                {
                    Actor->Destroy();
                }

                Spawner->ClearObstacles();

                TestEqual("Spawned Actors should reset to 0", Spawner->HoudiniAssetDetails[0].SpawnedActors.Num(), 0);
            });
        });

        Describe("When multiple HDA are selected", [this]()
        {
            BeforeEach([this]()
            {
                Spawner->MatchBy = NameOrTags;
                Spawner->SurfaceNamePattern = SurfaceActorName;
                Spawner->SurfaceTags.Add(FName(Tag));
                Spawner->RandomSeed = 3; // 3 is chosen as the first seed that spawns both HDA

                UHoudiniAsset* SampleCubeHDA = Cast<UHoudiniAsset>(StaticLoadObject(
                    UHoudiniAsset::StaticClass(), nullptr,
                    TEXT("HoudiniAsset'/Ambit/Test/Props/sample_cube.sample_cube'")));

                Spawner->HoudiniAssetDetails[0].HDAToLoad = SampleCubeHDA;
                FHoudiniLoadableAsset NewAsset;
                NewAsset.HDAToLoad = SampleCubeHDA;
                Spawner->HoudiniAssetDetails.Add(NewAsset);
            });
        });

        AfterEach([this]()
        {
            Spawner->ResetObstacleSpawner();
        });
    });

    AfterEach([this]()
    {
        SurfaceActor->Destroy();
        SurfaceActor = nullptr;
    });
}
