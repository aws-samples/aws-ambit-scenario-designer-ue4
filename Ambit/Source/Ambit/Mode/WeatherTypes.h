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

#pragma once
#include "Constant.h"
#include "IScenarioParameter.h"
#include <AmbitUtils/ConfigJsonSerializer.h>

#include "WeatherTypes.generated.h"

/**
 * Weather Types struct
 */
USTRUCT()
struct AMBIT_API FWeatherTypes
#if CPP
    : public IScenarioParameter, public FConfigJsonSerializer
#endif
{
    GENERATED_BODY()

    FWeatherTypes()
        : bSunny(false)
          , bRainy(false)
          , bFoggy(false)
    {
    }

private:
    UPROPERTY(EditAnywhere, Category = WeatherTypes, meta = (DisplayName = "Sunny"))
    bool bSunny;
    UPROPERTY(EditAnywhere, Category = WeatherTypes, meta = (DisplayName = "Rainy"))
    bool bRainy;
    UPROPERTY(EditAnywhere, Category = WeatherTypes, meta = (DisplayName = "Foggy"))
    bool bFoggy;

public:
    // Create an array that represents the enabled variants.
    TArray<FString> EnabledWeatherTypes = {};

    /**
    * @see AmbitUtils/UConfigJsonSerializer.h#SerializeToJson()
     */
    TSharedPtr<FJsonObject> SerializeToJson() const override;

    /**
    * @see AmbitUtils/UConfigJsonSerializer.h#DeserializeFromJson(TSharedPtr<FJsonObject>)
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


    // Getter and Setter of weather types
    void SetSunny(bool);

    bool GetSunny() const;

    void SetRainy(bool);

    bool GetRainy() const;

    void SetFoggy(bool);

    bool GetFoggy() const;
};
