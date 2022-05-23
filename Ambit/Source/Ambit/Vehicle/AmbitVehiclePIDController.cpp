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

#include "AmbitVehiclePIDController.h"

float FAmbitVehiclePIDController::RunStep(const float Error, const float DT)
{
    Errors.Emplace(Error);

    float Deviation;
    float IntegralDeviation;

    if (Errors.Num() >= 2)
    {
        //Only keep most recent 10 errors which is a magic number after multiple tweak
        if (Errors.Num() > 10)
        {
            Errors.RemoveAt(0);
        }
        //Calculate the deviation of the error at this step
        Deviation = (Errors.Last(0) - Errors.Last(1)) / DT;
        float Sum = 0;
        for (int i = 0; i < Errors.Num(); i++)
        {
            Sum += Errors[i];
        }
        //Calculate the Integral deviation for all the errors we keep so far
        IntegralDeviation = Sum * DT;
    }
    else
    {
        Deviation = 0;
        IntegralDeviation = 0;
    }

    return FMath::Clamp(Proportional * Error + Differential * Deviation + Integral * IntegralDeviation, -1.f, 1.f);
}

float FAmbitVehicleLongitudinalController::RunStep(const float TargetSpeed, const float CurrentSpeed, const float DT)
{
    const float Error = TargetSpeed - CurrentSpeed;
    return FAmbitVehiclePIDController::RunStep(Error, DT);
}

float FAmbitVehicleLateralController::RunStep(const FVector TargetLocation, const FVector CurrentLocation,
                                              const FVector Forward, const float DT)
{
    //Make forward vector only on x,y axis
    const FVector VecV = FVector(Forward.X, Forward.Y, 0);
    //Calculate the desired forward facing vector from targetlocation - currentlocation
    const FVector VecW = FVector(TargetLocation.X - CurrentLocation.X, TargetLocation.Y - CurrentLocation.Y, 0);

    //Calculate angle between two directional vector
    float Angle = FGenericPlatformMath::Acos(FVector::DotProduct(VecW.GetSafeNormal(), VecV.GetSafeNormal()));

    //Inverse the sign of angle if the cross product of two vectors is negative
    const FVector Cross = FVector::CrossProduct(VecV, VecW);
    if (Cross.Z < 0)
    {
        Angle *= -1.f;
    }

    return FAmbitVehiclePIDController::RunStep(Angle, DT);
}
