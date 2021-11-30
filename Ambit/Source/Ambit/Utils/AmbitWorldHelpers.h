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
#include "MatchBy.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/CollisionProfile.h"

/**
 * A collection of helpers functions that make using UWorld easier.
 */
namespace AmbitWorldHelpers
{
    /**
     * Performs a line trace from the specified location downward
     * and returns a hit result.
     *
     * @param Location
     *  the location to start the line trace
     * @param MaxDistance
     *  the maximum length of the line trace
     */
    FHitResult LineTraceBelowWorldPoint(const FVector& Location,
                                        const float MaxDistance = 100000);

    /**
     * Returns the list of actors that match by Name and/or a list of tags
     *
     * @param MatchBy
     *  The rule to match by AND/OR
     * @param NamePattern
     *  The name pattern to match on, if any (empty string)
     * @param TagsList
     *  The list of tags to match on, if any (empty list)
     * @param bMatchExactName
     *  Indicates if the NamePattern has to match exactly to the found actor.
     * @return
     *  An array of all actors that match the rules
     */
    TArray<AActor*> GetActorsByMatchBy(const EMatchBy& MatchBy,
                                       const FString& NamePattern,
                                       const TArray<FName>& TagsList,
                                        const bool bMatchExactName = false);

    /**
     * Returns a list of locations in the provided actors.
     *
     * @param ActorsToSearch
     *  Actors to use to generate the random locations
     * @param RandomSeed
     *  Locations are deterministic based on RandomSeed.
     * @param DensityMin
     *  the minimum density for the locations. Defaults to 0.
     * @param DensityMax
     *  the maximum density for the locations. Defaults to 0.2.
     * @param RotationMin
     *  the minimum value for the rotations. Defaults to 0.
     * @param RotationMax
     *  the maximum value for the rotations. Defaults to 360.
     * @return
     *  An array of locations within the ActorsToSearch list
     */
    TArray<FTransform> GenerateRandomLocationsFromActors(
        const TArray<AActor*>& ActorsToSearch, int32 RandomSeed,
        float DensityMin = 0.0, float DensityMax = 0.2,
        float RotationMin = 0.0, float RotationMax = 360.0);


    /**
     * Conducts a downward hit check to snap locations in Transform
     * to valid surfaces below and adjusts location if needed
     *
     * @param Transform
     * FTransform to adjust
     * @param ActorsToHit
     *  Actors to use as valid surfaces to hit
     * @param bSnapToSurfaceBelow
     *  Determines if the hit check should be conducted when ActorsToHit is empty
     * @return
     *  A boolean value that indicates if location hits a valid surface
     */
    bool CheckAndSnapToSurface(FTransform& Transform, const TArray<AActor*>& ActorsToHit,
        const bool& bSnapToSurfaceBelow);
    /**
     * Returns a list of locations in the provided spline
     * and actors.
     *
     * @param Spline
     *  Spline component to use to generate the random locations
     * @param ActorsToHit
     *  Actors to use in hit check for generated locations
     * @param RandomSeed
     *  Locations are deterministic based on RandomSeed.
     * @param bSnapToSurfaceBelow
     *  Determines if the hit check should be conducted when ActorsToHit is empty
     * @param DensityMin
     *  the minimum density for the locations. Defaults to 0.
     * @param DensityMax
     *  the maximum density for the locations. Defaults to 0.2.
     * @param bFollowSplineRotation
     *  Determines whether to use the rotation of Spline for the locations
     * @param RotationMin
     *  the minimum value for the rotations. Defaults to 0.
     * @param RotationMax
     *  the maximum value for the rotations. Defaults to 360.  
     * @return
     *  An array of locations within Spline and the ActorsToHit list (if any)
     */
    TArray<FTransform> GenerateRandomLocationsFromSpline(
        USplineComponent* Spline, const TArray<AActor*>& ActorsToHit,
        int32 RandomSeed, bool bSnapToSurfaceBelow, float DensityMin = 0.0, float DensityMax = 0.2,
        float RotationMin = 0.0, float RotationMax = 360.0, bool bFollowSplineRotation = false);


    /**
     * Returns a list of locations in the provided box component
     * and actors.
     *
     * @param Box
     *  Box component to use to generate the random locations
     * @param ActorsToHit
     *  Actors to use in hit check for generated locations
     * @param RandomSeed
     *  Locations are deterministic based on RandomSeed.
     * @param bSnapToSurfaceBelow
     *  Determines if the hit check should be conducted when ActorsToHit is empty
     * @param DensityMin
     *  the minimum density for the locations. Defaults to 0.
     * @param DensityMax
     *  the maximum density for the locations. Defaults to 0.2.
     * @param RotationMin
     *  the minimum value for the rotations. Defaults to 0.
     * @param RotationMax
     *  the maximum value for the rotations. Defaults to 360.
     * @return
     *  An array of locations within Spline and the ActorsToHit list (if any)
     */
    TArray<FTransform> GenerateRandomLocationsFromBox(
        UBoxComponent* Box, const TArray<AActor*>& ActorsToHit, int32 RandomSeed,
        bool bSnapToSurfaceBelow, float DensityMin = 0.0, float DensityMax = 0.2,
        float RotationMin = 0.0, float RotationMax = 360.0);

    /**
     * Returns a list of locations separate by fix distance in the provided spline
     *
     * @param Spline
     *  Spline component to use to generate the locations
     * @param Distance
     *  the fixed distance between each generated locations in cm
     * @return
     *  An array of locations within Spline
     */
    TArray<FTransform> GenerateFixedLocationsFromSpline(
        USplineComponent* Spline, float Distance = 1000.0);
};
