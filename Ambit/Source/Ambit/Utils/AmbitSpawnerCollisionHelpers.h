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

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"

namespace AmbitSpawnerCollisionHelpers
{

    /**
     * Sets bGenerateOverlapEvents to true
     * for the provided actor, unless called
     * to reset the provided actor to its original settings
     *
     * @param Actor
     *  pointer to actor to turn on generate overlap events
     * @param Original
     *  array to store original generate overlap event settings
     * @param bReset
     *  whether the function is called to reset values to original settings
     */
    void SetGenerateOverlapEventsForActor(
        const AActor* Actor, TArray<bool>& Original, const bool bReset = false);

    /**
     * Populates the provided array with all of the
     * default static mesh components of the provided actor class.
     *
     * @param Actor
     *  the UClass of the Actor from which to get all default UStaticMeshComponents
     * @param OutArray
     *  the array to populate with all default static mesh components
     */
    void FindDefaultStaticMeshComponents(UClass* Actor,
        TArray<UStaticMeshComponent*>& OutArray);

    /**
     * Sets collision profiles of all static mesh components
     * in the provided array to the custom profile for ambit spawned obstacles
     *
     * @param StaticMeshComponents
     *  the array of static mesh components to set collision profiles
     * @param bRemoveOverlaps
     *  whether collision profiles should block other spawned obstacles
     */
    void SetCollisionForAllStaticMeshComponents(
        const TArray<UStaticMeshComponent*>& StaticMeshComponents,
        bool bRemoveOverlaps = true);

    /**
     * Checks if the actor at provided ActorLocation is
     * penetrating the provided overlapping component;
     * function assumes that the actor and the component
     * are in fact overlapping, even if slightly
     *
     * @param OverlappingComponent
     *  the component that is overlapping the spawned obstacle located
     *  at SpawnedActorLocation
     * @param SpawnedActor
     *  the spawned obstacle with overlaps
     */
    bool IsPenetratingOverlap(
        UPrimitiveComponent* OverlappingComponent,
        AActor* SpawnedActor);

    /**
     * Stores collision profiles of the provided StaticMeshComponents
     * in the OutMap in an array whose key value is PathName
     *
     * @param PathName
     *  Path Name of the ActorToSpawn that the StaticMeshComponents belong to
     *  Used as the key for the OutMap
     * @param StaticMeshComponents
     *  An array of Static Mesh Components from which to get the collision profiles 
     *  that will be stored in OutMap
     * @param OutMap
     *  A Map where the collision profiles for these static mesh components will
     *  be stored using the PathName as the key
     */
    void StoreCollisionProfiles(
        const FString& PathName,
        const TArray<UStaticMeshComponent*>& StaticMeshComponents,
        TMap<FString, TArray<FCollisionResponseTemplate>>& OutMap);

    /**
     * Restores collision profiles of ActorsToSpawn asset CDOs
     * back to their original
     *
     * @param OriginalCollisionProfiles
     *  Map containing the original collision profiles of the static mesh components
     *  of the ActorsToSpawn
     * @param ActorsToSpawnClean
     *  Array of ActorsToSpawn without any duplicates
     */
    void ResetCollisionProfiles(
        const TMap<FString, TArray<FCollisionResponseTemplate>>& OriginalCollisionProfiles,
        const TArray<TSubclassOf<AActor>>& ActorsToSpawnClean);
}
