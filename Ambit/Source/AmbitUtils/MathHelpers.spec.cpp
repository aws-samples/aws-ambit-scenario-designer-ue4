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

#include "MathHelpers.h"

#include "Misc/AutomationTest.h"

DEFINE_SPEC(MathHelpersSpec, "Ambit.Unit.Utils.MathHelpers",
            EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

void MathHelpersSpec::Define()
{
    Describe("Convert angles in radians to degrees", [this]()
    {
        It("Should be equal to FVector(60.0, 120.0, 90.0) when input is FVector(PI / 3.0, 2 * PI / 3.0, PI / 2.0)",
           [this]()
           {
               const FVector RotationInRadians(PI / 3.0, 2 * PI / 3.0, PI / 2);
               const FVector RotationInDegrees = FMathHelpers::RadiansToDegrees(RotationInRadians);
               TestEqual("The actual rotation in degrees is", RotationInDegrees, FVector(60.0, 120.0, 90.0));
           });

        It(
            "Should not be equal to FVector(60.0, 120.0, 90.0) when input is not FVector(PI / 3.0, 2 * PI / 3.0, PI / 2.0)",
            [this]()
            {
                const FVector RotationInRadians(-PI / 3.0, PI / 3.0, 0);
                const FVector RotationInDegrees = FMathHelpers::RadiansToDegrees(RotationInRadians);
                TestNotEqual("The actual rotation in degrees is", RotationInDegrees, FVector(60.0, 120.0, 90.0));
            });
    });

    Describe("Convert location in meters to centimeters.", [this]()
    {
        It("Should be equal to FVector(100, 0, -100) when input is FVector(1, 0, -1).", [this]()
        {
            const FVector LocationInMeters(1, 0, -1);
            TestEqual("The actor location in meters is", FMathHelpers::MetersToCentimeters(LocationInMeters),
                      FVector(100, 0, -100));
        });

        It("Should not be equal to FVector(100, 0, -100) when input is not FVector(1, 0, -1).", [this]()
        {
            const FVector LocationInMeters(0, 1, -1);
            TestNotEqual("The actor location in meters is", FMathHelpers::MetersToCentimeters(LocationInMeters),
                         FVector(100, 0, -100));
        });
    });

    Describe("Clamp density", [this]()
    {
        It("Should be equal to 0 if density is lower than 0", [this]()
        {
            FString ReturnedMessage;
            const FString ExpectedMessage = TEXT(
                "The imported test min (-0.500000) is lower than 0.000000 and will be clamped to 0.000000.");
            TestEqual("Should return 0 when density is lower than 0",
                      FMathHelpers::ClampBoundary(-0.5f, 0.f, 1.f, TEXT("test min"), ReturnedMessage), 0.f);

            TestEqual("Should return warning message for min", ReturnedMessage, ExpectedMessage);
        });
        It("Should be equal to 1 if density is greater than 1", [this]()
        {
            FString ReturnedMessage;
            const FString ExpectedMessage = TEXT(
                "The imported test max (100.000000) is greater than 1.000000 and will be clamped to 1.000000.");
            TestEqual("Should return 1 when density is greater than 1",
                      FMathHelpers::ClampBoundary(100.f, 0.f, 1.f, TEXT("test max"), ReturnedMessage), 1.f);

            TestEqual("Should return warning message for max", ReturnedMessage, ExpectedMessage);
        });

        It("Should return same density because it is in th range of [0.0, 1.0]", [this]()
        {
            FString ReturnedMessage;
            TestEqual("Should return the input density itself",
                      FMathHelpers::ClampBoundary(0.5f, 0.f, 1.f, TEXT("test"), ReturnedMessage), 0.5f);

            TestTrue("Should not return a warning message when correct", ReturnedMessage.IsEmpty());
        });
    });
}
