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

#include "GameFramework/Actor.h"

#include "AmbitWeatherParameters.h"

#include "AmbitWeather.generated.h"

UCLASS(ABSTRACT)
class AAmbitWeather : public AActor
{
    GENERATED_BODY()

public:
    // Updates the time of day and notifies it to the blueprint's event
    UFUNCTION(BlueprintCallable, Category = AmbitWeather)
    void ApplyTimeOfDay(float InTimeOfDay);

    // Updates the weather parameters and notifies it to the blueprint's event
    UFUNCTION(BlueprintCallable, Category = AmbitWeather)
    void ApplyWeather(const FAmbitWeatherParameters& InWeatherParams);

protected:
    // Calls the blueprint event that actually changes the time of day.
    UFUNCTION(BlueprintImplementableEvent)
    void RefreshTimeOfDay(float InTimeOfDay);

    // Calls the blueprint event that actually changes the weather.
    UFUNCTION(BlueprintImplementableEvent)
    void RefreshWeather(const FAmbitWeatherParameters& InWeatherParameters);

private:
    UPROPERTY(VisibleAnywhere, Category = AmbitWeather)
    FAmbitWeatherParameters WeatherParameters;
};
