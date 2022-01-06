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

#include "AmbitSpawnerCollisionHelpers.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/UObjectGlobals.h"

#include "AmbitWorldHelpers.h"

BEGIN_DEFINE_SPEC(AmbitSpawnerCollisionHelpersSpec, "Ambit.Unit.AmbitSpawnerCollisionHelpers",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    UWorld* World;
    AStaticMeshActor* TestSurfaceActor;
    AActor* TestSpawnedActor;
    FString ExpectedActorName;
END_DEFINE_SPEC(AmbitSpawnerCollisionHelpersSpec)

void AmbitSpawnerCollisionHelpersSpec::Define()
{
    BeforeEach([this]()
    {
        // Create an empty test map;
        World = FAutomationEditorCommonUtils::CreateNewMap();
        TestNotNull("Check if World is properly created", World);

        const FString Path = "StaticMesh'/Engine/BasicShapes/Cube.Cube'";
        FActorSpawnParameters Params;
        ExpectedActorName = "Cube";
        Params.Name = FName(*ExpectedActorName);

        UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *Path, nullptr, LOAD_None, nullptr);
        TestNotNull("Check if static mesh for surface actor is properly loaded", StaticMesh);

        FVector Location(0, 0, 0);
        FRotator Rotation(0, 0, 0);
        FTransform Transform(Rotation, Location);

        TestSurfaceActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Params);

        TestNotNull("Check if surface actor is properly created", TestSurfaceActor);

        TestSurfaceActor->SetActorLabel(ExpectedActorName);
        TestSurfaceActor->SetMobility(EComponentMobility::Movable);
        TestSurfaceActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);

        FActorSpawnParameters NewParams;
        FString NewParamName = "Obstacle";
        NewParams.Name = FName(*NewParamName);
        FVector NewLocation(0, 0, 100);
        FRotator NewRotation(0, 0, 0);
        const FString SpawnedActorPath = "/Ambit/Test/Props/BP_Box01.BP_Box01_C";
        const FSoftClassPath ClassPath(SpawnedActorPath);
        const TSubclassOf<AActor> ActorToSpawn = ClassPath.TryLoadClass<UObject>();
        TestSpawnedActor = World->SpawnActor(ActorToSpawn.Get(), &NewLocation, &NewRotation);
        TestNotNull("Check if spawned actor is properly created", TestSpawnedActor);
    });
    Describe("SetGenerateOverlapEventsForActor()", [this]()
    {
        It("does nothing if there is no static mesh component", [this]()
        {
            AActor* NoStaticMeshActor = World->SpawnActor(AActor::StaticClass());
            TArray<bool> Original;
            AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(NoStaticMeshActor, Original);
            TestEqual("nothing added to array", Original.Num(), 0);
            NoStaticMeshActor->Destroy();
        });

        It("produces an error message and returns early if the number of static mesh components"
           " does not equal the number of elements in the original array if bReset is true", [this]()
           {
               AddExpectedError("The amount of StaticMeshComponents found for Actor");
               UStaticMeshComponent* StaticMesh = TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>();
               StaticMesh->SetGenerateOverlapEvents(false);
               TArray<bool> Original;
               Original.Add(true);
               Original.Add(true);
               AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original, true);
               TestFalse("static mesh setting was not changed", StaticMesh->GetGenerateOverlapEvents());
           });

        It("sets generate overlap events to true if only one static mesh component exists", [this]()
        {
            UStaticMeshComponent* StaticMesh = TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>();
            StaticMesh->SetGenerateOverlapEvents(false);
            TArray<bool> Original;
            AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original);
            TestTrue("set to true",
                     TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>()->GetGenerateOverlapEvents());
        });

        It("does not accumulate stored original settings if called twice", [this]()
        {
            UStaticMeshComponent* StaticMesh = TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>();
            StaticMesh->SetGenerateOverlapEvents(false);
            TArray<bool> Original;
            AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original);
            // intentional second call
            AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original);
            TestEqual("size is not accumulated", Original.Num(), 1);
        });

        It("stores original setting if only one static mesh component exists", [this]()
        {
            UStaticMeshComponent* StaticMesh = TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>();
            StaticMesh->SetGenerateOverlapEvents(false);
            TArray<bool> Original;
            AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original);
            TestFalse("stored original setting", Original[0]);
        });

        It("resets generate overlap events if only one static mesh component exists", [this]()
        {
            UStaticMeshComponent* StaticMesh = TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>();
            StaticMesh->SetGenerateOverlapEvents(true);
            TArray<bool> Original;
            Original.Add(false); // "Default"
            AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original, true);

            TestFalse("reset to false",
                      TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>()->GetGenerateOverlapEvents());
        });

        Describe("given multiple static mesh components", [this]()
        {
            BeforeEach([this]()
            {
                const FString Path = "StaticMesh'/Engine/BasicShapes/Plane.Plane'";

                UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *Path, nullptr, LOAD_None, nullptr);
                TestNotNull("Check if static mesh is properly loaded", StaticMesh);
                UStaticMeshComponent* Mesh2 = NewObject<UStaticMeshComponent>(
                    TestSurfaceActor, UStaticMeshComponent::StaticClass());
                Mesh2->SetStaticMesh(StaticMesh);
            });

            It("sets generate overlap events to true if multiple static mesh components exist", [this]()
            {
                TArray<UStaticMeshComponent*> ArrayOfMeshes;
                TestSurfaceActor->GetComponents<UStaticMeshComponent>(ArrayOfMeshes);

                TestEqual("size of array of static meshes", ArrayOfMeshes.Num(), 2);

                ArrayOfMeshes[0]->SetGenerateOverlapEvents(false);
                ArrayOfMeshes[1]->SetGenerateOverlapEvents(false);
                TArray<bool> Original;
                AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original);
                TestTrue("set to true", ArrayOfMeshes[0]->GetGenerateOverlapEvents());
                TestTrue("set to true", ArrayOfMeshes[1]->GetGenerateOverlapEvents());
            });

            It("resets generate overlap events if multiple static mesh components exist", [this]()
            {
                TArray<UStaticMeshComponent*> ArrayOfMeshes;
                TestSurfaceActor->GetComponents<UStaticMeshComponent>(ArrayOfMeshes);

                TestEqual("size of array of static meshes", ArrayOfMeshes.Num(), 2);

                ArrayOfMeshes[0]->SetGenerateOverlapEvents(true);
                ArrayOfMeshes[1]->SetGenerateOverlapEvents(true);
                TArray<bool> Original;
                Original.Add(false);
                Original.Add(false);
                AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original, true);
                TestFalse("reset", ArrayOfMeshes[0]->GetGenerateOverlapEvents());
                TestFalse("reset", ArrayOfMeshes[1]->GetGenerateOverlapEvents());
            });

            It("stores original settings if multiple static mesh components exist", [this]()
            {
                TArray<UStaticMeshComponent*> ArrayOfMeshes;
                TestSurfaceActor->GetComponents<UStaticMeshComponent>(ArrayOfMeshes);

                TestEqual("size of array of static meshes", ArrayOfMeshes.Num(), 2);

                ArrayOfMeshes[0]->SetGenerateOverlapEvents(false);
                ArrayOfMeshes[1]->SetGenerateOverlapEvents(false);
                TArray<bool> Original;
                AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(TestSurfaceActor, Original);
                TestTrue("stored original settings", Original.Num() == 2);
                TestFalse("stored original settings", Original[0]);
                TestFalse("stored original settings", Original[1]);
            });
        });
    });

    Describe("FindDefaultStaticMeshComponents()", [this]()
    {
        It("will not add anything to the array if there are no static mesh components.", [this]()
        {
            TArray<UStaticMeshComponent*> Actual;
            AmbitSpawnerCollisionHelpers::FindDefaultStaticMeshComponents(AActor::StaticClass(), Actual);
            TestEqual("Static mesh component array is empty", Actual.Num(), 0);
        });

        It("will get the right StaticMeshComponent when there is one", [this]()
        {
            TArray<UStaticMeshComponent*> Actual;
            AmbitSpawnerCollisionHelpers::FindDefaultStaticMeshComponents(TestSurfaceActor->GetClass(), Actual);
            TestEqual("Static mesh component array has one element", Actual.Num(), 1);

            TestEqual("static mesh component array is correct", TestSurfaceActor->GetStaticMeshComponent()->GetClass(),
                      Actual[0]->GetClass());
        });
    });

    Describe("SetCollisionForAllStaticMeshComponents()", [this]()
    {
        It("sets component to object type for Ambit Spawner Obstacles", [this]()
        {
            UStaticMeshComponent* StaticMesh = TestSurfaceActor->GetStaticMeshComponent();

            TArray<UStaticMeshComponent*> Meshes;
            StaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
            Meshes.Add(StaticMesh);
            AmbitSpawnerCollisionHelpers::SetCollisionForAllStaticMeshComponents(Meshes);

            TestEqual("channel", StaticMesh->GetCollisionObjectType(), ECC_GameTraceChannel1);
        });

        It("sets channels correctly", [this]()
        {
            UStaticMeshComponent* StaticMesh = TestSurfaceActor->GetStaticMeshComponent();

            TArray<UStaticMeshComponent*> Meshes;
            Meshes.Add(StaticMesh);
            AmbitSpawnerCollisionHelpers::SetCollisionForAllStaticMeshComponents(Meshes);
            TestEqual("collision enabled", StaticMesh->GetCollisionEnabled(), ECollisionEnabled::QueryAndPhysics);

            const FCollisionResponseContainer& Container = StaticMesh->GetCollisionResponseToChannels();
            TestEqual("ambit spawned obstacles", Container.GameTraceChannel1, ECR_Block);
            TestEqual("camera", Container.Camera, ECR_Overlap);
            TestEqual("visibility", Container.Visibility, ECR_Overlap);
            TestEqual("world static", Container.WorldStatic, ECR_Overlap);
            TestEqual("world dynamic", Container.WorldDynamic, ECR_Overlap);
            TestEqual("physics body", Container.PhysicsBody, ECR_Overlap);
            TestEqual("destructible", Container.Destructible, ECR_Overlap);
            TestEqual("pawn", Container.Pawn, ECR_Overlap);
            TestEqual("vehicle", Container.Vehicle, ECR_Overlap);
        });
    });

    Describe("IsPenetratingOverlap()", [this]()
    {
        It("returns true if actor is a penetrating overlap", [this]()
        {
            TestSurfaceActor->SetActorLocation(FVector(0));
            TestSurfaceActor->SetActorScale3D(FVector(5));
            TestSpawnedActor->SetActorLocation(FVector(0));

            UStaticMeshComponent* Surface = TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>();

            TestTrue("is penetrating overlap",
                     AmbitSpawnerCollisionHelpers::IsPenetratingOverlap(Surface, TestSpawnedActor));
        });

        It("returns false if actor is on top of surface", [this]()
        {
            TestSpawnedActor->SetActorLocation(FVector(0));
            TestSurfaceActor->SetActorLocation(FVector(0, 0, 100));
            TestSurfaceActor->SetActorScale3D(FVector(2));
            const FHitResult& Hit = AmbitWorldHelpers::LineTraceBelowWorldPoint(FVector(0, 0, 1000));
            TestSpawnedActor->SetActorLocation(Hit.ImpactPoint);
            TestSurfaceActor->SetActorEnableCollision(true);
            TestSpawnedActor->SetActorEnableCollision(true);

            UStaticMeshComponent* Surface = TestSurfaceActor->FindComponentByClass<UStaticMeshComponent>();

            TestFalse("is not penetrating overlap",
                      AmbitSpawnerCollisionHelpers::IsPenetratingOverlap(Surface, TestSpawnedActor));
        });
    });

    Describe("StoreCollisionProfiles()", [this]()
    {
        It("does nothing when there are no components given", [this]()
        {
            const TArray<UStaticMeshComponent*> StaticMeshComponents;
            TMap<FString, TArray<FCollisionResponseTemplate>> OutMap;
            AmbitSpawnerCollisionHelpers::StoreCollisionProfiles("Empty", StaticMeshComponents, OutMap);

            TestEqual("empty key added to map", OutMap.Num(), 1);
            TestEqual("empty array associated with empty key", OutMap.FindChecked("Empty").Num(), 0);
        });

        It("correctly adds to the map when there is only one component", [this]()
        {
            UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>();
            Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
            TArray<UStaticMeshComponent*> StaticMeshComponents;
            StaticMeshComponents.Add(Mesh);
            TMap<FString, TArray<FCollisionResponseTemplate>> OutMap;

            AmbitSpawnerCollisionHelpers::StoreCollisionProfiles("One", StaticMeshComponents, OutMap);

            TestEqual("number of map elements", OutMap.Num(), 1);
            TestEqual("number of static meshes for element", OutMap.FindChecked("One").Num(), 1);
            TestTrue("correct collisions",
                     OutMap.FindChecked("One")[0].ResponseToChannels == Mesh->GetCollisionResponseToChannels());
        });

        It("correctly adds to the map when there are multiple components", [this]()
        {
            UStaticMeshComponent* MeshOne = NewObject<UStaticMeshComponent>();
            MeshOne->SetCollisionResponseToAllChannels(ECR_Ignore);

            UStaticMeshComponent* MeshTwo = NewObject<UStaticMeshComponent>();
            MeshTwo->SetCollisionResponseToAllChannels(ECR_Overlap);

            TArray<UStaticMeshComponent*> StaticMeshComponents;
            StaticMeshComponents.Add(MeshOne);
            StaticMeshComponents.Add(MeshTwo);

            TMap<FString, TArray<FCollisionResponseTemplate>> OutMap;
            AmbitSpawnerCollisionHelpers::StoreCollisionProfiles("One", StaticMeshComponents, OutMap);

            TestEqual("elements in map", OutMap.Num(), 1);
            TestEqual("number of static meshes for element", OutMap.FindChecked("One").Num(), 2);
            TestTrue("correct collisions",
                     OutMap.FindChecked("One")[0].ResponseToChannels == MeshOne->GetCollisionResponseToChannels());
            TestTrue("correct collisions",
                     OutMap.FindChecked("One")[1].ResponseToChannels == MeshTwo->GetCollisionResponseToChannels());
        });
    });

    AfterEach([this]()
    {
        TestSurfaceActor->Destroy();
        TestSurfaceActor = nullptr;
        TestSpawnedActor->Destroy();
        TestSpawnedActor = nullptr;
    });
}
