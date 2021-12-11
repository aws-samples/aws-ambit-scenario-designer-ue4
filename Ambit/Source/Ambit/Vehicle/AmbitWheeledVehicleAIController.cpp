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

#include "AmbitWheeledVehicleAIController.h"

#include "Constant.h"
#include "EngineUtils.h"
#include "WheeledVehicleMovementComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "GameFramework/Pawn.h"

#include "AmbitVehicleHelpers.h"

AAmbitWheeledVehicleAIController::AAmbitWheeledVehicleAIController(const FObjectInitializer& ObjectInitializer) : Super(
    ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    ThrottleController = FAmbitVehicleLongitudinalController(PIDController::LongitudinalProportionalTerm,
                                                             PIDController::LongitudinalDifferentialTerm,
                                                             PIDController::LongitudinalIntegralTerm);
    SteeringController = FAmbitVehicleLateralController(PIDController::LateralProportionalTerm,
                                                        PIDController::LateralDifferentialTerm,
                                                        PIDController::LateralIntegralTerm);
}

void AAmbitWheeledVehicleAIController::OnPossess(APawn* APawn)
{
    Super::OnPossess(APawn);

    Vehicle = Cast<AWheeledVehicle>(APawn);

    PastWaypoint = Vehicle->GetActorLocation();
}

void AAmbitWheeledVehicleAIController::OnUnPossess()
{
    Super::OnUnPossess();

    Vehicle = nullptr;
}

void AAmbitWheeledVehicleAIController::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(Vehicle))
    {
        return;
    }

    UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(
        Vehicle->GetVehicleMovement());

    if (WaypointsBuffer.Num() == 0)
    {
        Vehicle4W->SetSteeringInput(0);
        Vehicle4W->SetBrakeInput(1);
        Vehicle4W->SetThrottleInput(0);
        return;
    }

    if (WaypointsBuffer.Num() < 3 && IsLooped)
    {
        for (const FVector& Waypoint : Waypoints)
        {
            WaypointsBuffer.Emplace(Waypoint);
        }
    }

    const FVector CurrentLocation = Vehicle->GetActorLocation();
    const float CurrentSpeed = Vehicle4W->GetForwardSpeed();

    float TargetSpeed = SpeedLimit;

    if (WaypointsBuffer.Num() >= 3)
    {
        float Radius = AmbitVehicleHelpers::GetThreePointCircleRadius(PastWaypoint, WaypointsBuffer[0],
                                                                      WaypointsBuffer[1]);

        TargetSpeed = TargetSpeed > Radius * VehicleControl::TurningRadiusSpeedFactor
                          ? Radius * VehicleControl::TurningRadiusSpeedFactor
                          : TargetSpeed;

        for (int i = 0; i < WaypointsBuffer.Num() - 2; i++)
        {
            if (FVector::Dist(FVector(CurrentLocation.X, CurrentLocation.Y, 0),
                              FVector(WaypointsBuffer[i].X, WaypointsBuffer[i].Y, 0)) < CurrentSpeed *
                VehicleControl::LookingAheadDistanceFactor)
            {
                Radius = AmbitVehicleHelpers::GetThreePointCircleRadius(WaypointsBuffer[i], WaypointsBuffer[i + 1],
                                                                        WaypointsBuffer[i + 2]);
                TargetSpeed = TargetSpeed > Radius * VehicleControl::TurningRadiusSpeedFactor
                                  ? Radius * VehicleControl::TurningRadiusSpeedFactor
                                  : TargetSpeed;
            }
            else
            {
                break;
            }
        }
    }

    const float Acceleration = ThrottleController.RunStep(TargetSpeed, CurrentSpeed, DeltaTime);

    if (Acceleration > 0)
    {
        Vehicle4W->SetBrakeInput(0);
        Vehicle4W->SetThrottleInput(Acceleration);
    }
    else
    {
        Vehicle4W->SetBrakeInput(-Acceleration);
        Vehicle4W->SetThrottleInput(0);
    }

    const FVector TargetLocation = WaypointsBuffer[0];
    const FVector Forward = Vehicle->GetActorForwardVector();

    //Not change steering input significantly
    float Steering = SteeringController.RunStep(TargetLocation, CurrentLocation, Forward, DeltaTime);
    if (Steering > PastSteering + VehicleControl::SteeringDelta)
    {
        Steering = PastSteering + VehicleControl::SteeringDelta;
    }
    else if (Steering < PastSteering - VehicleControl::SteeringDelta)
    {
        Steering = PastSteering - VehicleControl::SteeringDelta;
    }
    Vehicle4W->SetSteeringInput(Steering);
    PastSteering = Steering;

    //Purge all past Waypoints
    float MaxIndex = -1;

    for (int i = 0; i < WaypointsBuffer.Num(); i++)
    {
        if (FVector::Dist(FVector(CurrentLocation.X, CurrentLocation.Y, 0),
                          FVector(WaypointsBuffer[i].X, WaypointsBuffer[i].Y, 0)) < WaypointDistanceThreshold)
        {
            MaxIndex = i;
        }
        else
        {
            break;
        }
    }
    if (MaxIndex >= 0)
    {
        PastWaypoint = WaypointsBuffer[MaxIndex];
        for (int i = 0; i < MaxIndex + 1; i++)
        {
            WaypointsBuffer.RemoveAt(0);
        }
    }
}

void AAmbitWheeledVehicleAIController::SetVehicleRoute(const TArray<FTransform>& Locations)
{
    for (const FTransform& Transform : Locations)
    {
        Waypoints.Emplace(Transform.GetLocation());
        WaypointsBuffer.Emplace(Transform.GetLocation());
    }
}

void AAmbitWheeledVehicleAIController::SetSpeedLimit(const float Speed)
{
    SpeedLimit = Speed;
    WaypointDistanceThreshold = SpeedLimit * VehicleControl::WaypointDistanceThresholdFactor;
}

void AAmbitWheeledVehicleAIController::SetLoopedPath(bool Looped)
{
    IsLooped = Looped;
}
