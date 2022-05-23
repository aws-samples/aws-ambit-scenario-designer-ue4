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

#include "MathHelpers.h"

FVector FMathHelpers::RadiansToDegrees(FVector Rotation)
{
    return FVector(FMath::RadiansToDegrees(Rotation.X), FMath::RadiansToDegrees(Rotation.Y),
                   FMath::RadiansToDegrees(Rotation.Z));
}

FVector FMathHelpers::MetersToCentimeters(FVector Location)
{
    return FVector(Location.X * 100, Location.Y * 100, Location.Z * 100);
}

float FMathHelpers::ClampBoundary(float Density, float Min, float Max, const FString& DensityBoundary,
                                  FString& OutMessage)
{
    if (Density < Min)
    {
        OutMessage = FString::Printf(
            TEXT("The imported %s (%f) is lower than %f and will be clamped to %f."), *DensityBoundary, Density, Min,
            Min);
        Density = Min;
    }
    if (Density > Max)
    {
        OutMessage = FString::Printf(
            TEXT("The imported %s (%f) is greater than %f and will be clamped to %f."), *DensityBoundary, Density, Max,
            Max);
        Density = Max;
    }
    return Density;
}
