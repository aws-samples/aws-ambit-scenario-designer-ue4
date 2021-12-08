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

#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

#include "Ambit/AmbitModule.h"
#include "Ambit/Mode/Constant.h"

// TODO: Figure out a way to change DefaultEngine.ini in code
// to indicate that GameTraceChannel1 is for "Spawned Obstacles"
// This may not work if customers have used Unreal Engine to
// define custom trace channels that happen to use this same channel
#define AMBIT_SPAWNED_OBSTACLE ECC_GameTraceChannel1
#define AMBIT_SPAWNED_OVERLAP ECC_GameTraceChannel2

bool AmbitSpawnerCollisionHelpers::IsPenetratingOverlap(
    UPrimitiveComponent* OverlappingComponent,
    AActor* SpawnedActor)
{
    // Filter out overlaps if they are overlapping AMBIT_SPAWNER_OVERLAP objects
    if (OverlappingComponent->GetCollisionObjectType()
        == AMBIT_SPAWNED_OVERLAP)
    {
        return false;
    }
    // Delete if spawned obstacle is overlapping another spawned obstacle
    if (OverlappingComponent->GetCollisionObjectType()
        == AMBIT_SPAWNED_OBSTACLE)
    {
        return true;
    }
    const FVector& SpawnedActorLocation =
            SpawnedActor->GetActorLocation();
    const FVector& LocationOfOverlappingComp =
            OverlappingComponent->GetComponentLocation();

    const float DistanceBetweenActorAndComp =
            FVector::Distance(SpawnedActorLocation,
                              LocationOfOverlappingComp) / 100.f;

    // Gets point on collision that is closest
    // to the location of the spawned actor
    // assumes that the anchor point of the spawned actor
    // is at the bottom of its mesh
    FVector CollisionPointClosestToActor;
    OverlappingComponent->GetClosestPointOnCollision(
        SpawnedActorLocation,
        CollisionPointClosestToActor);
    // Calculates distance from location of overlapping component
    // to the collision point on said component closest to spawned actor
    const float DistanceFromOverlappedMesh =
            FVector::Distance(CollisionPointClosestToActor,
                              LocationOfOverlappingComp) / 100.f;

    // If the distances are nearly equal, this means that spawned actor
    // is not penetrating the overlapping actor
    if (FMath::IsNearlyEqual(
        DistanceBetweenActorAndComp,
        DistanceFromOverlappedMesh))
    {
        // Checks for edge case when collision points are equidistant
        // from the actor location
        if (DistanceBetweenActorAndComp == 0)
        {
            return OverlappingComponent->Bounds.BoxExtent.Z > 0;
        }
        return false;
    }
    return true;
}

void AmbitSpawnerCollisionHelpers::FindDefaultStaticMeshComponents(UClass* Actor,
                                                                   TArray<UStaticMeshComponent*>& OutArray)
{
    OutArray.Empty();
    // Gets all C++ added default sub-object components of Actor class
    TArray<UObject*> AllComponents;
    TArray<UObject*> StaticMeshUObjects;
    Actor->GetDefaultObjectSubobjects(AllComponents);
    ContainsObjectOfClass(AllComponents,
                          UStaticMeshComponent::StaticClass(), false,
                          &StaticMeshUObjects);

    for (UObject* StaticMeshUObject : StaticMeshUObjects)
    {
        OutArray.Add(Cast<UStaticMeshComponent>(StaticMeshUObject));
    }

    if (Actor->IsInBlueprint())
    {
        // If actor is in blueprint, gets all (if any) static mesh components
        // and adds them to the actor class via blueprint
        UClass* StaticMeshComponent = UStaticMeshComponent::StaticClass();
        const UBlueprintGeneratedClass* ActorBlueprintGeneratedClass =
                Cast<UBlueprintGeneratedClass>(Actor);
        const TArray<USCS_Node*>& Nodes =
                ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes();
        for (const USCS_Node* Node : Nodes)
        {
            if (UClass::FindCommonBase(Node->ComponentClass,
                                       StaticMeshComponent) == StaticMeshComponent)
            {
                OutArray.Add(
                    Cast<UStaticMeshComponent>(
                        Node->ComponentTemplate));
            }
        }
    }
}

