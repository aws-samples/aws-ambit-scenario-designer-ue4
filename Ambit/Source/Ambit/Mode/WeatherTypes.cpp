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

#include "WeatherTypes.h"

#include "Constant.h"
#include "ScenarioDefinition.h"
#include "Dom/JsonObject.h"

TSharedPtr<FJsonObject> FWeatherTypes::SerializeToJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    JsonObject->SetBoolField(Weather::KSunny, this->bSunny);
    JsonObject->SetBoolField(Weather::KRainy, this->bRainy);
    JsonObject->SetBoolField(Weather::KFoggy, this->bFoggy);

    return JsonObject;
}

void FWeatherTypes::DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject)
{
    if (JsonObject->HasField(Weather::KSunny))
    {
        SetSunny(JsonObject->GetBoolField(Weather::KSunny));
    }

    if (JsonObject->HasField(Weather::KRainy))
    {
        SetRainy(JsonObject->GetBoolField(Weather::KRainy));
    }

    if (JsonObject->HasField(Weather::KFoggy))
    {
        SetFoggy(JsonObject->GetBoolField(Weather::KFoggy));
    }
}

int32 FWeatherTypes::GetVariantCount()
{
    return EnabledWeatherTypes.Num();
}

void FWeatherTypes::ApplyVariant(int32 VariantIndex, FScenarioDefinition& Scenario)
{
    if (EnabledWeatherTypes.Num() != 0)
    {
        const FString WeatherTypes = EnabledWeatherTypes[VariantIndex];
        AmbitWeatherParameters::GetWeatherParametersFromPresetWeatherType(
            WeatherTypes, Scenario.AmbitWeatherParameters);
    }
}

void FWeatherTypes::SetSunny(bool SelectedSunny)
{
    this->bSunny = SelectedSunny;
    if (SelectedSunny)
    {
        if (!EnabledWeatherTypes.Contains(Weather::KSunny))
        {
            EnabledWeatherTypes.Emplace(Weather::KSunny);
        }
    }
    else
    {
        EnabledWeatherTypes.Remove(Weather::KSunny);
    }
}

bool FWeatherTypes::GetSunny() const
{
    return this->bSunny;
}

void FWeatherTypes::SetRainy(bool SelectedRainy)
{
    this->bRainy = SelectedRainy;
    if (SelectedRainy)
    {
        if (!EnabledWeatherTypes.Contains(Weather::KRainy))
        {
            EnabledWeatherTypes.Emplace(Weather::KRainy);
        }
    }
    else
    {
        EnabledWeatherTypes.Remove(Weather::KRainy);
    }
}

bool FWeatherTypes::GetRainy() const
{
    return this->bRainy;
}

void FWeatherTypes::SetFoggy(bool SelectedFoggy)
{
    this->bFoggy = SelectedFoggy;
    if (SelectedFoggy)
    {
        if (!EnabledWeatherTypes.Contains(Weather::KFoggy))
        {
            EnabledWeatherTypes.Emplace(Weather::KFoggy);
        }
    }
    else
    {
        EnabledWeatherTypes.Remove(Weather::KFoggy);
    }
}

bool FWeatherTypes::GetFoggy() const
{
    return this->bFoggy;
}
