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

#include "AmbitVehiclePIDController.h"

#include "Misc/AutomationTest.h"

BEGIN_DEFINE_SPEC(AmbitVehiclePIDControllerSpec, "Ambit.Unit.AmbitVehiclePIDController",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    FAmbitVehiclePIDController Controller;
END_DEFINE_SPEC(AmbitVehiclePIDControllerSpec)

void AmbitVehiclePIDControllerSpec::Define()
{
    Describe("AmbitVehiclePIDController", [this]()
    {
        BeforeEach([this]()
        {
            Controller = FAmbitVehiclePIDController(0.5f, 0.5f, 0.5f);
        });
        Describe("RunStep()", [this]()
        {
            It("return correct result", [this]()
            {
                const float Result = Controller.RunStep(1, 0.02);
                TestEqual("RunStepResult", Result, 0.5f);
            });

            It("clamp to value between -1 and 1", [this]()
            {
                const float Result = Controller.RunStep(100000, 0.02);
                TestTrue("RunStepResult", Result >= -1);
                TestTrue("RunStepResult", Result <= 1);
            });
        });
    });

    Describe("AmbitVehicleLongitudinalController", [this]()
    {
        Describe("RunStep()", [this]()
        {
            It("return correct result", [this]()
            {
                FAmbitVehicleLongitudinalController Controller = FAmbitVehicleLongitudinalController(0.5f, 0.5f, 0.5f);

                const float Result = Controller.RunStep(30, 29, 0.02);
                TestEqual("RunStepResult", Result, 0.5f);
            });
        });
    });

    Describe("AmbitVehicleLateralController", [this]()
    {
        Describe("RunStep()", [this]()
        {
            It("return correct result", [this]()
            {
                FAmbitVehicleLateralController Controller = FAmbitVehicleLateralController(0.5f, 0.5f, 0.5f);

                const float Result = Controller.RunStep(FVector(0, 10, 0), FVector(0, 1, 0), FVector(0, 1, 0), 0.02);
                TestEqual("RunStepResult", Result, 0.f);
            });
        });
    });
}
