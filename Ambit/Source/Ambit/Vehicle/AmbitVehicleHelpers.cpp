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

#include "AmbitVehicleHelpers.h"

float AmbitVehicleHelpers::GetThreePointCircleRadius(FVector FirstLocation, FVector MiddleLocation,
                                                     FVector LastLocation)
{
    float x1 = FirstLocation.X;
    float y1 = FirstLocation.Y;
    float x2 = MiddleLocation.X;
    float y2 = MiddleLocation.Y;
    float x3 = LastLocation.X;
    float y3 = LastLocation.Y;

    float x12 = x1 - x2;
    float x13 = x1 - x3;
    float y12 = y1 - y2;
    float y13 = y1 - y3;
    float y31 = y3 - y1;
    float y21 = y2 - y1;
    float x31 = x3 - x1;
    float x21 = x2 - x1;

    float sx13 = x1 * x1 - x3 * x3;
    float sy13 = y1 * y1 - y3 * y3;
    float sx21 = x2 * x2 - x1 * x1;
    float sy21 = y2 * y2 - y1 * y1;

    float f_denom = 2 * (y31 * x12 - y21 * x13);
    if (f_denom == 0)
    {
        return TNumericLimits<float>::Max();
    }
    float f = (sx13 * x12 + sy13 * x12 + sx21 * x13 + sy21 * x13) / f_denom;

    float g_denom = 2 * (x31 * y12 - x21 * y13);
    if (g_denom == 0)
    {
        return TNumericLimits<float>::Max();
    }
    float g = (sx13 * y12 + sy13 * y12 + sx21 * y13 + sy21 * y13) / g_denom;

    float c = -(x1 * x1 + y1 * y1) - 2 * g * x1 - 2 * f * y1;
    float h = -g;
    float k = -f;

    return FMath::Sqrt(h * h + k * k - c);
}
