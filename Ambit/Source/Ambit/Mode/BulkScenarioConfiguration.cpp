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

#include "Constant.h"
#include "ScenarioDefinition.h"
#include "Dom/JsonObject.h"

#include "AmbitUtils/MenuHelpers.h"

// This should be incremented every time changes are made to the serialization/deseriaization logic.
const FString FBulkScenarioConfiguration::KCurrentVersion = "1.0.0";

/**
 *Serialize the BSC config information into a Json object
 */
TSharedPtr<FJsonObject> FBulkScenarioConfiguration::SerializeToJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    JsonObject->SetStringField(JsonConstants::KVersionKey, KCurrentVersion);
    JsonObject->SetStringField(JsonConstants::KConfigurationNameKey,
                               this->ConfigurationName);
    JsonObject->SetStringField(JsonConstants::KBatchNameKey, this->BatchName);
    JsonObject->SetObjectField(JsonConstants::KTimeOfDayTypesKey,
                               this->TimeOfDayTypes.SerializeToJson());
    JsonObject->SetObjectField(JsonConstants::KWeatherTypesKey,
                               this->WeatherTypes.SerializeToJson());
    JsonObject->SetObjectField(JsonConstants::KBatchPedestrianDensityKey,
                               this->PedestrianDensity.SerializeToJson());
    JsonObject->SetObjectField(JsonConstants::KBatchTrafficDensityKey,
                               this->VehicleDensity.SerializeToJson());
    JsonObject->SetNumberField(JsonConstants::KNumberOfPermutationsKey,
                               this->NumberOfPermutations);
    JsonObject->SetObjectField(JsonConstants::KAllSpawnersConfigsKey, this->AllSpawnersConfigs);

    return JsonObject;
}

/**
 *Deserialize the BSC Json object into Unreal engine editor
 */
void FBulkScenarioConfiguration::DeserializeFromJson(
    TSharedPtr<FJsonObject> JsonObject)
{
    if (JsonObject->HasField(JsonConstants::KVersionKey))
    {
        const FString Version = JsonObject->GetStringField(JsonConstants::KVersionKey);
        // TODO: Support a list of approved/backwards compatible versions?
        if (!Version.Equals(KCurrentVersion))
        {
            const FString InvalidVersion = FString::Printf(
                TEXT("The version in the file (%s) is not supported"), *Version);
            FMenuHelpers::LogErrorAndPopup(InvalidVersion);

            return;
        }
    }

    if (JsonObject->HasField(JsonConstants::KConfigurationNameKey))
    {
        this->ConfigurationName = JsonObject->GetStringField(
            JsonConstants::KConfigurationNameKey);
    }

    if (JsonObject->HasField(JsonConstants::KBatchNameKey))
    {
        this->BatchName = JsonObject->GetStringField(
            JsonConstants::KBatchNameKey);
    }

    if (JsonObject->HasField(JsonConstants::KTimeOfDayTypesKey))
    {
        this->TimeOfDayTypes.DeserializeFromJson(
            JsonObject->GetObjectField(
                JsonConstants::KTimeOfDayTypesKey));
    }

    if (JsonObject->HasField(JsonConstants::KWeatherTypesKey))
    {
        this->WeatherTypes.DeserializeFromJson(
            JsonObject->GetObjectField(
                JsonConstants::KWeatherTypesKey));
    }

    if (JsonObject->HasField(JsonConstants::KBatchPedestrianDensityKey))
    {
        this->PedestrianDensity.DeserializeFromJson(
            JsonObject->GetObjectField(
                JsonConstants::KBatchPedestrianDensityKey));
    }

    if (JsonObject->HasField(JsonConstants::KBatchTrafficDensityKey))
    {
        this->VehicleDensity.DeserializeFromJson(
            JsonObject->GetObjectField(
                JsonConstants::KBatchTrafficDensityKey));
    }

    if (JsonObject->HasField(JsonConstants::KNumberOfPermutationsKey))
    {
        this->NumberOfPermutations = JsonObject->GetNumberField(
            JsonConstants::KNumberOfPermutationsKey);
    }
}

TArray<FScenarioDefinition> FBulkScenarioConfiguration::GenerateScenarios()
{
    const TArray<IScenarioParameter*> VariantParameters
    {
        &TimeOfDayTypes,
        &WeatherTypes,
        &PedestrianDensity,
        &VehicleDensity
        // Add more parameters here as needed.
    };

    TArray<FScenarioDefinition> Scenarios;
    FScenarioDefinition CurrentSDF;
    GetAllPermutationScenarios(VariantParameters, Scenarios, 0, CurrentSDF);
    return Scenarios;
}


void FBulkScenarioConfiguration::GetAllPermutationScenarios(TArray<IScenarioParameter*> VariantParameters,
                                                            TArray<FScenarioDefinition>& Scenarios, int Depth,
                                                            FScenarioDefinition& CurrentSDF)
{
    if (Depth == VariantParameters.Num())
    {
        Scenarios.Add(CurrentSDF);
        return;
    }
    IScenarioParameter* Parameter = VariantParameters[Depth];
    const int32 Variants = Parameter->GetVariantCount();
    if (Variants == 0)
    {
        GetAllPermutationScenarios(VariantParameters, Scenarios, Depth + 1, CurrentSDF);
    }
    else
    {
        for (int32 i = 0; i < Variants; i++)
        {
            Parameter->ApplyVariant(i, CurrentSDF);
            GetAllPermutationScenarios(VariantParameters, Scenarios, Depth + 1, CurrentSDF);
        }
    }
}
