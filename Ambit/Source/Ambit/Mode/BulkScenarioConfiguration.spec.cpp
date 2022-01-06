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

#include "BulkScenarioConfiguration.h"

#include "ScenarioDefinition.h"
#include "Dom/JsonObject.h"
#include "Misc/AutomationTest.h"

#include <AmbitUtils/JsonHelpers.h>

BEGIN_DEFINE_SPEC(BulkScenarioConfigurationSpec, "Ambit.Unit.BulkScenarioConfiguration",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    FBulkScenarioConfiguration BulkConfig;
    TSharedPtr<FJsonObject> Json;

END_DEFINE_SPEC(BulkScenarioConfigurationSpec)

void BulkScenarioConfigurationSpec::Define()
{
    Describe("SerializeToJson()", [this]()
    {
        BeforeEach([this]()
        {
            BulkConfig = FBulkScenarioConfiguration{};
        });

        It("Configuration name is the same as the setup", [this]()
        {
            BulkConfig.ConfigurationName = "TestConfigurationName";
            const FString ConfigurationName = BulkConfig.SerializeToJson()->GetStringField(
                JsonConstants::KConfigurationNameKey);
            TestEqual("Configuration Name", ConfigurationName, "TestConfigurationName");
        });

        It("Batch name is the same as the setup", [this]()
        {
            BulkConfig.BatchName = "TestBatchName";
            const FString BatchName = BulkConfig.SerializeToJson()->GetStringField(JsonConstants::KBatchNameKey);
            TestEqual("BatchName", BatchName, "TestBatchName");
        });

        It("Time of day types is the same as the setup", [this]()
        {
            FTimeOfDayTypes TimeOfDay;
            TimeOfDay.SetMorning(true);
            BulkConfig.TimeOfDayTypes = TimeOfDay;
            const bool IsMorning = BulkConfig.SerializeToJson()->GetObjectField(JsonConstants::KTimeOfDayTypesKey)->
                                              GetBoolField(TimeOfDay::KMorning);
            TestTrue("The morning in time of day type is true", IsMorning);
        });

        It("Weather types is the same as the setup", [this]()
        {
            FWeatherTypes WeatherTypes;
            WeatherTypes.SetSunny(true);
            BulkConfig.WeatherTypes = WeatherTypes;
            const bool IsSunny = BulkConfig.SerializeToJson()->GetObjectField(JsonConstants::KWeatherTypesKey)->
                                            GetBoolField(Weather::KSunny);
            TestTrue("Sunny in weather type is true", IsSunny);
        });

        It("Batch pedestrian density is the same as the setup", [this]()
        {
            FPedestrianTraffic PedestrianDensity;
            PedestrianDensity.Min = 0.1f;
            PedestrianDensity.Max = 0.3f;
            BulkConfig.PedestrianDensity = PedestrianDensity;
            const float DensityMin = BulkConfig.SerializeToJson()->GetObjectField(
                JsonConstants::KBatchPedestrianDensityKey)->GetNumberField(JsonConstants::KMinKey);
            const float DensityMax = BulkConfig.SerializeToJson()->GetObjectField(
                JsonConstants::KBatchPedestrianDensityKey)->GetNumberField(JsonConstants::KMaxKey);
            TestEqual("The minimum of pedestrian density", DensityMin, 0.1f);
            TestEqual("The maximum of pedestrian density", DensityMax, 0.3f);
        });

        It("Batch vehicle density is the same as the setup", [this]()
        {
            FVehicleTraffic VehicleDensity;
            VehicleDensity.Min = 0.2f;
            VehicleDensity.Max = 0.4f;
            BulkConfig.VehicleDensity = VehicleDensity;
            const float DensityMin = BulkConfig.SerializeToJson()->GetObjectField(
                JsonConstants::KBatchTrafficDensityKey)->GetNumberField(JsonConstants::KMinKey);
            const float DensityMax = BulkConfig.SerializeToJson()->GetObjectField(
                JsonConstants::KBatchTrafficDensityKey)->GetNumberField(JsonConstants::KMaxKey);
            TestEqual("The minimum of vehicle density", DensityMin, 0.2f);
            TestEqual("The maximum of vehicle density", DensityMax, 0.4f);
        });
    });

    Describe("DeserializeFromJson()", [this]()
    {
        BeforeEach([this]()
        {
            BulkConfig = FBulkScenarioConfiguration{};
            const FString JsonFString = FString(
                "{"
                "   'Version': '1.0.0',"
                "   'ConfigurationName' : 'AmbitScenarioConfiguration',"
                "   'BulkScenarioName' : 'AmbitScenario',"
                "   'TimeOfDayTypes' :"
                "   {"
                "       'Morning': true,"
                "       'Noon' : false,"
                "       'Evening' : true,"
                "       'Night': false"
                "   },"
                "   'WeatherTypes':"
                "   {"
                "       'Sunny': true,"
                "       'Rainy' : true,"
                "       'Foggy' : false"
                "   },"
                "   'PedestrianDensity':"
                "   {"
                "       'Min': 0,"
                "       'Max' : 0.10000000149011612,"
                "       'Increment' : 0.10000000149011612"
                "   },"
                "   'TrafficDensity':"
                "   {"
                "       'Min': 0,"
                "       'Max' : 0.10000000149011612,"
                "       'Increment' : 0.10000000149011612"
                "   },"
                "   'NumberOfPermutations': 16"
                "}"
            ).Replace(TEXT("'"), TEXT("\""));
            Json = FJsonHelpers::DeserializeJson(JsonFString);
        });

        It("when ConfigurationName is 'AmbitScenarioConfiguration'", [this]()
        {
            BulkConfig.DeserializeFromJson(Json);
            TestEqual("Configuration Name", BulkConfig.ConfigurationName, "AmbitScenarioConfiguration");
        });

        It("When BulkScenarioName is 'AmbitScenario'", [this]()
        {
            BulkConfig.DeserializeFromJson(Json);
            TestEqual("BulkScenario Name", BulkConfig.BatchName, "AmbitScenario");
        });

        It("When Morning and Evening in time of day types are true", [this]()
        {
            BulkConfig.DeserializeFromJson(Json);
            TestTrue("Time of day types: Morning", BulkConfig.TimeOfDayTypes.GetMorning());
            TestTrue("Time of day types: Evening", BulkConfig.TimeOfDayTypes.GetEvening());
        });

        It("When Sunny and Rainy in weather types are true", [this]()
        {
            BulkConfig.DeserializeFromJson(Json);
            TestTrue("Weather types: Sunny", BulkConfig.WeatherTypes.GetSunny());
            TestTrue("Weather types: Rainy", BulkConfig.WeatherTypes.GetRainy());
        });

        It("Pedestrian Density min and max", [this]()
        {
            BulkConfig.DeserializeFromJson(Json);
            TestEqual("The minimum of pedestrian density is", BulkConfig.PedestrianDensity.Min, 0.0f);
            TestEqual("The maximum of pedestrian density is", BulkConfig.PedestrianDensity.Max, 0.1f);
        });

        It("Vehicle Density min and max", [this]()
        {
            BulkConfig.DeserializeFromJson(Json);
            TestEqual("The minimum of vehicle density is", BulkConfig.VehicleDensity.Min, 0.0f);
            TestEqual("The maximum of vehicle density is", BulkConfig.VehicleDensity.Max, 0.1f);
        });
    });

    Describe("GenerateScenarios()", [this]()
    {
        BeforeEach([this]()
        {
            BulkConfig = FBulkScenarioConfiguration{};
        });
        It("when single permutation is expected", [this]()
        {
            // Configure for single weather variant.
            BulkConfig.WeatherTypes.SetRainy(true);

            // Configure for single pedestrian traffic variant.
            BulkConfig.PedestrianDensity.Min = 0.2f;
            BulkConfig.PedestrianDensity.Max = 0.2f;

            // Configure for single traffic variant.
            BulkConfig.VehicleDensity.Min = 0.5f;
            BulkConfig.VehicleDensity.Max = 0.5f;

            const TArray<FScenarioDefinition> GeneratedScenarios = BulkConfig.GenerateScenarios();

            // Test for expected number of permutations.
            TestEqual("Scenario count", GeneratedScenarios.Num(), 1);
            if (HasAnyErrors())
            {
                return;
            }

            // Test that the correct scenario was generated.
            const FScenarioDefinition Scenario = GeneratedScenarios[0];
            TestEqual("Precipitation", Scenario.AmbitWeatherParameters.Precipitation, 80.0f);
            TestEqual("Pedestrian Density", Scenario.PedestrianDensity, 0.2f);
            TestEqual("Vehicle Density", Scenario.VehicleDensity, 0.5f);
        });

        It("when single permutation is expected with no weather and time of day types", [this]()
        {
            // Configure for single pedestrian traffic variant.
            BulkConfig.PedestrianDensity.Min = 0.0f;
            BulkConfig.PedestrianDensity.Max = 0.0f;

            // Configure for single traffic variant.
            BulkConfig.VehicleDensity.Min = 0.0f;
            BulkConfig.VehicleDensity.Max = 0.0f;

            const TArray<FScenarioDefinition> GeneratedScenarios = BulkConfig.GenerateScenarios();

            // Test for expected number of permutations.
            TestEqual("Scenario count", GeneratedScenarios.Num(), 1);
            if (HasAnyErrors())
            {
                return;
            }

            // Test that the correct scenario was generated.
            const FScenarioDefinition Scenario = GeneratedScenarios[0];
            TestEqual("Pedestrian Density", Scenario.PedestrianDensity, 0.0f);
            TestEqual("Vehicle Density", Scenario.VehicleDensity, 0.0f);
        });

        It("when 2 weather variants are configured", [this]()
        {
            // Configure for two weather variants.
            BulkConfig.WeatherTypes.SetSunny(true);
            BulkConfig.WeatherTypes.SetRainy(true);

            // Configure for single pedestrian traffic variant.
            BulkConfig.PedestrianDensity.Min = 0.2;
            BulkConfig.PedestrianDensity.Max = 0.2;

            // Configure for single traffic variant.
            BulkConfig.VehicleDensity.Min = 0.5;
            BulkConfig.VehicleDensity.Max = 0.5;

            const TArray<FScenarioDefinition> GeneratedScenarios = BulkConfig.GenerateScenarios();

            // Test for expected number of permutations.
            TestEqual("scenario count", GeneratedScenarios.Num(), 2);
            if (HasAnyErrors())
            {
                return;
            }

            // Test that the correct scenarios were generated.
            TestEqual("The Cloudiness in Sunny day", GeneratedScenarios[0].AmbitWeatherParameters.Cloudiness, 20.0f);
            TestEqual("The Cloudiness in Rainy day", GeneratedScenarios[1].AmbitWeatherParameters.Cloudiness, 90.0f);
        });

        It("when 2 weather variant and 3 pedestrian variants are configured", [this]()
        {
            // Configure for two weather variants.
            BulkConfig.WeatherTypes.SetSunny(true);
            BulkConfig.WeatherTypes.SetFoggy(true);

            // Configure for single pedestrian traffic variant.
            BulkConfig.PedestrianDensity.Min = 0.2;
            BulkConfig.PedestrianDensity.Max = 0.4;

            // Configure for single traffic variant.
            BulkConfig.VehicleDensity.Min = 0.5;
            BulkConfig.VehicleDensity.Max = 0.5;

            const TArray<FScenarioDefinition> GeneratedScenarios = BulkConfig.GenerateScenarios();

            // Test for expected number of permutations.
            TestEqual("scenario count", GeneratedScenarios.Num(), 6);
            if (HasAnyErrors())
            {
                return;
            }

            // Test that the correct scenarios were generated.
            const auto FirstScenario = GeneratedScenarios[0];
            TestEqual("The Cloudiness in the first scenario", FirstScenario.AmbitWeatherParameters.Cloudiness, 20.0f);
            TestEqual("Pedestrian Density in the first scenario", FirstScenario.PedestrianDensity, 0.2f);

            const auto LastScenario = GeneratedScenarios[5];
            TestEqual("Cloudiness in the last scenario", LastScenario.AmbitWeatherParameters.Cloudiness, 30.f);
            TestEqual("Pedestrian Density in the last scenario", LastScenario.PedestrianDensity, 0.4f);
        });

        It(
            "when 3 weather variants, 4 time of day variants, 11 pedestrian variants and 11 traffic variants are configured",
            [this]()
            {
                // Configure for all weather variants.
                BulkConfig.WeatherTypes.SetSunny(true);
                BulkConfig.WeatherTypes.SetRainy(true);
                BulkConfig.WeatherTypes.SetFoggy(true);

                // Configure for all time of day variants.
                BulkConfig.TimeOfDayTypes.SetMorning(true);
                BulkConfig.TimeOfDayTypes.SetNoon(true);
                BulkConfig.TimeOfDayTypes.SetEvening(true);
                BulkConfig.TimeOfDayTypes.SetNight(true);

                // Configure for single pedestrian traffic variant.
                BulkConfig.PedestrianDensity.Min = 0;
                BulkConfig.PedestrianDensity.Max = 1;

                // Configure for single traffic variant.
                BulkConfig.VehicleDensity.Min = 0;
                BulkConfig.VehicleDensity.Max = 1;

                const TArray<FScenarioDefinition> GeneratedScenarios = BulkConfig.GenerateScenarios();

                // Test for expected number of permutations.
                TestEqual("scenario count", GeneratedScenarios.Num(), 3 * 4 * 11 * 11);

                // Test that the correct scenarios were generated.
                const auto FirstScenario = GeneratedScenarios[0];
                TestEqual("The Cloudiness in the first scenario", FirstScenario.AmbitWeatherParameters.Cloudiness,
                          20.0f);
                TestEqual("The Time of day in the first scenario", FirstScenario.TimeOfDay, 6.0f);
                TestEqual("Pedestrian Density in the first scenario", FirstScenario.PedestrianDensity, 0.0f);
                TestEqual("Vehicle Density in the first scenario", FirstScenario.VehicleDensity, 0.0f);

                const auto LastScenario = GeneratedScenarios[3 * 4 * 11 * 11 - 1];
                TestEqual("Cloudiness in the last scenario", LastScenario.AmbitWeatherParameters.Cloudiness, 30.f);
                TestEqual("The Time of day in the last scenario", LastScenario.TimeOfDay, 0.0f);
                TestEqual("Pedestrian Density in the last scenario", LastScenario.PedestrianDensity, 1.0f);
                TestEqual("Vehicle Density in the last scenario", LastScenario.VehicleDensity, 1.0f);
            });
    });
}
