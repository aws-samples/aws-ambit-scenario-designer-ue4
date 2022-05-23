//   Copyright 2022 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
#include "WheeledVehicle.h"
#include "GameFramework/Actor.h"

#include "AmbitSpawner.h"

#include <Ambit/Actors/SpawnerConfigs/SpawnVehiclePathConfig.h>

#include "SpawnVehiclePath.generated.h"

/**
 * This class provides Waypoints generation along a spline
 * as well as spawns a vehicle at the starting point
 */
UCLASS()
class AMBIT_API ASpawnVehiclePath : public AActor, public IAmbitSpawner
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASpawnVehiclePath();

    class UBillboardComponent* IconComponent;

    /** The speed limit of the vehicle running along the path in km/h */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambit Vehicle Path Generator",
        meta = (DisplayName = "Speed Limit", ClampMin = "0.1", UIMin = "0.1"))
    float SpeedLimit = 20.f;

    /** The fixed distance between each generated Waypoint in m */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambit Vehicle Path Generator",
        meta = (DisplayName = "Distance Between Waypoints", ClampMin = "0.1", UIMin = "0.1"))
    float DistanceBetweenWaypoints = 5.f;

    /** Whether to render Waypoint out for debugging purpose at runtime*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambit Vehicle Path Generator",
        meta = (DisplayName = "Show Debug Waypoints"))
    bool ShowDebugWaypoint = false;

    /** The vehicle to spawn at the beginning of the path */
    UPROPERTY(EditAnywhere, Category = "Ambit Vehicle Path Generator")
    TSubclassOf<class AWheeledVehicle> VehicleToSpawn;

    UPROPERTY(VisibleAnywhere, Category = "Ambit Vehicle Path Generator")
    class USplineComponent* Spline;

    /**
     * Emits the USpawnedVehiclePathConfig object describing the asset path of
     * the spawned vehicle and path info the vehicle is running along with to
     * FOnSpawnedObjectsConfigCompleted.
     */
    void GenerateSpawnedObjectConfiguration() override;

    /**
     * @inheritDoc
     * Calls GenerateSpawnedObjectConfiguration() as if there were no seed.
     */
    void GenerateSpawnedObjectConfiguration(int Seed) override
    {
        GenerateSpawnedObjectConfiguration();
    }

    /**
     * @inheritDoc
     */
    TSharedPtr<FSpawnVehiclePathConfig> GetConfiguration() const;

    /**
     * @inheritDoc
     */
    void Configure(const TSharedPtr<FSpawnVehiclePathConfig>& Config);

    /**
     * @inheritDoc
     * Returns true when there are valid path objects that will be created.
     */
    bool HasActorsToSpawn() const override;

private:
    // Determines whether the required user parameters have been set.
    bool AreParametersValid() const;

    // Spawns actors as specified by this object's parameters.
    void SpawnVehicle() const;

    // Called when the game starts or when spawned
    void BeginPlay() override;
};
