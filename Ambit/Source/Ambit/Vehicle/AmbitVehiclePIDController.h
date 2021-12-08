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

/**
 * Base class for Vehicle PID Controller
 */
class AMBIT_API FAmbitVehiclePIDController
{
public:
    FAmbitVehiclePIDController() = default;

    FAmbitVehiclePIDController(const float P, const float D, const float I)
        : Proportional(P), Differential(D), Integral(I)
    {
    }

    /**
     * Execute one step of control given the error received for this step using P,I,D factors
     *
     * @param Error
     *  Error received at current state
     * @param DT
     *  Delta time for this step
     * @return
     *  Input value to control the vehicle
     */
    float RunStep(const float Error, const float DT);

private:
    float Proportional = 0.f;

    float Differential = 0.f;

    float Integral = 0.f;

    TArray<float> Errors;
};

/**
 * Longitudinal control using a PID
 */
class AMBIT_API FAmbitVehicleLongitudinalController : public FAmbitVehiclePIDController
{
public:
    FAmbitVehicleLongitudinalController() = default;

    FAmbitVehicleLongitudinalController(const float P, const float D, const float I)
        : FAmbitVehiclePIDController(P, D, I)
    {
    }

    /**
     * Execute one step of control for this Longitudinal controller
     *
     * @param TargetSpeed
     *  Target speed to achieve
     * @param CurrentSpeed
     *  Current speed
     * @param DT
     *  Delta time for this step
     * @return
     *  throttle input value to control the vehicle
     */
    float RunStep(const float TargetSpeed, const float CurrentSpeed, const float DT);
};

/**
 * Lateral control using a PID
 */
class AMBIT_API FAmbitVehicleLateralController : public FAmbitVehiclePIDController
{
public:
    FAmbitVehicleLateralController() = default;

    FAmbitVehicleLateralController(const float P, const float D, const float I)
        : FAmbitVehiclePIDController(P, D, I)
    {
    }

    /**
     * Execute one step of control for this Lateral controller
     *
     * @param TargetLocation
     *  Target location to reach
     * @param CurrentLocation
     *  Current location
     * @param Forward
     *  Forward facing direction of vehicle
     * @param DT
     *  Delta time for this step
     * @return
     *  steering input value to control the vehicle
     */
    float RunStep(const FVector TargetLocation, const FVector CurrentLocation, const FVector Forward, const float DT);
};
