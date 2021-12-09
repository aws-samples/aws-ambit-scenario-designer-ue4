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

#include "SpawnerBase.h"

#include "EngineUtils.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Ambit/AmbitModule.h"
#include "Ambit/Actors/SpawnedObjectConfigs/SpawnedObjectConfig.h"
#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/AmbitSpawnerCollisionHelpers.h"
#include "Ambit/Utils/UserMetricsSubsystem.h"
#include "AmbitUtils/JsonHelpers.h"

#include <AmbitUtils/MenuHelpers.h>

// Sets default values
ASpawnerBase::ASpawnerBase()
{
    IconComponent = CreateDefaultSubobject<UBillboardComponent>("Icon");
}

void ASpawnerBase::GenerateSpawnedObjectConfiguration()
{
    GenerateSpawnedObjectConfiguration(RandomSeed);
}

void ASpawnerBase::GenerateSpawnedObjectConfiguration(int32 Seed)
{
    USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();

    const int32 OriginalSeed = RandomSeed;
    RandomSeed = Seed;

    Config->SpawnedObjects = GenerateActors();

    DestroyGeneratedActors();
    RandomSeed = OriginalSeed;

    auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);
    OnSpawnedObjectConfigCompleted.ExecuteIfBound(FinalConfig, true);
}

template <typename Struct>
TSharedPtr<Struct> ASpawnerBase::GetConfiguration() const
{
    TSharedPtr<Struct> Config = MakeShareable(new Struct);
    Config->SpawnerLocation = this->GetActorLocation();
    Config->SpawnerRotation = this->GetActorRotation();
    Config->MatchBy = MatchBy;
    Config->SurfaceNamePattern = SurfaceNamePattern;
    Config->SurfaceTags = SurfaceTags;
    Config->DensityMin = DensityMin;
    Config->DensityMax = DensityMax;
    Config->RotationMin = RotationMin;
    Config->RotationMax = RotationMax;
    Config->bAddPhysics = bAddPhysics;
    TArray<TSubclassOf<AActor>> ActorsToSpawnClean;
    for (const auto& Actor : ActorsToSpawn)
    {
        ActorsToSpawnClean.AddUnique(Actor);
    }
    Config->ActorsToSpawn = ActorsToSpawnClean;
    Config->bRemoveOverlaps = bRemoveOverlaps;
    Config->RandomSeed = RandomSeed;

    return Config;
}

template <typename Struct>
void ASpawnerBase::Configure(const TSharedPtr<Struct>& Config)
{
    MatchBy = Config->MatchBy;
    SurfaceNamePattern = Config->SurfaceNamePattern;
    SurfaceTags = Config->SurfaceTags;
    DensityMin = Config->DensityMin;
    DensityMax = Config->DensityMax;
    RotationMin = Config->RotationMin;
    RotationMax = Config->RotationMax;
    bAddPhysics = Config->bAddPhysics;
    ActorsToSpawn = Config->ActorsToSpawn;
    bRemoveOverlaps = Config->bRemoveOverlaps;
    RandomSeed = Config->RandomSeed;
}

// Called when the game starts or when spawned
void ASpawnerBase::BeginPlay()
{
    Super::BeginPlay();
    GenerateActors();

    const TSharedRef<FJsonObject> MetricContextData = MakeShareable(new FJsonObject);
    MetricContextData->SetNumberField(UserMetrics::AmbitSpawner::KAmbitSpawnerSpawnNumberContextData,
                                      SpawnedActors.Num());
    GEngine->GetEngineSubsystem<UUserMetricsSubsystem>()->Track(UserMetrics::AmbitSpawner::KAmbitSpawnerRunEvent,
                                                                UserMetrics::AmbitSpawner::KAmbitSpawnerNameSpace,
                                                                MetricContextData);
}

