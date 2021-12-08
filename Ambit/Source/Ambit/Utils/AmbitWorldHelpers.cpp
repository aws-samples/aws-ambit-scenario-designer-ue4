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

#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "Ambit/AmbitModule.h"

FHitResult AmbitWorldHelpers::LineTraceBelowWorldPoint(
    const FVector& Location, const float MaxDistance)
{
    UWorld* World = GEngine->GetWorldContexts()[0].World();
    FHitResult Hit;
    FVector LineStart = Location;
    FVector LineEnd = LineStart + FVector(0, 0, -MaxDistance);
    FCollisionQueryParams QueryParams{FName("Visibility")};
    FCollisionResponseParams ResponseParams{};
    World->LineTraceSingleByChannel(Hit, LineStart, LineEnd,
                                    ECC_Visibility, QueryParams,
                                    ResponseParams);
    return Hit;
}

TArray<AActor*> AmbitWorldHelpers::GetActorsByMatchBy(
    const EMatchBy& MatchBy, const FString& NamePattern,
    const TArray<FName>& TagsList, const bool bMatchExactName)
{
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(
        GEngine->GetWorldContexts()[0].World(), AActor::StaticClass(),
        AllActors);

    // Filter actors to just those matching criteria.
    bool bNamePatternIsEmpty = NamePattern.IsEmpty();
    bool bTagsListIsEmpty = TagsList.Num() == 0;

    return AllActors.FilterByPredicate(
        [MatchBy, NamePattern, TagsList, bNamePatternIsEmpty, bTagsListIsEmpty, bMatchExactName](
    const AActor* Actor)
        {
            // if name pattern is empty, default to false. Else match to the name pattern.
            bool bMatchesName = false;

            if (!bNamePatternIsEmpty
                && (bMatchExactName && Actor->GetName() == NamePattern
                    || Actor->GetName().Contains(NamePattern)))
            {
                bMatchesName = true;
            }

            // if the tags list is empty, default to false. Else match to tags.
            bool bMatchesTags = !bTagsListIsEmpty;
            for (FName Tag : TagsList)
            {
                if (!Actor->Tags.Contains(Tag))
                {
                    bMatchesTags = false;
                    break;
                }
            }

            switch (MatchBy)
            {
                case NameAndTags:
                    return bMatchesName && bMatchesTags;

                case NameOrTags:
                    return bMatchesName || bMatchesTags;

                default:
                    return false;
            }
        });
}

TArray<FTransform> AmbitWorldHelpers::GenerateRandomLocationsFromActors(
    const TArray<AActor*>& ActorsToSearch, int32 RandomSeed, float DensityMin,
    float DensityMax, float RotationMin, float RotationMax)
{
    TArray<FTransform> Transforms;
    if (DensityMin > DensityMax)
    {
        UE_LOG(LogAmbit, Warning, TEXT("DensityMin is greater than DensityMax. No actors spawned."));
        return Transforms;
    }

    if (RotationMin > RotationMax)
    {
        UE_LOG(LogAmbit, Warning, TEXT("RotationMin is greater than RotationMax. No actors spawned."));
        return Transforms;
    }

    FRandomStream Random;
    Random.Initialize(RandomSeed);

    for (AActor* SurfaceActor : ActorsToSearch)
    {
        // Calculate the surface area of this actor to determine how many items to spawn.
        FBox Bounds = SurfaceActor->GetComponentsBoundingBox();
        const FVector SizeMeters = Bounds.GetSize() / 100.f;
        const float AreaMeters = SizeMeters.X * SizeMeters.Y;
        int SpawnCount = AreaMeters * Random.FRandRange(DensityMin, DensityMax);

        FVector Location(0, 0, Bounds.Max.Z + 1);
        FRotator Rotation(0);

        // Spawn items at random positions within the overall bounds of the target surface,
        // being sure to maintain the user-specified target density.
        // SpawnCount is initially set to the calculated maximum number of items that
        // may need to be spawned to achieve the desired density,
        // but in cases where surfaces don't completely fill their bounding boxes (which is very common).
        // the actually number of spawned items will be smaller in order to achieve the desired density
        // relative to the actual surface area available.
        while (SpawnCount > 0)
        {
            // Choose a random point just above the surface of SurfaceActor's bounding box.
            Location.X = Random.FRandRange(Bounds.Min.X, Bounds.Max.X);
            Location.Y = Random.FRandRange(Bounds.Min.Y, Bounds.Max.Y);
            const float Yaw = Random.FRandRange(RotationMin, RotationMax);

            const FHitResult& Hit = LineTraceBelowWorldPoint(Location);

            if (Hit.IsValidBlockingHit() && Hit.GetActor() == SurfaceActor)
            {
                // Calculate rotation axis using normal of impact point
                FVector RotationAxis = FVector::CrossProduct(
                    FVector(0, 0, 1), Hit.ImpactNormal);
                RotationAxis.Normalize();

                const float RotationAngle = acosf(
                    FVector::DotProduct(FVector(0, 0, 1), Hit.ImpactNormal));
                const FQuat& Quat = FQuat(RotationAxis, RotationAngle);
                // Adjust Rotation of new transform (0,0,0) with the new rotation axis
                FQuat AdjustedRotation = Rotation.Quaternion() * Quat;

                // Combine "local" Yaw rotation generated from user inputted restrictions
                // with the adjusted rotation axis
                FTransform Transform(AdjustedRotation *
                                     FRotator(0, Yaw, 0).Quaternion(),
                                     Hit.ImpactPoint);
                Transforms.Push(Transform);
            }

            // No matter hit or not, reduce spawn count
            SpawnCount--;
        }
    }
    return Transforms;
}

