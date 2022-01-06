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

#include "AmbitWorldHelpers.h"

#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "UObject/UObjectGlobals.h"

BEGIN_DEFINE_SPEC(AmbitWorldHelpersSpec, "Ambit.Unit.AmbitWorldHelpers",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    UWorld* World;
    AStaticMeshActor* TestSurfaceActor;
    AActor* TestComponentActor;
    UBoxComponent* RealBox;
    USplineComponent* RealSpline;
    FString ExpectedActorName;
    FString ExpectedTag_1;
    FString ExpectedTag_2;
    FString ExpectedTagWithSpecialCharacters;
END_DEFINE_SPEC(AmbitWorldHelpersSpec)

void AmbitWorldHelpersSpec::Define()
{
    BeforeEach([this]()
    {
        // Create a empty test map;
        World = FAutomationEditorCommonUtils::CreateNewMap();
        TestNotNull("Check if World is properly created", World);

        const FString Path = "StaticMesh'/Engine/BasicShapes/Plane.Plane'";
        FActorSpawnParameters Params;
        ExpectedActorName = "Plane";
        Params.Name = FName(*ExpectedActorName);

        UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *Path, nullptr, LOAD_None, nullptr);
        if (StaticMesh == nullptr || !StaticMesh->IsValidLowLevelFast())
        {
            UE_LOG(LogAmbit, Error, TEXT("StaticMesh is not there"));
        }

        FVector Location(0, 0, 0);
        FRotator Rotation(0, 0, 0);
        FTransform Transform(Rotation, Location);

        TestSurfaceActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Params);
        if (TestSurfaceActor == nullptr)
        {
            UE_LOG(LogAmbit, Error, TEXT("TestSurfaceActor is null pointer."));
        }
        TestSurfaceActor->SetActorLabel(ExpectedActorName);
        TestSurfaceActor->SetMobility(EComponentMobility::Movable);
        TestSurfaceActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
        ExpectedTag_1 = "Tag_1";
        TestSurfaceActor->Tags.Add(FName(ExpectedTag_1));

        FActorSpawnParameters NewParams;
        FString NewParamName = "Component";
        NewParams.Name = FName(*NewParamName);
        FVector NewLocation(0, 0, 100);
        FRotator NewRotation(0, 0, 0);
        FTransform NewTransform(NewRotation, NewLocation);
        TestComponentActor = World->SpawnActor<AActor>(AActor::StaticClass(), NewTransform, NewParams);

        RealBox = NewObject<UBoxComponent>(TestComponentActor, UBoxComponent::StaticClass(), "RealBox");
        RealSpline = NewObject<USplineComponent>(TestComponentActor, USplineComponent::StaticClass(), "RealSpline");
    });

    Describe("GetActorsByMatchBy()", [this]()
    {
        Describe("MatchBy::NameOrTags", [this]()
        {
            Describe("Single tag", [this]()
            {
                It("with correct name and correct tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with correct name and wrong tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName("WrongTag"));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with wrong name and correct tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with wrong name and wrong tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName("WrongTag"));
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with empty name and correct tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const FString EmptyName = "";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, EmptyName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with empty name and wrong tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName("WrongTag"));
                    const FString EmptyName = "";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, EmptyName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with correct name and empty tag list", [this]()
                {
                    const TArray<FName> TagList;
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with wrong name and empty tag list", [this]()
                {
                    const TArray<FName> TagList;
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with empty name and empty tag list", [this]()
                {
                    const TArray<FName> TagList;
                    const FString EmptyName = "";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, EmptyName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });
            });

            Describe("Multiple tags", [this]()
            {
                BeforeEach([this]()
                {
                    ExpectedTag_2 = "Tag_2";
                    TestSurfaceActor->Tags.Add(FName(ExpectedTag_2));
                });

                It("with correct name and correct tags, expected to get a matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    TagList.Add(FName(ExpectedTag_2));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with correct name and correct tags but in different order, expected to get a matched actor",
                   [this]()
                   {
                       TArray<FName> TagList;
                       TagList.Add(FName(ExpectedTag_2));
                       TagList.Add(FName(ExpectedTag_1));
                       const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                           EMatchBy::NameOrTags, ExpectedActorName, TagList);
                       TestEqual("The number of matched actor", ResultActors.Num(), 1);
                   });

                It("with correct name and only one contained tag, expected to get a matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with correct name and a not contained tag, expected to get a matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    TagList.Add(FName("WrongTag"));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });
                It("with wrong name and a not contained tag, expected no matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    TagList.Add(FName("WrongTag"));
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameOrTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });
            });
        });

        Describe("MatchBy::NameAndTags", [this]()
        {
            Describe("Single Tag", [this]()
            {
                It("with correct name and correct tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with correct name and wrong tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName("WrongTag"));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with wrong name and correct tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with wrong name and wrong tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName("WrongTag"));
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with empty name and correct tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const FString EmptyName = "";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, EmptyName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with empty name and wrong tag", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName("WrongTag"));
                    const FString EmptyName = "";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, EmptyName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with correct name and empty tag", [this]()
                {
                    const TArray<FName> TagList;
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with wrong name and empty tag", [this]()
                {
                    const TArray<FName> TagList;
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with empty name and empty tag", [this]()
                {
                    const TArray<FName> TagList;
                    const FString EmptyName = "";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, EmptyName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });
            });

            Describe("Multiple tags", [this]()
            {
                BeforeEach([this]()
                {
                    ExpectedTag_2 = "Tag_2";
                    TestSurfaceActor->Tags.Add(FName(ExpectedTag_2));
                });

                It("with correct name and correct tags, expected to get a matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    TagList.Add(FName(ExpectedTag_2));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with correct name and correct tags but in different order, expected to get a matched actor",
                   [this]()
                   {
                       TArray<FName> TagList;
                       TagList.Add(FName(ExpectedTag_2));
                       TagList.Add(FName(ExpectedTag_1));
                       const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                           EMatchBy::NameAndTags, ExpectedActorName, TagList);
                       TestEqual("The number of matched actor", ResultActors.Num(), 1);
                   });

                It("with correct name and only one contained tag, expected to get a matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 1);
                });

                It("with correct name and a not contained tag, expected no matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    TagList.Add(FName("WrongTag"));
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, ExpectedActorName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });

                It("with wrong name and correct tags, expected no matched actor", [this]()
                {
                    TArray<FName> TagList;
                    TagList.Add(FName(ExpectedTag_1));
                    TagList.Add(FName(ExpectedTag_2));
                    const FString WrongName = "Cube";
                    const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                        EMatchBy::NameAndTags, WrongName, TagList);
                    TestEqual("The number of matched actor", ResultActors.Num(), 0);
                });
            });
        });

        Describe("Special Characters in the name", [this]()
        {
            BeforeEach([this]()
            {
                ExpectedActorName = "~!@#$%^&*()_+`-={}|[]\'<>?,./";
                TestSurfaceActor->SetActorLabel(ExpectedActorName);
            });

            It("when the name only contains *_+-<>?, the name can be checked and we expect to get a matched actor",
               [this]()
               {
                   TArray<FName> TagList;
                   TagList.Add(FName(ExpectedTag_1));
                   const FString NamePatternWithSpecialCharacter = "*_+-<>?";
                   const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                       EMatchBy::NameAndTags, NamePatternWithSpecialCharacter, TagList);
                   TestEqual("The number of matched actor", ResultActors.Num(), 1);
               });
            It("when the name contains special characters except *_+-<>?, it will not find the matched actor", [this]()
            {
                TArray<FName> TagList;
                TagList.Add(FName(ExpectedTag_1));
                const FString NamePatternWithSpecialCharacter = "~!@#$%^&()";
                const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                    EMatchBy::NameAndTags, NamePatternWithSpecialCharacter, TagList);
                TestEqual("The number of matched actor", ResultActors.Num(), 0);
            });
        });

        Describe("Special Characters in the tag", [this]()
        {
            BeforeEach([this]()
            {
                ExpectedTagWithSpecialCharacters = "~!@#$%^&*()_+`-={}|[]\'<>?,./";
                TestSurfaceActor->Tags.Add(FName(ExpectedTagWithSpecialCharacters));
            });

            It("expected to get a matched actor", [this]()
            {
                TArray<FName> TagList;
                TagList.Add(FName(ExpectedTagWithSpecialCharacters));
                const TArray<AActor*> ResultActors = AmbitWorldHelpers::GetActorsByMatchBy(
                    EMatchBy::NameAndTags, ExpectedActorName, TagList);
                TestEqual("The number of matched actor", ResultActors.Num(), 1);
            });
        });
    });

    Describe("LineTraceBelowWorldPoint()", [this]()
    {
        It("when the LineStart is above the actor, the line trace will hit the actor", [this]()
        {
            const FVector LineStart(0, 0, 10);
            const FHitResult& Hit = AmbitWorldHelpers::LineTraceBelowWorldPoint(LineStart);
            TestTrue("The line trace hit the actor", Hit.IsValidBlockingHit());
        });

        It("when the LineStart is below the actor, the line trace will not hit the actor", [this]()
        {
            const FVector LineStart(0, 0, -10);
            const FHitResult& Hit = AmbitWorldHelpers::LineTraceBelowWorldPoint(LineStart);
            TestFalse("The line trace doesn't hit the actor", Hit.IsValidBlockingHit());
        });
    });

    Describe("GenerateRandomLocationsFromBox()", [this]()
    {
        It("will return an empty array if Box is invalid", [this]()
        {
            AddExpectedError(TEXT("Invalid box component."), EAutomationExpectedErrorFlags::Exact, 1);
            UBoxComponent* EmptyBox = nullptr;
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                EmptyBox, ActorsToSearch, RandomSeed, true);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestEqual("The number of Transforms is 0.", Transforms.Num(), 0);
        });

        It("Can generate random locations when Box's extent is large enough", [this]()
        {
            RealBox->SetBoxExtent(FVector(300, 300, 0));

            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, false);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestTrue("The number of Transforms is greater than 0.", Transforms.Num() > 0);
        });

        It("Cannot generate random locations when Box's extent is not large enough", [this]()
        {
            RealBox->SetBoxExtent(FVector(0, 0, 0));

            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, false);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestEqual("The number of Transforms is equal to 0.", Transforms.Num(), 0);
        });

        It("Can snap to surface below if provided in ActorsToSearch array", [this]()
        {
            RealBox->SetBoxExtent(FVector(200, 200, 0));
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, false, DensityMin, DensityMax);

            for (const FTransform& OneTransform : Transforms)
            {
                TestEqual("All of the generated locations have snapped to the surface provided.",
                          TestSurfaceActor->GetActorLocation().Z, OneTransform.GetLocation().Z, 0.05f);
            }
        });

        It("Can snap to unspecified surface below if bSnapToSurfaceBelow is true when ActorsToSearch is empty", [this]()
        {
            RealBox->SetBoxExtent(FVector(200, 200, 0));
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = true;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            bool bAllLocationsSnapped = true;
            for (const FTransform& OneTransform : Transforms)
            {
                TestEqual("All of the generated locations have snapped to the surface below.",
                          TestSurfaceActor->GetActorLocation().Z, OneTransform.GetLocation().Z, 0.05f);
            }
        });

        It("Will generate random rotations only within the provided range", [this]()
        {
            const float RotationMin = 30.0f;
            const float RotationMax = 90.0f;
            RealBox->SetBoxExtent(FVector(300, 300, 0));

            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, false, DensityMin, DensityMax, RotationMin, RotationMax);

            for (const FTransform& OneTransform : Transforms)
            {
                UE_LOG(LogAmbit, Display, TEXT("Yaw: %f"), OneTransform.Rotator().Yaw);
                TestTrue("We expect all of the generated rotations to be within the provided range",
                         RotationMin < OneTransform.Rotator().Yaw && RotationMax > OneTransform.Rotator().Yaw);
            }
        });

        It("Use same random seed will have the exact same FTransform set", [this]()
        {
            RealBox->SetBoxExtent(FVector(200, 200, 0));
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            TArray<FTransform> ExpectedArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);

            UE_LOG(LogAmbit, Display, TEXT("The number of ExpectedArray is: %d"), ExpectedArray.Num());
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());
            TestEqual("With the same random seed, we expect they will have the same size.", ActualArray.Num(),
                      ExpectedArray.Num());
            for (int i = 0; i < ActualArray.Num(); ++i)
            {
                TestTrue("With the same random seed, we expect each corresponding elements are same.",
                         ExpectedArray[i].Equals(ActualArray[i]));
            }
        });

        It("Use different random seed will have different set", [this]()
        {
            RealBox->SetBoxExtent(FVector(200, 200, 0));
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed_1 = 0;
            const int32 RandomSeed_2 = 1;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            TArray<FTransform> ExpectedArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed_1, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed_2, bSnapToSurfaceBelow, DensityMin, DensityMax);

            UE_LOG(LogAmbit, Display, TEXT("The number of ExpectedArray is: %d"), ExpectedArray.Num());
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());

            bool bNotEqual = false;
            if (ExpectedArray.Num() != ActualArray.Num())
            {
                bNotEqual = true;
            }
            else
            {
                for (int i = 0; i < ActualArray.Num(); ++i)
                {
                    // At least one element is unique to the second array
                    if (!ExpectedArray[i].Equals(ActualArray[i]))
                    {
                        bNotEqual = true;
                        break;
                    }
                }
            }

            TestTrue("With different random seeds, we expect it will have different sets.", bNotEqual);
        });

        It("Will generate the right amount of FTransforms for a rotated box", [this]()
        {
            RealBox->SetBoxExtent(FVector(500, 500, 0));
            RealBox->SetRelativeRotation(FRotator(0, 45, 0).Quaternion());
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform> RotatedArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);

            RealBox->SetRelativeRotation(FRotator(0, 0, 0).Quaternion());
            const TArray<FTransform> NormalArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);

            UE_LOG(LogAmbit, Display, TEXT("The number of RotatedArray is: %d"), RotatedArray.Num());
            UE_LOG(LogAmbit, Display, TEXT("The number of NormalArray is: %d"), NormalArray.Num());

            TestEqual("The rotated box and the normal box should generate the same number of FTransforms.",
                      NormalArray.Num(), RotatedArray.Num());
        });

        It("Can handle when DensityMin is greater than DensityMax", [this]()
        {
            AddExpectedError(
                TEXT("DensityMin is greater than DensityMax. No actors spawned."), EAutomationExpectedErrorFlags::Exact,
                1);
            RealBox->SetBoxExtent(FVector(500, 500, 0));
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMax, DensityMin);
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());

            TestEqual("The array should be empty.", ActualArray.Num(), 0);
        });

        It("Can handle when RotationMin is greater than RotationMax", [this]()
        {
            AddExpectedError(
                TEXT("RotationMin is greater than RotationMax. No actors spawned."),
                EAutomationExpectedErrorFlags::Exact, 1);
            RealBox->SetBoxExtent(FVector(500, 500, 0));
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;
            const TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax, 90.f, 45.f);
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());

            TestEqual("The array should be empty.", ActualArray.Num(), 0);
        });

        It("will generate zero locations if bSnapToSurfaceBelow is true but there are no surfaces below", [this]()
        {
            TestComponentActor->SetActorLocation(FVector(0, 0, -100));
            RealBox->SetBoxExtent(FVector(500, 500, 0));
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = true;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TestComponentActor->SetActorLocation(FVector(0, 0, 0));
            TestEqual("The array is empty", Transforms.Num(), 0);
        });

        It("will generate zero locations if a hit surface is specified but the bounding box is below it", [this]()
        {
            TestComponentActor->SetActorLocation(FVector(0, 0, -100));
            RealBox->SetBoxExtent(FVector(500, 500, 0));
            const FVector Scale3D(100, 100, 0);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Push(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromBox(
                RealBox, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TestComponentActor->SetActorLocation(FVector(0, 0, 0));
            TestEqual("The array is empty", Transforms.Num(), 0);
        });
    });

    Describe("GenerateRandomLocationsFromSpline()", [this]()
    {
        It("will return an empty array if Spline is invalid", [this]()
        {
            AddExpectedError(TEXT("Invalid spline component."), EAutomationExpectedErrorFlags::Exact, 1);
            USplineComponent* EmptySpline = nullptr;
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                EmptySpline, ActorsToSearch, RandomSeed, true);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestEqual("The number of Transforms is equal to 0.", Transforms.Num(), 0);
        });

        It("can generate random locations on a closed loop Spline", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            RealSpline->SetClosedLoop(true);
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, false);
            RealSpline->SetClosedLoop(false);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestTrue("The number of Transforms is greater than 0.", Transforms.Num() > 0);
        });

        It("Can generate random locations when Spline's length is large enough", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);

            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, false);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestTrue("The number of Transforms is greater than 0.", Transforms.Num() > 0);
        });

        It("Cannot generate random locations when Spline's length is not large enough", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 1, 100), ESplineCoordinateSpace::World);

            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, false);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestEqual("The number of Transforms is equal to 0.", Transforms.Num(), 0);
        });

        It("Can snap to surface below if provided in ActorsToSearch array", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.02f;
            const float DensityMax = 0.5f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, false, DensityMin, DensityMax);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());

            for (const FTransform& OneTransform : Transforms)
            {
                UE_LOG(LogAmbit, Display, TEXT("Test Surface: %f, Object: %f"), TestSurfaceActor->GetActorLocation().Z,
                       OneTransform.GetLocation().Z);
                TestEqual("The generated location has snapped to the surface provided.",
                          TestSurfaceActor->GetActorLocation().Z, OneTransform.GetLocation().Z, 0.05f);
            }
        });

        It("Can snap to unspecified surface below if bSnapToSurfaceBelow is true when ActorsToSearch is empty", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = true;
            const float DensityMin = 0.02f;
            const float DensityMax = 0.5f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());

            for (const FTransform& OneTransform : Transforms)
            {
                UE_LOG(LogAmbit, Display, TEXT("Test Surface: %f, Object: %f"), TestSurfaceActor->GetActorLocation().Z,
                       OneTransform.GetLocation().Z);
                TestEqual("The generated location has snapped to the surface below.",
                          TestSurfaceActor->GetActorLocation().Z, OneTransform.GetLocation().Z, 0.05f);
            }
        });

        It("Will generate random rotations only within the provided range", [this]()
        {
            const float RotationMin = 30.0f;
            const float RotationMax = 90.0f;
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);

            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const float DensityMin = 0.02f;
            const float DensityMax = 0.5f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, false, DensityMin, DensityMax, RotationMin, RotationMax);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());

            for (const FTransform& OneTransform : Transforms)
            {
                UE_LOG(LogAmbit, Display, TEXT("Yaw: %f"), OneTransform.Rotator().Yaw);
                TestTrue("We expect all of the generated rotations to be within the provided range",
                         RotationMin < OneTransform.Rotator().Yaw && RotationMax > OneTransform.Rotator().Yaw);
            }
        });

        It("Use same random seed will have the exact same FTransform set", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.02f;
            const float DensityMax = 0.5f;

            TArray<FTransform> ExpectedArray = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);

            UE_LOG(LogAmbit, Display, TEXT("The number of ExpectedArray is: %d"), ExpectedArray.Num());
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());
            TestEqual("With the same random seed, we expect they will have the same size.", ActualArray.Num(),
                      ExpectedArray.Num());
            for (int i = 0; i < ActualArray.Num(); ++i)
            {
                TestTrue("With the same random seed, we expect each corresponding elements are same.",
                         ExpectedArray[i].Equals(ActualArray[i]));
            }
        });

        It("Use different random seed will have different set", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed_1 = 0;
            const int32 RandomSeed_2 = 1;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.02f;

            const float DensityMax = 0.5f;

            TArray<FTransform> DifferentArray = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed_1, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed_2, bSnapToSurfaceBelow, DensityMin, DensityMax);

            UE_LOG(LogAmbit, Display, TEXT("The number of DifferentArray is: %d"), DifferentArray.Num());
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());
            bool bNotEqual = false;
            if (DifferentArray.Num() != ActualArray.Num())
            {
                bNotEqual = true;
            }
            else
            {
                for (int i = 0; i < ActualArray.Num(); ++i)
                {
                    // At least one element is unique to the second array
                    if (!DifferentArray[i].Equals(ActualArray[i]))
                    {
                        bNotEqual = true;
                        break;
                    }
                }
            }

            TestTrue("With different random seeds, we expect it will have different sets.", bNotEqual);
        });

        It("Can handle when DensityMin is greater than DensityMax", [this]()
        {
            AddExpectedError(
                TEXT("DensityMin is greater than DensityMax. No actors spawned."), EAutomationExpectedErrorFlags::Exact,
                1);
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMax, DensityMin);

            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());

            TestEqual("The array should be empty.", ActualArray.Num(), 0);
        });

        It("Can handle when RotationMin is greater than RotationMax", [this]()
        {
            AddExpectedError(
                TEXT("RotationMin is greater than RotationMax. No actors spawned."),
                EAutomationExpectedErrorFlags::Exact, 1);
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax, 90.f, 45.f);

            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());
            TestEqual("The array should be empty.", ActualArray.Num(), 0);
        });

        It("will generate zero locations if bSnapToSurfaceBelow is true but there are no surfaces below", [this]()
        {
            TestComponentActor->SetActorLocation(FVector(0, 0, -100));
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = true;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TestComponentActor->SetActorLocation(FVector(0, 0, 0));
            TestEqual("The array is empty", Transforms.Num(), 0);
        });

        It("will generate zero locations if a hit surface is specified but the spline is below it", [this]()
        {
            TestComponentActor->SetActorLocation(FVector(0, 0, -100));
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            const FVector Scale3D(100, 100, 0);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Push(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const bool bSnapToSurfaceBelow = false;
            const float DensityMin = 0.5f;
            const float DensityMax = 1.0f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, bSnapToSurfaceBelow, DensityMin, DensityMax);
            TestComponentActor->SetActorLocation(FVector(0, 0, 0));
            TestEqual("The array is empty", Transforms.Num(), 0);
        });

        It("Will generate objects relative to spline rotation if bFollowSplineRotation is true", [this]()
        {
            const bool bFollowSplineRotation = true;

            const float RotationMin = 120.0f;
            const float RotationMax = 120.0f;
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 1000, 0), ESplineCoordinateSpace::World);

            const TArray<AActor*> ActorsToSearch;
            const int32 RandomSeed = 0;
            const float DensityMin = 1.0f;
            const float DensityMax = 1.0f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
                RealSpline, ActorsToSearch, RandomSeed, false, DensityMin, DensityMax, RotationMin, RotationMax,
                bFollowSplineRotation);

            const float ExpectedRotation = 210.0f;

            for (const FTransform& OneTransform : Transforms)
            {
                const float SplineRotation = OneTransform.Rotator().GetDenormalized().Yaw;

                TestTrue(
                    "We expect the determined transform to be equal to the sum of the spline rotation and min rotation value",
                    FMath::IsNearlyEqual(SplineRotation, ExpectedRotation, 0.01f));
            }
        });
    });

    Describe("GenerateRandomLocationsFromActor()", [this]()
    {
        It("Can generate random locations when SurfaceActor is large enough", [this]()
        {
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed, DensityMin, DensityMax);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestTrue("The number of Transforms is greater than 1.", Transforms.Num() > 0);
        });

        It("Cannot generate random locations when SurfaceActor is not large enough", [this]()
        {
            const FVector Scale3D(1, 1, 1);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed, DensityMin, DensityMax);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transforms is: %d"), Transforms.Num());
            TestEqual("The number of Transforms is equal to 0.", Transforms.Num(), 0);
        });

        It("Will generate random rotations only within the provided range", [this]()
        {
            const float RotationMin = 30.0f;
            const float RotationMax = 90.0f;

            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;

            const TArray<FTransform>& Transforms = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed, DensityMin, DensityMax, RotationMin, RotationMax);

            for (const FTransform& OneTransform : Transforms)
            {
                UE_LOG(LogAmbit, Display, TEXT("Yaw: %f"), OneTransform.Rotator().Yaw);
                TestTrue("We expect all of the generated rotations to be within the provided range",
                         RotationMin < OneTransform.Rotator().Yaw && RotationMax > OneTransform.Rotator().Yaw);
            }
        });

        It("Use same random seed will have the exact same FTransform set", [this]()
        {
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;

            TArray<FTransform> ExpectedArray = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed, DensityMin, DensityMax);
            //TSet<FTransform> ExpectedSet = TSet<FTransform>(ExpectedArray);
            TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed, DensityMin, DensityMax);
            //TSet<FTransform> ActualSet = TSet<FTransform>(ActualArray);

            UE_LOG(LogAmbit, Display, TEXT("The number of ExpectedArray is: %d"), ExpectedArray.Num());
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());
            TestEqual("With the same random seed, we expect they will have the same size.", ActualArray.Num(),
                      ExpectedArray.Num());
            for (int i = 0; i < ActualArray.Num(); ++i)
            {
                TestTrue("With the same random seed, we expect each corresponding elements are same.",
                         ExpectedArray[i].Equals(ActualArray[i]));
            }
        });

        It("Use different random seed will have different set", [this]()
        {
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed_1 = 0;
            const int32 RandomSeed_2 = 1;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;

            TArray<FTransform> DifferentArray = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed_1, DensityMin, DensityMax);
            TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed_2, DensityMin, DensityMax);

            UE_LOG(LogAmbit, Display, TEXT("The number of DifferentArray is: %d"), DifferentArray.Num());
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());
            bool bNotEqual = false;
            if (DifferentArray.Num() != ActualArray.Num())
            {
                bNotEqual = true;
            }
            else
            {
                for (int i = 0; i < ActualArray.Num(); ++i)
                {
                    // At least one element is unique to the second array
                    if (!DifferentArray[i].Equals(ActualArray[i]))
                    {
                        bNotEqual = true;
                        break;
                    }
                }
            }

            TestTrue("With different random seeds, we expect it will have different sets.", bNotEqual);
        });

        It("Can handle when DensityMin is greater than DensityMax", [this]()
        {
            AddExpectedError(
                TEXT("DensityMin is greater than DensityMax. No actors spawned."), EAutomationExpectedErrorFlags::Exact,
                1);
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;
            const TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed, DensityMax, DensityMin);
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());
            TestEqual("The array should be empty.", ActualArray.Num(), 0);
        });

        It("Can handle when RotationMin is greater than RotationMax", [this]()
        {
            AddExpectedError(
                TEXT("RotationMin is greater than RotationMax. No actors spawned."),
                EAutomationExpectedErrorFlags::Exact, 1);
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToSearch;
            ActorsToSearch.Add(TestSurfaceActor);
            const int32 RandomSeed = 0;
            const float DensityMin = 0.05f;
            const float DensityMax = 0.2f;

            const TArray<FTransform> ActualArray = AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                ActorsToSearch, RandomSeed, DensityMin, DensityMax, 90.f, 45.f);
            UE_LOG(LogAmbit, Display, TEXT("The number of ActualArray is: %d"), ActualArray.Num());

            TestEqual("The array should be empty.", ActualArray.Num(), 0);
        });
    });

    Describe("CheckAndSnapToSurface()", [this]()
    {
        It("Will return an unchanged FTransform if ActorsToHit is empty and bSnapToSurfaceBelow is false", [this]()
        {
            const TArray<AActor*> ActorsToHit;
            const bool bSnapToSurfaceBelow = false;

            const FVector Location(0, 0, 100);
            const FRotator Rotation(0, 0, 0);
            FTransform Transform(Rotation, Location);
            const FTransform OriginalTransform(Rotation, Location);

            AmbitWorldHelpers::CheckAndSnapToSurface(Transform, ActorsToHit, bSnapToSurfaceBelow);

            TestTrue("We expect the element of the adjusted FTransform to be the same as the original FTransform.",
                     OriginalTransform.Equals(Transform));
        });

        It("Will return an changed FTransform if ActorsToHit is NOT empty and bSnapToSurfaceBelow is false", [this]()
        {
            const FVector Scale3D(100, 100, 100);
            TestSurfaceActor->SetActorScale3D(Scale3D);
            TArray<AActor*> ActorsToHit;
            ActorsToHit.Add(TestSurfaceActor);
            const bool bSnapToSurfaceBelow = false;

            const FVector Location(0, 0, 100);
            const FRotator Rotation(0, 0, 0);
            FTransform Transform(Rotation, Location);
            const FTransform OriginalTransform(Rotation, Location);

            AmbitWorldHelpers::CheckAndSnapToSurface(Transform, ActorsToHit, bSnapToSurfaceBelow);

            TestFalse("We expect the element of the adjusted FTransform to be different from the original FTransform.",
                      OriginalTransform.Equals(Transform));
        });

        It("Will return an changed FTransform if ActorsToHit is empty and bSnapToSurfaceBelow is true", [this]()
        {
            const TArray<AActor*> ActorsToHit;
            const bool bSnapToSurfaceBelow = true;

            const FVector Location(0, 0, 100);
            const FRotator Rotation(0, 0, 0);
            FTransform Transform(Rotation, Location);
            const FTransform OriginalTransform(Rotation, Location);

            AmbitWorldHelpers::CheckAndSnapToSurface(Transform, ActorsToHit, bSnapToSurfaceBelow);

            TestFalse("We expect the element of the adjusted FTransform to be different from the original FTransform.",
                      OriginalTransform.Equals(Transform));
        });
    });

    Describe("GenerateFixedLocationsFromSpline()", [this]()
    {
        It("will return an empty array if Spline is invalid", [this]()
        {
            AddExpectedError(TEXT("Invalid spline component."), EAutomationExpectedErrorFlags::Exact, 1);
            USplineComponent* EmptySpline = nullptr;

            const float Distance = 100;
            const TArray<FTransform> Transform = AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
                EmptySpline, Distance);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transform is: %d"), Transform.Num());
            TestEqual("The number of Transform is equal to 0.", Transform.Num(), 0);
        });

        It("can generate fixed locations on a closed loop Spline", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);
            RealSpline->SetClosedLoop(true);

            const float Distance = 100;
            const TArray<FTransform> Transform = AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
                RealSpline, Distance);
            RealSpline->SetClosedLoop(false);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transform is: %d"), Transform.Num());
            TestTrue("The number of Transform is greater than 0.", Transform.Num() > 0);
        });

        It("Can generate fixed locations when Spline's length is larger than the distance", [this]()
        {
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 5000, 0), ESplineCoordinateSpace::Local);

            const float Distance = 100;
            const TArray<FTransform> Transform = AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
                RealSpline, Distance);
            UE_LOG(LogAmbit, Display, TEXT("The number of Transform is: %d"), Transform.Num());
            TestTrue("The number of Transform is greater than 0.", Transform.Num() > 0);
        });

        It("Cannot generate fixed locations when Spline's length is smaller than the distance", [this]()
        {
            AddExpectedError(
                TEXT("Distance must be smaller than the total length of Spline."), EAutomationExpectedErrorFlags::Exact,
                1);
            RealSpline->SetLocationAtSplinePoint(1, FVector(0, 1, 0), ESplineCoordinateSpace::Local);
            const float Distance = 100;
            const TArray<FTransform> Transform = AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
                RealSpline, Distance);

            UE_LOG(LogAmbit, Display, TEXT("The number of Transform is: %d"), Transform.Num());

            TestEqual("The Transform array should be empty.", Transform.Num(), 0);
        });
    });

    AfterEach([this]()
    {
        TestSurfaceActor->Destroy();
        TestSurfaceActor = nullptr;
        TestComponentActor->Destroy();
        TestComponentActor = nullptr;
    });
}