void ASpawnerBase::PostActorCreated()
{
    //Currently there is a bug that this function could be called twice in UE engine
    if (!HasAllFlags(RF_Transient))
    {
        GEngine->GetEngineSubsystem<UUserMetricsSubsystem>()->Track(UserMetrics::AmbitSpawner::KAmbitSpawnerPlacedEvent,
                                                                    UserMetrics::AmbitSpawner::KAmbitSpawnerNameSpace);
    }
}


void ASpawnerBase::DestroyGeneratedActors()
{
    // Remove previously created actors.
    for (AActor* Actor : SpawnedActors)
    {
        // This class does not explicitly retain strong references to the actors in the
        // SpawnedActors array. Therefore, we must check for null pointers to
        // accommodate cases where the actor no longer exists.
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }

    SpawnedActors.Empty();
}

bool ASpawnerBase::HasActorsToSpawn() const
{
    return ActorsToSpawn.Num() > 0 && !ActorsToSpawn.Contains(nullptr);
}

void ASpawnerBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    PostEditErrorFixes();
}

bool ASpawnerBase::AreParametersValid() const
{
    if (ActorsToSpawn.Num() > 0)
    {
        for (const auto& Actor : ActorsToSpawn)
        {
            if (!IsValid(Actor))
            {
                const FString& Message = FString::Printf(
                    TEXT("An actor to spawn in the array of %s is not specified, which is not allowed."),
                    *this->GetActorLabel());
                FMenuHelpers::DisplayMessagePopup(Message, "Warning");
                return false;
            }
        }
    }
    else
    {
        const FString& Message = FString::Printf(
            TEXT("The array ActorsToSpawn of %s is not specified, which is not allowed."), *this->GetActorLabel());
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
        return false;
    }
    return AreMinMaxValid();
}

bool ASpawnerBase::AreMinMaxValid() const
{
    if (RotationMin > RotationMax)
    {
        const FString& Message = FString::Printf(
            TEXT(
                "The minimum degree of rotation is greater than the maximum degree of rotation in %s, which is not allowed."),
            *this->GetActorLabel());
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
        return false;
    }
    if (DensityMin > DensityMax)
    {
        const FString& Message = FString::Printf(
            TEXT("The minimum density is greater than the maximum density in %s, which is not allowed."),
            *this->GetActorLabel());
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
        return false;
    }
    return true;
}

void ASpawnerBase::PostEditErrorFixes()
{
    if (bRestrictToOneRotation && RotationMax != RotationMin)
    {
        RotationMax = RotationMin;
    }
    const float DensityWarningLimit = 3.f;
    const float PracticalMax = FMath::Max(DensityMin, DensityMax);
    if (PracticalMax > DensityWarningLimit)
    {
        // TODO: This text should eventually support localization.
        const FString& Message = FString::Printf(
            TEXT(
                "You've set a maximum density of %s items per square meter in %s, which is pretty high. Be aware that high density values may cause performance problems when you run your simulation."),
            *FString::SanitizeFloat(PracticalMax), *this->GetActorLabel());
        FMenuHelpers::DisplayMessagePopup(Message, "Warning");
    }
}

void ASpawnerBase::CleanAndSetUpActorsToSpawn(TArray<TSubclassOf<AActor>>& OutArray,
                                              TMap<FString, TArray<FCollisionResponseTemplate>>& OutMap)
{
    OutMap.Empty();
    OutArray.Empty();
    for (const TSubclassOf<AActor>& Actor : ActorsToSpawn)
    {
        const int32 LastIndex = OutArray.Num() - 1;
        // AddUnique returns the Index of the provided Actor
        // if it already exists in the array; if not,
        // AddUnique adds Actor to the end of the array;
        // this checks if the actor already existed, and if so,
        // notifies the user that a duplicate was found in the array
        if (OutArray.AddUnique(Actor) <= LastIndex)
        {
            UE_LOG(LogAmbit, Warning, TEXT("%s: Duplicate of %s found in ActorsToSpawn. Ignoring..."),
                   *this->GetActorLabel(), *Actor->GetName());
        }
        else
        {
            TArray<UStaticMeshComponent*> StaticMeshComponents;
            AmbitSpawnerCollisionHelpers::FindDefaultStaticMeshComponents(Actor.Get(), StaticMeshComponents);
            AmbitSpawnerCollisionHelpers::StoreCollisionProfiles(Actor->GetPathName(), StaticMeshComponents, OutMap);
            AmbitSpawnerCollisionHelpers::SetCollisionForAllStaticMeshComponents(StaticMeshComponents, bRemoveOverlaps);
        }
    }
}

