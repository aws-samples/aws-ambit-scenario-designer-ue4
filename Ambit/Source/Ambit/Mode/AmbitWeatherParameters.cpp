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

FAmbitWeatherParameters::FAmbitWeatherParameters(float NewCloudiness, float NewPrecipitation,
                                                 float NewPuddles, float NewWetness, float NewFogDensity)
    : Cloudiness(NewCloudiness)
      , Precipitation(NewPrecipitation)
      , Puddles(NewPuddles)
      , Wetness(NewWetness)
      , FogDensity(NewFogDensity)
{
}

bool FAmbitWeatherParameters::operator==(const FAmbitWeatherParameters& WeatherParameters) const
{
    return (Cloudiness == WeatherParameters.Cloudiness) &&
        (Precipitation == WeatherParameters.Precipitation) &&
        (Puddles == WeatherParameters.Puddles) &&
        (Wetness == WeatherParameters.Wetness) &&
        (FogDensity == WeatherParameters.FogDensity);
}

void AmbitWeatherParameters::GetWeatherParametersFromPresetWeatherType(const FString& WeatherType,
                                                                       FAmbitWeatherParameters& WeatherParams)
{
    if (WeatherType == Weather::KRainy)
    {
        WeatherParams.Cloudiness = 90;
        WeatherParams.Precipitation = 80;
        WeatherParams.Puddles = 100;
        WeatherParams.FogDensity = 20;
        WeatherParams.Wetness = 50;
    }
    else if (WeatherType == Weather::KSunny)
    {
        WeatherParams.Cloudiness = 20;
        WeatherParams.Precipitation = 0;
        WeatherParams.Puddles = 0;
        WeatherParams.FogDensity = 0;
        WeatherParams.Wetness = 0;
    }
    else if (WeatherType == Weather::KFoggy)
    {
        WeatherParams.Cloudiness = 30;
        WeatherParams.Precipitation = 0;
        WeatherParams.Puddles = 0;
        WeatherParams.FogDensity = 90;
        WeatherParams.Wetness = 10;
    }
}

FString AmbitWeatherParameters::GetWeatherType(const FAmbitWeatherParameters& WeatherParams)
{
    if (WeatherParams == Weather::RainyParameters)
    {
        return Weather::KRainy;
    }
    if (WeatherParams == Weather::SunnyParameters)
    {
        return Weather::KSunny;
    }
    if (WeatherParams == Weather::FoggyParameters)
    {
        return Weather::KFoggy;
    }
    return Weather::KCustom;
}