TArray<FTransform> AmbitWorldHelpers::GenerateRandomLocationsFromBox(
    UBoxComponent* Box, const TArray<AActor*>& ActorsToHit, int32 RandomSeed,
    bool bSnapToSurfaceBelow, float DensityMin, float DensityMax,
    float RotationMin, float RotationMax)
{
    TArray<FTransform> Transforms;
    if (!IsValid(Box))
    {
        UE_LOG(LogAmbit, Warning, TEXT("Invalid box component."));
        return Transforms;
    }
    if (DensityMin > DensityMax)
    {
        UE_LOG(LogAmbit, Warning, TEXT("DensityMin is greater than DensityMax. No actors spawned."));
        return Transforms;
    }

    if (RotationMin > RotationMax)
    {
        UE_LOG(LogAmbit, Warning, TEXT("RotationMin is greater than RotationMax. No actors spawned."));
        return Transforms;
    }
    FRandomStream Random;
    Random.Initialize(RandomSeed);

    FTransform BoxTransform = Box->GetComponentTransform();

    FBox Bounds = Box->Bounds.GetBox();

    float BoxYaw = BoxTransform.Rotator().Yaw;
    float BoxYawMod = UKismetMathLibrary::GenericPercent_FloatFloat(BoxYaw, 90.f);
    if (!FMath::IsNearlyEqual(BoxYawMod, 0.f))
    {
        // If BoxComponent is a rotated box, create a rectangle
        // with the same area as a temporary bounding box
        Bounds = FBox(FVector(-Box->GetScaledBoxExtent().X,
                              -Box->GetScaledBoxExtent().Y, 0.0),
                      FVector(Box->GetScaledBoxExtent().X,
                              Box->GetScaledBoxExtent().Y, 0.0));
    }

    // Calculate the surface area of the bounding box to determine how many items to spawn.
    const FVector SizeMeters = Bounds.GetSize() / 100.f;
    const float AreaMeters = SizeMeters.X * SizeMeters.Y;
    int SpawnCount = AreaMeters * Random.FRandRange(DensityMin, DensityMax);

    FVector Location(0, 0, Bounds.Max.Z);

    while (SpawnCount > 0)
    {
        Location.X = Random.FRandRange(Bounds.Min.X, Bounds.Max.X);
        Location.Y = Random.FRandRange(Bounds.Min.Y, Bounds.Max.Y);

        FVector NewLocation = Location;
        if (!FMath::IsNearlyEqual(BoxYawMod, 0.f))
        {
            // If BoxComponent is a rotated box, iterate through transforms
            // and transform each location to match the rotated box area
            NewLocation = BoxTransform.TransformPosition(Location);
        }

        FTransform Transform(FRotator(0), NewLocation);
        if (CheckAndSnapToSurface(Transform, ActorsToHit, bSnapToSurfaceBelow))
        {
            FRotator Rotation(0,
                              Random.FRandRange(RotationMin, RotationMax), 0);
            // Combine "local" Yaw rotation generated from user inputted restrictions
            // with the adjusted rotation axis
            Transform.SetRotation(
                Transform.GetRotation() * Rotation.Quaternion());
            Transforms.Push(Transform);
        }

        // No matter hit or not, reduce spawn count
        SpawnCount--;
    }

    return Transforms;
}

