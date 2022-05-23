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

#pragma once

#include "CoreMinimal.h"
#include "IScenarioParameter.h"

#include <AmbitUtils/ConfigJsonSerializer.h>

#include "TimeOfDayTypes.generated.h"

//TODO: How to combine this struct with Weather Type
/**
 * Time of Day Types struct
 */
USTRUCT()
struct AMBIT_API FTimeOfDayTypes
#if CPP
        : public IScenarioParameter, public FConfigJsonSerializer
#endif
{
    GENERATED_BODY()

    FTimeOfDayTypes() : bMorning(false), bNoon(false), bEvening(false), bNight(false)
    {
    }

private:
    UPROPERTY(EditAnywhere, Category = TimeOfDayTypes, meta = (DisplayName = "Morning"))
    bool bMorning;
    UPROPERTY(EditAnywhere, Category = TimeOfDayTypes, meta = (DisplayName = "Noon"))
    bool bNoon;
    UPROPERTY(EditAnywhere, Category = TimeOfDayTypes, meta = (DisplayName = "Evening"))
    bool bEvening;
    UPROPERTY(EditAnywhere, Category = TimeOfDayTypes, meta = (DisplayName = "Night"))
    bool bNight;

public:
    // Create an array that represents the enabled variants.
    TArray<FString> EnabledTimeOfDayTypes = {};

    /**
    * @see AmbitUtils/FConfigJsonSerializer.h#SerializeToJson()
     */
    TSharedPtr<FJsonObject> SerializeToJson() const override;

    /**
    * @see AmbitUtils/FConfigJsonSerializer.h#DeserializeFromJson(TSharedPtr<FJsonObject>)
    */
    void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) override;

    /**
     * @see IScenarioParameter#GetVariantCount()
     */
    int32 GetVariantCount() override;

    /**
     * @see IScenarioParameter#ApplyVariant(int32 VariantIndex, FScenarioDefinition& Scenario)
     */
    void ApplyVariant(int32 VariantIndex, FScenarioDefinition& Scenario) override;

    /**
     * Getter and Setter of time of day types
     */
    void SetMorning(bool SelectedMorning);

    bool GetMorning() const;

    void SetNoon(bool SelectedNoon);

    bool GetNoon() const;

    void SetEvening(bool SelectedEvening);

    bool GetEvening() const;

    void SetNight(bool SelectedNight);

    bool GetNight() const;
};
