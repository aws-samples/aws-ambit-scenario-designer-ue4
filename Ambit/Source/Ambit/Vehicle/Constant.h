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

namespace PIDController
{
    const static float LongitudinalProportionalTerm = 0.15f;
    const static float LongitudinalDifferentialTerm = 0.05f;
    const static float LongitudinalIntegralTerm = 0.07f;
    const static float LateralProportionalTerm = 0.58f;
    const static float LateralDifferentialTerm = 0.02f;
    const static float LateralIntegralTerm = 0.5f;
}

namespace VehicleControl
{
    const static float SteeringDelta = 0.2f;
    const static float WaypointDistanceThresholdFactor = 0.25f;
    const static float TurningRadiusSpeedFactor = 0.5f;
    const static float LookingAheadDistanceFactor = 3.f;
}
