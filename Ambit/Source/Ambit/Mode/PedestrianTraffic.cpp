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

#include "PedestrianTraffic.h"

#include "Constant.h"
#include "ScenarioDefinition.h"
#include "Dom/JsonObject.h"

#include "AmbitUtils/MathHelpers.h"
#include "AmbitUtils/MenuHelpers.h"

TSharedPtr<FJsonObject> FPedestrianTraffic::SerializeToJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

    JsonObject->SetNumberField(JsonConstants::KMinKey, this->Min);
    JsonObject->SetNumberField(JsonConstants::KMaxKey, this->Max);
    JsonObject->SetNumberField(JsonConstants::KIncrementKey, this->Increment);

    return JsonObject;
}

void FPedestrianTraffic::DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject)
{
    FString Warnings;
    if (JsonObject->HasField(JsonConstants::KMinKey))
    {
        FString TempMessage;

        const float IncomingMin = JsonObject->GetNumberField(
            JsonConstants::KMinKey);
        this->Min = FMathHelpers::ClampBoundary(IncomingMin, 0.f, 1.f,
                                                TEXT("pedestrian density min"), TempMessage);

        if (!TempMessage.IsEmpty())
        {
            Warnings.Append(TempMessage + LINE_TERMINATOR);
        }
    }

    if (JsonObject->HasField(JsonConstants::KMaxKey))
    {
        FString TempMessage;

        const float IncomingMax = JsonObject->GetNumberField(
            JsonConstants::KMaxKey);
        this->Max = FMathHelpers::ClampBoundary(IncomingMax, 0.f, 1.f,
                                                TEXT("pedestrian density max"), TempMessage);

        if (!TempMessage.IsEmpty())
        {
            Warnings.Append(TempMessage + LINE_TERMINATOR);
        }
    }

    if (JsonObject->HasField(JsonConstants::KIncrementKey))
    {
        FString TempMessage;

        const float IncomingIncrement = JsonObject->GetNumberField(
            JsonConstants::KIncrementKey);
        this->Increment = FMathHelpers::ClampBoundary(IncomingIncrement, 0.1f, 0.1f,
                                                      TEXT("pedestrian density increment"), TempMessage);

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

int32 FPedestrianTraffic::GetVariantCount()
{
    if (Max < Min)
    {
        //TODO: pop up a notification for customers to change the min/max?
        return 1;
    }
    return FMath::FloorToInt((Max - Min) / Increment) + 1;
}

void FPedestrianTraffic::ApplyVariant(int32 VariantIndex,
                                      FScenarioDefinition& Scenario)
{
    // Configure traffic density-related properties.
    Scenario.PedestrianDensity = Min + Increment * VariantIndex;
}
