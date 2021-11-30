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
#include "Engine/CollisionProfile.h"
#include "GameFramework/Actor.h"

#include "AmbitSpawner.h"
#include "Ambit/Utils/MatchBy.h"

#include "SpawnerBase.generated.h"

struct FSpawnerBaseConfig;
class USpawnedObjectConfig;

/**
 * This abstract class is a base class for Ambit Spawners that generate
 * BP actor obstacles to screen (e.g.SpawnOnSpline)
 */
UCLASS(Abstract)
class AMBIT_API ASpawnerBase : public AActor, public IAmbitSpawner
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASpawnerBase();

    class UBillboardComponent* IconComponent;
    /**
     * Select to Match By AND/OR
     */
    UPROPERTY(EditAnywhere, Category = "Ambit Spawner")
    TEnumAsByte<EMatchBy> MatchBy = EMatchBy::NameAndTags;

    /**
     * The search string used to find actors representing surfaces to spawn onto.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambit Spawner",
        meta = (DisplayName = "Surface Name Pattern"))
    FString SurfaceNamePattern = "";

    /**
     * The tag name used to find actors representing surfaces to spawn onto.
     */
    UPROPERTY(EditAnywhere, Category = "Ambit Spawner",
        meta = (DisplayName = "Surface Tags"))
    TArray<FName> SurfaceTags;

    /**
     * The minimum number of items to spawn per square meter on average.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =
        "Ambit Spawner",
        meta = (DisplayName = "Items Per Meter (Min)", ClampMin = "0.0", UIMin =
            "0.0"))
    float DensityMin = 0.05f;

    /**
     * The maximum number of items to spawn per square meter on average.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =
        "Ambit Spawner",
        meta = (DisplayName = "Items Per Meter (Max)", ClampMin = "0.0", UIMin =
            "0.0"))
    float DensityMax = 0.2f;

    /**
     * Automatically sets maximum rotation equal to the minimum.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =
        "Ambit Spawner")
    bool bRestrictToOneRotation = false;

    /**
     * The minimum degree of rotation.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =
        "Ambit Spawner",
        meta = (DisplayName = "Degree of Rotation (Min)", ClampMin = "-360.0",
            ClampMax = "360.0", UIMin = "-360.0", UIMax = "360.0"))
    float RotationMin = 0.0f;

    /**
     * The maximum degree of rotation.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambit Spawner",
        meta = (EditCondition = "!bRestrictToOneRotation", DisplayName =
            "Degree of Rotation (Max)",
            ClampMin = "-360.0", ClampMax = "360.0", UIMin = "-360.0", UIMax =
            "360.0"))
    float RotationMax = 360.0;

    /**
     * Whether to add physics simulation to obstacles.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambit Spawner")
    bool bAddPhysics = false;

    /**
     * The actors that will be spawned randomly.
     */
    UPROPERTY(EditAnywhere, Category = "Ambit Spawner")
    TArray<TSubclassOf<class AActor>> ActorsToSpawn;

    /**
     * Whether or not to remove spawned obstacles
     * that overlap with other obstacles.
     */
    UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, 
        Category = "Ambit Spawner")
    bool bRemoveOverlaps = true;

    /**
     * Change this value to generate different random arrangements.
     */
    UPROPERTY(EditAnywhere, AdvancedDisplay, Category =
        "Ambit Spawner")
    int32 RandomSeed = 0;

    /**
     * @inheritDoc
     */
    bool HasActorsToSpawn() const override;

    /**
     * Returns a configuration object populated
     * with the property values of this instance.
     */
    template <typename Struct>
    TSharedPtr<Struct> GetConfiguration() const;

    /**
     * Updates the property values of this instance by applying the provided configuration
     * object.
     */
    template <typename Struct>
    void Configure(const TSharedPtr<Struct>& Config);

    /**
     * See https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/PostEditChangeProperty/
     */
    virtual void PostEditChangeProperty(
        FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * @inheritDoc
     * Emits the USpawnedObjectConfig object describing the placement of spawned objects
     * to FOnSpawnedObjectsConfigCompleted.
     * It uses this instance's RandomSeed value to generate the positions of the objects.
     */
    void GenerateSpawnedObjectConfiguration() override;

    /**
     * @inheritDoc
     * Emits the USpawnedObjectConfig object describing the placement of spawned objects
     * to FOnSpawnedObjectsConfigCompleted.
     * It uses the provided random seed value to generate the positions of the objects.
     */
    void GenerateSpawnedObjectConfiguration(int32 Seed) override;


protected:
    TArray<AActor*> SpawnedActors;

    // Used to generate repeatable random numbers.
    FRandomStream Random;

    // Called when the game starts or when spawned
    void BeginPlay() override;

    // Called when an actor is done spawning into the world
    void PostActorCreated() override;

    // Creates no-duplicate version of ActorsToSpawn array and adjusts
    // collision profiles to match ambit spawned obstacles profile
    void CleanAndSetUpActorsToSpawn(
        TArray<TSubclassOf<AActor>>& OutArray,
        TMap<FString, TArray<FCollisionResponseTemplate>>& OutMap);

    virtual TMap<FString, TArray<FTransform>> GenerateActors()
        PURE_VIRTUAL(AmbitSpawnerParent::GenerateActors,
            return TMap<FString, TArray<FTransform>>(););

    // Spawns actors using locations and rotations in provided array
    void SpawnActorsAtTransforms(
        const TArray<FTransform>& Transforms,
        TMap<FString, TArray<FTransform>>& OutMap);
    
    // Destroys any actors that were previously generated.
    void DestroyGeneratedActors();

    // Determines whether the required user parameters have been set.
    bool AreParametersValid() const;

    // Warns the user if the values they've entered pose a potential performance problem
    void PostEditErrorFixes();

    // Determines if the min/max fields are valid
    bool AreMinMaxValid() const;
};
