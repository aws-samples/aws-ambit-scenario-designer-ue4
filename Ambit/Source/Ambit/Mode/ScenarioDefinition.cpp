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

#include "ScenarioDefinition.h"

#include "AmbitUtils/MathHelpers.h"
#include "AmbitUtils/MenuHelpers.h"

#include "Constant.h"
#include "JsonObjectConverter.h"

// This should be incremented every time changes are made to
// the serialization/deseriaization logic.
const FString FScenarioDefinition::KCurrentVersion = "1.0.0";

TSharedPtr<FJsonObject> FScenarioDefinition::SerializeToJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    JsonObject->SetStringField(JsonConstants::KVersionKey, KCurrentVersion);
    JsonObject->SetStringField(JsonConstants::KScenarioNameKey,
                               this->ScenarioName);
    JsonObject->SetNumberField(JsonConstants::KTimeOfDayKey,
                               this->TimeOfDay);
    JsonObject->SetObjectField(JsonConstants::KWeatherParametersKey,
                               FJsonObjectConverter::UStructToJsonObject(this->AmbitWeatherParameters));
    JsonObject->SetNumberField(JsonConstants::KPedestrianDensityKey,
                               this->PedestrianDensity);
    JsonObject->SetNumberField(JsonConstants::KTrafficDensityKey,
                               this->VehicleDensity);
    JsonObject->SetObjectField(JsonConstants::KAllSpawnersConfigsKey, this->AllSpawnersConfigs);

    return JsonObject;
}

void FScenarioDefinition::DeserializeFromJson(
    TSharedPtr<FJsonObject> JsonObject)
{
    FString Warnings;

    if (JsonObject->HasField(JsonConstants::KVersionKey))
    {
        const FString Version = JsonObject->GetStringField(JsonConstants::KVersionKey);
        // TODO: Support a list of approved/backwards compatible versions?
        if (!Version.Equals(KCurrentVersion))
        {
            const FString InvalidVersion = FString::Printf(TEXT("The version in the file (%s) is not supported"), *Version);
            FMenuHelpers::LogErrorAndPopup(InvalidVersion);

            return;
        }
    }

    if (JsonObject->HasField(JsonConstants::KScenarioNameKey))
    {
        this->ScenarioName = JsonObject->GetStringField(
            JsonConstants::KScenarioNameKey);
    }

    if (JsonObject->HasField(JsonConstants::KTimeOfDayKey))
    {
        FString TempMessage;

        const float IncomingTime = JsonObject->GetNumberField(
            JsonConstants::KTimeOfDayKey);
        this->TimeOfDay = FMathHelpers::ClampBoundary(IncomingTime, 0.f, 23.99999f,
            TEXT("time of day"), TempMessage);

        if (!TempMessage.IsEmpty())
        {
            Warnings.Append(TempMessage + LINE_TERMINATOR);
        }
    }

    if (JsonObject->HasField(JsonConstants::KWeatherParametersKey))
    {
        FJsonObjectConverter::JsonObjectToUStruct(
            JsonObject->GetObjectField(JsonConstants::KWeatherParametersKey).
                        ToSharedRef(),
            FAmbitWeatherParameters::StaticStruct(), &(this->AmbitWeatherParameters), 0, 0);

        FString CloudinessTempMessage;
        this->AmbitWeatherParameters.Cloudiness = FMathHelpers::ClampBoundary(this->AmbitWeatherParameters.Cloudiness, 0.f, 100.f,
            TEXT("cloudiness"), CloudinessTempMessage);
        if (!CloudinessTempMessage.IsEmpty())
        {
            Warnings.Append(CloudinessTempMessage + LINE_TERMINATOR);
        }

        FString PrecipitationTempMessage;
        this->AmbitWeatherParameters.Precipitation = FMathHelpers::ClampBoundary(this->AmbitWeatherParameters.Precipitation, 0.f, 100.f,
            TEXT("precipitation"), PrecipitationTempMessage);
        if (!PrecipitationTempMessage.IsEmpty())
        {
            Warnings.Append(PrecipitationTempMessage + LINE_TERMINATOR);
        }

        FString PuddlesTempMessage;
        this->AmbitWeatherParameters.Puddles = FMathHelpers::ClampBoundary(this->AmbitWeatherParameters.Puddles, 0.f, 100.f,
            TEXT("puddles"), PuddlesTempMessage);
        if (!PuddlesTempMessage.IsEmpty())
        {
            Warnings.Append(PuddlesTempMessage + LINE_TERMINATOR);
        }

        FString WetnessTempMessage;
        this->AmbitWeatherParameters.Wetness = FMathHelpers::ClampBoundary(this->AmbitWeatherParameters.Wetness, 0.f, 100.f,
            TEXT("cloudiness"), WetnessTempMessage);
        if (!WetnessTempMessage.IsEmpty())
        {
            Warnings.Append(WetnessTempMessage + LINE_TERMINATOR);
        }

        FString FogDensityTempMessage;
        this->AmbitWeatherParameters.FogDensity = FMathHelpers::ClampBoundary(this->AmbitWeatherParameters.FogDensity, 0.f, 100.f,
            TEXT("fog density"), FogDensityTempMessage);
        if (!FogDensityTempMessage.IsEmpty())
        {
            Warnings.Append(FogDensityTempMessage + LINE_TERMINATOR);
        }
    }

    if (JsonObject->HasField(JsonConstants::KPedestrianDensityKey))
    {
        FString TempMessage;

        const float IncomingDensity = JsonObject->GetNumberField(
            JsonConstants::KPedestrianDensityKey);
        this->PedestrianDensity = FMathHelpers::ClampBoundary(IncomingDensity, 0.f, 1.f,
            TEXT("pedestrian density"), TempMessage);

        if (!TempMessage.IsEmpty())
        {
            Warnings.Append(TempMessage + LINE_TERMINATOR);
        }
    }

    if (JsonObject->HasField(JsonConstants::KTrafficDensityKey))
    {
        FString TempMessage;

        const float IncomingDensity = JsonObject->GetNumberField(
            JsonConstants::KTrafficDensityKey);
        this->VehicleDensity = FMathHelpers::ClampBoundary(IncomingDensity, 0.f, 1.f,
            TEXT("vehicle density"), TempMessage);

        if (!TempMessage.IsEmpty())
        {
            Warnings.Append(TempMessage + LINE_TERMINATOR);
        }
    }

    if (!Warnings.IsEmpty())
    {
        FMenuHelpers::LogErrorAndPopup(Warnings);
    }
}
