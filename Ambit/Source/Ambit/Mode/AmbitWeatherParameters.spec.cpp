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

#include "AmbitWeatherParameters.h"

#include "Misc/AutomationTest.h"

DEFINE_SPEC(AmbitWeatherParametersSpec, "Ambit.Unit.AmbitWeatherParameters",
            EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

void AmbitWeatherParametersSpec::Define()
{
    Describe("AmbitWeatherParameters Operator== overload", [this]()
    {
        It("The test WeatherParameters object has the same parameters as the RainyWeather", [this]()
        {
            const FAmbitWeatherParameters TestWeatherParameters = {90.0, 80.0, 100.0, 50.0, 20.0};
            TestTrue("The parameters should be equal to the preset rainy parameters.",
                     TestWeatherParameters == Weather::RainyParameters);
        });

        It("The test WeatherParameters object has the different parameters from the RainyWeather", [this]()
        {
            const FAmbitWeatherParameters TestWeatherParameters = {90.00001, 80.0, 100.0, 50.0, 20.0};
            TestFalse("The parameters should not be equal to the preset rainy parameters",
                      TestWeatherParameters == Weather::RainyParameters);
        });
    });
}