void ASpawnerBase::SpawnActorsAtTransforms(const TArray<FTransform>& Transforms,
                                           TMap<FString, TArray<FTransform>>& OutMap)
{
    OutMap.Empty();

    Random.Initialize(RandomSeed);
    UWorld* World = GetWorld();
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);
    TMap<FString, TArray<bool>> OriginalGenerateOverlapEventsMap;
    for (AActor* Actor : AllActors)
    {
        TArray<bool> OriginalGenerateOverlapEvents;
        // Set GenerateOverlapEvents to true while Play mode is active
        AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(Actor, OriginalGenerateOverlapEvents);
        // Store original overlap event settings
        OriginalGenerateOverlapEventsMap.Add(Actor->GetName(), OriginalGenerateOverlapEvents);
    }

    // Remove duplicates from array and set up collision profiles for ActorsToSpawn
    TArray<TSubclassOf<AActor>> ActorsToSpawnClean;
    TMap<FString, TArray<FCollisionResponseTemplate>> OriginalCollisionProfiles;
    CleanAndSetUpActorsToSpawn(ActorsToSpawnClean, OriginalCollisionProfiles);

    for (const FTransform& Transform : Transforms)
    {
        FVector SpawnedActorLocation = Transform.GetLocation();
        const FRotator& SpawnedActorRotation = Transform.Rotator();

        int32 RandomIndex = 0;
        if (ActorsToSpawnClean.Num() > 1)
        {
            RandomIndex = Random.RandRange(0, ActorsToSpawnClean.Num() - 1);
        }
        // ActorsToSpawnClean will always have at least one element;
        // it contains all elements of a non-empty ActorsToSpawn (with duplicates removed)
        TSubclassOf<AActor> ChosenActor = ActorsToSpawnClean[RandomIndex];

        FActorSpawnParameters ActorSpawnParams;
        ActorSpawnParams.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

        // Try to spawn actor at location
        // will do nothing if it would overlap with any other spawned actor
        AActor* SpawnedActor = World->SpawnActor(ChosenActor.Get(), &SpawnedActorLocation, &SpawnedActorRotation,
                                                 ActorSpawnParams);

        // Try to spawn actor again at offset
        // if bAddPhysics is true and first spawn attempt failed
        // in order to potentially "stack"
        // TODO: If Add Physics is turned on, should AmbitSpawners spawn obstacles one at a time?
        // TODO: Unreal Engine does not necessarily have a set order in which Actors are processed,
        // so this may be non-deterministic
        if (!IsValid(SpawnedActor) && bAddPhysics)
        {
            FVector LocationOffset(0, 0, 100);
            SpawnedActorLocation = SpawnedActorLocation + LocationOffset;
            SpawnedActor = World->SpawnActor(ChosenActor.Get(), &SpawnedActorLocation, &SpawnedActorRotation,
                                             ActorSpawnParams);
        }

        if (IsValid(SpawnedActor))
        {
            UStaticMeshComponent* PhysicsComponent = SpawnedActor->FindComponentByClass<UStaticMeshComponent>();
            if (bAddPhysics)
            {
                // Set mobility
                PhysicsComponent->SetMobility(EComponentMobility::Movable);

                // If actor could not be spawned at surface level,
                // we want to sweep it to the surface and check for collision along the way
                // before enabling SimulatePhysics
                if (SpawnedActor->GetActorLocation() != Transform.GetLocation())
                {
                    PhysicsComponent->SetWorldLocation(Transform.GetLocation(), true, nullptr,
                                                       ETeleportType::ResetPhysics);
                }
            }
            // Check for any overlaps and verify that overlaps are not beyond the surface
            // of the overlapping actor
            TArray<UPrimitiveComponent*> OverlappingComponents;
            SpawnedActor->GetOverlappingComponents(OverlappingComponents);
            for (UPrimitiveComponent* OverlappingComponent : OverlappingComponents)
            {
                if (AmbitSpawnerCollisionHelpers::IsPenetratingOverlap(OverlappingComponent, SpawnedActor))
                {
                    SpawnedActor->Destroy();
                    break;
                }
            }

            // Makes sure that SpawnedActor did not have any overlaps
            // and was not destroyed
            if (IsValid(SpawnedActor))
            {
                // Set the collision profile(s) of this ActorToSpawn instance
                // to the original collision profile(s) of the class default object
                TArray<UStaticMeshComponent*> StaticMeshComponents;
                SpawnedActor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

                const FString& PathName = ChosenActor.Get()->GetPathName();
                const TArray<FCollisionResponseTemplate> OriginalResponses = OriginalCollisionProfiles.FindChecked(
                    PathName);
                for (int i = 0; i < StaticMeshComponents.Num(); i++)
                {
                    UStaticMeshComponent* StaticMeshComponent = StaticMeshComponents[i];
                    FCollisionResponseTemplate Response = OriginalResponses[i];

                    // Restores the collision profile of this specific actor
                    // to match expected collision behavior of the asset
                    // Maintains ObjectType as "AmbitSpawnerObstacle"
                    // to ensure no overlaps occur with future spawned objects
                    StaticMeshComponent->SetCollisionResponseToChannels(Response.ResponseToChannels);
                    StaticMeshComponent->SetCollisionEnabled(Response.CollisionEnabled);

                    // Maintains that "Overlappable" Obstacles
                    // are continued to be recognized as such
                    // This is set to Overlap to ensure that
                    // actors (not spawned by AmbitSpawners)
                    // will respond to the spawned obstacle correctly
                    // if it is supposed to generate overlap events.
                    // The AmbitSpawner Overlap detection/destruction
                    // will ignore AMBIT_SPAWNER_OVERLAP typed objects.
                    StaticMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2, // AMBIT_SPAWNED_OVERLAP
                                                                       ECR_Overlap);
                }

                // Turn on Simulate Physics if bAddPhysics is true
                if (bAddPhysics && !PhysicsComponent->IsSimulatingPhysics())
                {
                    PhysicsComponent->SetSimulatePhysics(true);
                }

                // Add FTransform to map for SDF export
                SpawnedActors.Push(SpawnedActor);
                TArray<FTransform> PathNameTransforms;
                if (bAddPhysics)
                {
                    PathNameTransforms.Add(PhysicsComponent->GetComponentTransform());
                }
                else
                {
                    PathNameTransforms.Add(SpawnedActor->GetActorTransform());
                }
                // Update map array value to include new transform
                if (OutMap.Find(PathName) != nullptr)
                {
                    PathNameTransforms.Append(OutMap.FindAndRemoveChecked(PathName));
                }
                OutMap.Add(PathName, PathNameTransforms);
            }
        }
    }
    // Restore CDO collision profiles to original
    AmbitSpawnerCollisionHelpers::ResetCollisionProfiles(OriginalCollisionProfiles, ActorsToSpawnClean);

    for (const auto& Actor : AllActors)
    {
        TArray<bool> OriginalGenerateOverlapEvents = OriginalGenerateOverlapEventsMap.FindChecked(Actor->GetName());
        // Reset GenerateOverlapEvents
        AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(Actor, OriginalGenerateOverlapEvents, true);
    }
}