TArray<FTransform> AmbitWorldHelpers::GenerateRandomLocationsFromSpline(
    USplineComponent* Spline, const TArray<AActor*>& ActorsToHit,
    int32 RandomSeed, bool bSnapToSurfaceBelow, float DensityMin, float DensityMax,
    float RotationMin, float RotationMax, bool bFollowSplineRotation)
{
    TArray<FTransform> Transforms;
    if (!IsValid(Spline))
    {
        UE_LOG(LogAmbit, Warning, TEXT("Invalid spline component."));
        return Transforms;
    }
    if (DensityMin > DensityMax)
    {
        UE_LOG(LogAmbit, Warning, TEXT("DensityMin is greater than DensityMax. No actors spawned."));
        return Transforms;
    }

    if (RotationMin > RotationMax)
    {
        UE_LOG(LogAmbit, Warning, TEXT("RotationMin is greater than RotationMax. No actors spawned."));
        return Transforms;
    }
    FRandomStream Random;
    Random.Initialize(RandomSeed);

    const float SplineLength = Spline->GetSplineLength();

    // Calculate length of spline in meters to determine how many items to spawn.
    int SpawnCount = SplineLength / 100.f * Random.FRandRange(DensityMin, DensityMax);

    while (SpawnCount > 0)
    {
        const float Distance = Random.FRandRange(0, SplineLength);
        FVector Location = Spline->GetLocationAtDistanceAlongSpline(
            Distance, ESplineCoordinateSpace::World);

        FTransform Transform(FRotator(0), Location);
        if (CheckAndSnapToSurface(Transform, ActorsToHit, bSnapToSurfaceBelow))
        {
            FRotator Rotation(0, Random.FRandRange(RotationMin, RotationMax), 0);
            if (bFollowSplineRotation)
            {
                const float SplinePointRotation = Spline->GetRotationAtDistanceAlongSpline(
                    Distance, ESplineCoordinateSpace::World).Yaw;
                Rotation.Yaw += SplinePointRotation;
            }
            // Combine "local" Yaw rotation generated from user inputted restrictions
            // with the adjusted rotation axis
            Transform.SetRotation(Transform.GetRotation() * Rotation.Quaternion());
            Transforms.Push(Transform);
        }

        // No matter hit or not, reduce spawn count
        SpawnCount--;
    }
    return Transforms;
}

bool AmbitWorldHelpers::CheckAndSnapToSurface(FTransform& Transform,
                                              const TArray<AActor*>& ActorsToHit,
                                              const bool& bSnapToSurfaceBelow)
{
    if (!bSnapToSurfaceBelow && ActorsToHit.Num() == 0)
    {
        return true;
    }

    FVector Location = Transform.GetLocation();
    const FHitResult& Hit = LineTraceBelowWorldPoint(Location);
    AActor* ActorHit = Hit.GetActor();

    if (Hit.IsValidBlockingHit())
    {
        Location.Z = Hit.ImpactPoint.Z;
        Transform.SetLocation(Location);

        // Calculate rotation axis using impact normal
        FVector RotationAxis = FVector::CrossProduct(
            FVector(0, 0, 1), Hit.ImpactNormal);
        RotationAxis.Normalize();

        const float RotationAngle = acosf(
            FVector::DotProduct(FVector(0, 0, 1), Hit.ImpactNormal));
        const FQuat& Quat = FQuat(RotationAxis, RotationAngle);

        Transform.SetRotation(Transform.GetRotation() * Quat);

        if (ActorsToHit.Num() == 0)
        {
            return true;
        }

        for (AActor* ActorToHit : ActorsToHit)
        {
            if (ActorHit == ActorToHit)
            {
                return true;
            }
        }
    }
    return false;
}

TArray<FTransform> AmbitWorldHelpers::GenerateFixedLocationsFromSpline(
    USplineComponent* Spline, float Distance)
{
    TArray<FTransform> Transforms;
    if (!IsValid(Spline))
    {
        UE_LOG(LogAmbit, Warning, TEXT("Invalid spline component."));
        return Transforms;
    }

    const float SplineLength = Spline->GetSplineLength();

    if (Distance > SplineLength)
    {
        UE_LOG(LogAmbit, Warning, TEXT("Distance must be smaller than the total length of Spline."));
        return Transforms;
    }

    for (float i = 0.f; i <= SplineLength; i += Distance)
    {
        FRotator Rotation(0, 0, 0);
        Rotation.Yaw = Spline->GetRotationAtDistanceAlongSpline(i,
                                                                ESplineCoordinateSpace::World).Yaw;

        FVector Location = Spline->GetLocationAtDistanceAlongSpline(i,
                                                                    ESplineCoordinateSpace::World);

        FTransform Transform(Rotation, Location);
        Transforms.Push(Transform);
    }

    return Transforms;
}
