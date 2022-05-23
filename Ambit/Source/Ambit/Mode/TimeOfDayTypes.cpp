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

#include "TimeOfDayTypes.h"

#include "Constant.h"
#include "ScenarioDefinition.h"
#include "Dom/JsonObject.h"

TSharedPtr<FJsonObject> FTimeOfDayTypes::SerializeToJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    JsonObject->SetBoolField(TimeOfDay::KMorning, this->bMorning);
    JsonObject->SetBoolField(TimeOfDay::KMidDay, this->bNoon);
    JsonObject->SetBoolField(TimeOfDay::KEvening, this->bEvening);
    JsonObject->SetBoolField(TimeOfDay::KNight, this->bNight);

    return JsonObject;
}

void FTimeOfDayTypes::DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject)
{
    if (JsonObject->HasField(TimeOfDay::KMorning))
    {
        SetMorning(JsonObject->GetBoolField(TimeOfDay::KMorning));
    }

    if (JsonObject->HasField(TimeOfDay::KMidDay))
    {
        SetNoon(JsonObject->GetBoolField(TimeOfDay::KMidDay));
    }

    if (JsonObject->HasField(TimeOfDay::KEvening))
    {
        SetEvening(JsonObject->GetBoolField(TimeOfDay::KEvening));
    }

    if (JsonObject->HasField(TimeOfDay::KNight))
    {
        SetNight(JsonObject->GetBoolField(TimeOfDay::KNight));
    }
}

int32 FTimeOfDayTypes::GetVariantCount()
{
    return EnabledTimeOfDayTypes.Num();
}

void FTimeOfDayTypes::ApplyVariant(int32 VariantIndex, FScenarioDefinition& Scenario)
{
    if (EnabledTimeOfDayTypes.Num() != 0)
    {
        const FString TimeOfDayTypes = EnabledTimeOfDayTypes[VariantIndex];
        Scenario.TimeOfDay = TimeOfDay::PresetTimeOfDayToNumber[TimeOfDayTypes];
    }
}

void FTimeOfDayTypes::SetMorning(bool SelectedMorning)
{
    this->bMorning = SelectedMorning;
    if (SelectedMorning)
    {
        if (!EnabledTimeOfDayTypes.Contains(TimeOfDay::KMorning))
        {
            EnabledTimeOfDayTypes.Emplace(TimeOfDay::KMorning);
        }
    }
    else
    {
        EnabledTimeOfDayTypes.Remove(TimeOfDay::KMorning);
    }
}

bool FTimeOfDayTypes::GetMorning() const
{
    return this->bMorning;
}

void FTimeOfDayTypes::SetNoon(bool SelectedNoon)
{
    this->bNoon = SelectedNoon;
    if (SelectedNoon)
    {
        if (!EnabledTimeOfDayTypes.Contains(TimeOfDay::KMidDay))
        {
            EnabledTimeOfDayTypes.Emplace(TimeOfDay::KMidDay);
        }
    }
    else
    {
        EnabledTimeOfDayTypes.Remove(TimeOfDay::KMidDay);
    }
}

bool FTimeOfDayTypes::GetNoon() const
{
    return this->bNoon;
}

void FTimeOfDayTypes::SetEvening(bool SelectedEvening)
{
    this->bEvening = SelectedEvening;
    if (SelectedEvening)
    {
        if (!EnabledTimeOfDayTypes.Contains(TimeOfDay::KEvening))
        {
            EnabledTimeOfDayTypes.Emplace(TimeOfDay::KEvening);
        }
    }
    else
    {
        EnabledTimeOfDayTypes.Remove(TimeOfDay::KEvening);
    }
}

bool FTimeOfDayTypes::GetEvening() const
{
    return this->bEvening;
}

void FTimeOfDayTypes::SetNight(bool SelectedNight)
{
    this->bNight = SelectedNight;
    if (SelectedNight)
    {
        if (!EnabledTimeOfDayTypes.Contains(TimeOfDay::KNight))
        {
            EnabledTimeOfDayTypes.Emplace(TimeOfDay::KNight);
        }
    }
    else
    {
        EnabledTimeOfDayTypes.Remove(TimeOfDay::KNight);
    }
}

bool FTimeOfDayTypes::GetNight() const
{
    return this->bNight;
}
