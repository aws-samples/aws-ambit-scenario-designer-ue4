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

#include "CoreMinimal.h"
#include "AmbitWeatherParameters.generated.h"

USTRUCT(BlueprintType)
struct AMBIT_API FAmbitWeatherParameters
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AmbitWeather, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax =
        "100.0"))
    float Cloudiness = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AmbitWeather, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax =
        "100.0"))
    float Precipitation = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AmbitWeather, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax =
        "100.0"))
    float Puddles = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AmbitWeather, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax =
        "100.0"))
    float Wetness = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AmbitWeather, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax =
        "100.0"))
    float FogDensity = 0.0f;

    FAmbitWeatherParameters() = default;

    FAmbitWeatherParameters(float NewCloudiness, float NewPrecipitation, float NewPuddles,
                            float NewWetness, float NewFogDensity);

    bool operator==(const FAmbitWeatherParameters& WeatherParameters) const;
};

namespace AmbitWeatherParameters
{
    // Get the WeatherParameters by the given Weather Type
    void GetWeatherParametersFromPresetWeatherType(const FString& PresetWeatherType,
                                                   FAmbitWeatherParameters& WeatherParameters);

    // Compare WeatherParameters to get corresponding WeatherType
    FString GetWeatherType(const FAmbitWeatherParameters& WeatherParameters);
}

namespace Weather
{
    static const FString KCustom = "Custom";
    static const FString KRainy = "Rainy";
    static const FString KSunny = "Sunny";
    static const FString KFoggy = "Foggy";

    // Set preset parameters according to the actual effects of the Weather Actor in the viewport
    static const FAmbitWeatherParameters RainyParameters = {90.0, 80.0, 100.0, 50.0, 20.0};
    static const FAmbitWeatherParameters SunnyParameters = {20.0, 0.0, 0.0, 0.0, 0.0};
    static const FAmbitWeatherParameters FoggyParameters = {30.0, 0.0, 0.0, 10.0, 90.0};
}