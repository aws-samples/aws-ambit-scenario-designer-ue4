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
 * This namespace contains helper functions to deal with unit conversions.
 */
namespace FMathHelpers
{
    /**
     * Convert angles in radians to degrees by using Math library.
     *
     * @param Rotation is a vector saving angles in radians.
     * @return Return a FVector saving angles in degrees.
     */
    AMBITUTILS_API FVector RadiansToDegrees(FVector Rotation);

    /**
     * Convert a transformation in meters to centimeters by multiplying 100.
     * The length unit in UE4 is in centimeters.
     *
     * @param Location is a vector saving (x, y, z) in meters.
     * @return Return a FVector saving (x, y, z) in centimeters.
     */
    AMBITUTILS_API FVector MetersToCentimeters(FVector Location);

    /**
    * Clamp the density boundary to [Min, Max].
    *
    * @param Density the density which may be out of range [Min, Max].
    * @param Min the minimum boundary
    * @param Max the maximum boundary
    * @param DensityBoundary the min or max boundary for output error message.
    * @param OutMessage Contains the warning message, if there was one generated.
    *
    * @return The float number indicating the clamped density for the specific density boundary.
    */
    AMBITUTILS_API float ClampBoundary(float Density, float Min, float Max, const FString& DensityBoundary, FString& OutMessage);
};