void AmbitSpawnerCollisionHelpers::SetCollisionForAllStaticMeshComponents(
    const TArray<UStaticMeshComponent*>& StaticMeshComponents,
    bool bRemoveOverlaps)
{
    for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
    {
        StaticMeshComponent->SetGenerateOverlapEvents(true);
        // Set appropriate AmbitSpawner Spawned Type
        if (!bRemoveOverlaps)
        {
            StaticMeshComponent->SetCollisionObjectType(AMBIT_SPAWNED_OVERLAP);
        }
        else
        {
            StaticMeshComponent->SetCollisionObjectType(AMBIT_SPAWNED_OBSTACLE);
        }

        StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        StaticMeshComponent->SetCollisionProfileName(AmbitSpawner::KAmbitCollisionProfileName);
        StaticMeshComponent->GetStaticMesh()->bCustomizedCollision = true;
        StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
        StaticMeshComponent->SetCollisionResponseToChannel(
            AMBIT_SPAWNED_OBSTACLE, ECR_Block);
        // Set Overlappable component response to Overlap
        StaticMeshComponent->SetCollisionResponseToChannel(
            AMBIT_SPAWNED_OVERLAP, ECR_Overlap);
    }
}

void AmbitSpawnerCollisionHelpers::SetGenerateOverlapEventsForActor(
    const AActor* Actor, TArray<bool>& Original, const bool bReset)
{
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
    if (!bReset)
    {
        // if function is not called to reset GenerateOverlapEvents,
        // make sure that the array passed in is empty
        Original.Empty();
    }
    else
    {
        if (StaticMeshComponents.Num() != Original.Num())
        {
            UE_LOG(LogAmbit, Warning,
                   TEXT(
                       "The amount of StaticMeshComponents found for Actor %s does not match the number of Original GenerateOverlapEvents settings stored."
                   ),
                   *Actor->GetName());
            return;
        }
    }
    for (int i = 0; i < StaticMeshComponents.Num(); i++)
    {
        UStaticMeshComponent* StaticMeshComponent = StaticMeshComponents[i];
        if (!bReset)
        {
            // if function is not called to reset GenerateOverlapEvents,
            // store original setting into given array
            Original.Add(StaticMeshComponent->GetGenerateOverlapEvents());
            StaticMeshComponent->SetGenerateOverlapEvents(true);
        }
        else
        {
            StaticMeshComponent->SetGenerateOverlapEvents(Original[i]);
        }
    }
}

void AmbitSpawnerCollisionHelpers::StoreCollisionProfiles(
    const FString& PathName,
    const TArray<UStaticMeshComponent*>& StaticMeshComponents,
    TMap<FString, TArray<FCollisionResponseTemplate>>& OutMap)
{
    // Store the original collision profiles of the CDO static mesh components
    TArray<FCollisionResponseTemplate> OriginalResponses;
    for (const UStaticMeshComponent* Mesh : StaticMeshComponents)
    {
        FCollisionResponseTemplate CollisionResponse;
        CollisionResponse.ResponseToChannels = Mesh->GetCollisionResponseToChannels();
        CollisionResponse.ObjectType = Mesh->GetCollisionObjectType();
        CollisionResponse.CollisionEnabled = Mesh->GetCollisionEnabled();
        CollisionResponse.Name = Mesh->GetCollisionProfileName();
        OriginalResponses.Add(CollisionResponse);
    }
    OutMap.Add(PathName, OriginalResponses);
}

void AmbitSpawnerCollisionHelpers::ResetCollisionProfiles(
    const TMap<FString, TArray<FCollisionResponseTemplate>>& OriginalCollisionProfiles,
    const TArray<TSubclassOf<AActor>>& ActorsToSpawnClean)
{
    for (const TSubclassOf<AActor>& Actor : ActorsToSpawnClean)
    {
        TArray<FCollisionResponseTemplate> Originals =
                OriginalCollisionProfiles.FindChecked(Actor->GetPathName());
        TArray<UStaticMeshComponent*> StaticMeshComponents;
        FindDefaultStaticMeshComponents(
            Actor.Get(), StaticMeshComponents);

        // Iterates through all collision profiles associated with
        // static mesh components of the Actor
        for (int32 i = 0; i < Originals.Num(); i++)
        {
            FCollisionResponseTemplate Profile = Originals[i];
            UStaticMeshComponent* Mesh = StaticMeshComponents[i];
            Mesh->SetCollisionResponseToChannels(Profile.ResponseToChannels);
            Mesh->SetCollisionEnabled(Profile.CollisionEnabled);
            Mesh->SetCollisionProfileName(Profile.Name);
            Mesh->SetCollisionObjectType(Profile.ObjectType);
        }
    }
}
