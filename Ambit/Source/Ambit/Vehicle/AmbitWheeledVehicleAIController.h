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

#include "WheeledVehicle.h"
#include "GameFramework/Controller.h"

#include "AmbitVehiclePIDController.h"
#include "AmbitWheeledVehicleAIController.generated.h"

/**
 * Ambit Wheeled Vehicle AI Controller utilizing PID Controller
 */
UCLASS()
class AMBIT_API AAmbitWheeledVehicleAIController : public AController
{
    GENERATED_BODY()

public:
    AAmbitWheeledVehicleAIController(const FObjectInitializer& ObjectInitializer);

    /**
     * https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/GameFramework/AController/OnPossess
     */
    void OnPossess(APawn* APawn) override;

    /**
     * https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/GameFramework/AController/OnUnPossess
     */
    void OnUnPossess() override;

    /**
     * https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/GameFramework/AActor/Tick/
     */
    void Tick(float DeltaTime) override;

    /**
     * Set Waypoints as vehicle route from an Array of Transform. Waypoint will just be Vector for now.
     *
     *@param Locations an array of waypoint locations to pass in
     */
    void SetVehicleRoute(const TArray<FTransform>& Locations);

    /**
     * Set Speedlimit for the vehicle. This will make vehicle try to reach this speed along the path.
     * @param Speed the speed limit set for the vehicle
     */
    void SetSpeedLimit(const float Speed);

    /**
     * Set whether the vehicle path is looped or not.
     * @param Looped indicate whether the path is close looped or not
     */
    void SetLoopedPath(bool Looped);

private:
    //Longitudinal and Lateral PID Controllers
    FAmbitVehicleLongitudinalController ThrottleController;
    FAmbitVehicleLateralController SteeringController;

    //AWheeledVehicle possessed by the controller
    UPROPERTY()
    AWheeledVehicle* Vehicle = nullptr;

    float SpeedLimit = 0.f;

    //Store the steering input from last frame to determine the next one
    float PastSteering = 0.f;
    //Store the last waypoint vehicle just passed
    FVector PastWaypoint;

    //Minimum distance to check against to determine whether a waypoint is reached
    float WaypointDistanceThreshold = 0.f;

    //Indicate whether the waypoints are looped path
    bool IsLooped = false;

    //Storing an array of waypoints defined by the spline
    TArray<FVector> Waypoints;
    //Storing an array of upcoming waypoints the vehicle is about to navigate through
    TArray<FVector> WaypointsBuffer;
};
