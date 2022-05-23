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
#include "Constant.h"
#include "ExportPlatforms.h"
#include "PedestrianTraffic.h"
#include "TimeOfDayTypes.h"
#include "VehicleTraffic.h"
#include "WeatherTypes.h"
#include "UObject/Object.h"

#include <aws/core/Region.h>

#include "AmbitWeatherParameters.h"

// must always be last
#include "AmbitObject.generated.h"

class FAmbitMode;

/**
* This class represents the tool input from the mode menu
*/
UCLASS()
class AMBIT_API UAmbitObject : public UObject
{
    GENERATED_BODY()

public:
    FAmbitMode* ParentMode;

    /**
     * Name of Individual Scenario. Will Default if not Specified.
     */
    UPROPERTY(EditAnywhere, Category = "Scenario Settings")
    FString ScenarioName;

    /**
     * Select Preset Time of Day
     */
    UPROPERTY(EditAnywhere, Category = "Scenario Settings", meta = (DisplayName = "Preset Time of Day"))
    FString PresetTimeOfDay = TimeOfDay::KCustom;

    /**
     * Time of day slider ranging from 0.0 to 23.99999
     */
    UPROPERTY(EditAnywhere, Category = "Scenario Settings",
        meta = (ClampMin = "0.0", ClampMax = "23.99999", UIMin = "0.0", UIMax = "23.99999", DisplayName = "Time of Day"
        ))
    float TimeOfDay = 9.0f;

    /**
     * Select Preset Weather Types
     */
    UPROPERTY(EditAnywhere, Category = "Scenario Settings", meta = (DisplayName = "Weather Type"))
    FString PresetWeather;

    /**
     * Weather Parameters struct
     */
    UPROPERTY(EditAnywhere, Category = "Scenario Settings", meta = (DisplayName = "Weather Parameters"))
    FAmbitWeatherParameters WeatherParameters;

    /**
     * Pedestrian Density [0.0 - 1.0]
     */
    UPROPERTY(EditAnywhere, Category = "Scenario Settings",
        meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float PedestrianDensity;

    /**
     * Vehicle Density [0.0 - 1.0]
     */
    UPROPERTY(EditAnywhere, Category = "Scenario Settings",
        meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float VehicleDensity;

    /**
     * The name of the Bulk Scenario Configuration
     */
    UPROPERTY(EditAnywhere, Category = "Permutation Settings")
    FString ConfigurationName;

    /**
     * The base name of the corresponding Scenario Definition Files
     */
    UPROPERTY(EditAnywhere, Category = "Permutation Settings")
    FString BatchName;

    /**
     * The options of time of day: morning, noon, evening and night
     */
    UPROPERTY(EditAnywhere, Category = "Permutation Settings")
    FTimeOfDayTypes TimeOfDayTypes;

    /**
     * The options of weather: sunny, rainy and foggy
     */
    UPROPERTY(EditAnywhere, Category = "Permutation Settings")
    FWeatherTypes WeatherTypes;

    /**
     * The options of pedestrian traffic density: from 0.0 to 1.0 in increments of 0.1
     */
    UPROPERTY(EditAnywhere, Category = "Permutation Settings")
    FPedestrianTraffic BulkPedestrianTraffic;

    /**
     * The options of vehicle traffic density: from 0.0 to 1.0 in increments of 0.1
     */
    UPROPERTY(EditAnywhere, Category = "Permutation Settings")
    FVehicleTraffic BulkVehicleTraffic;

    /**
     * Number of Scenario Definition Files this Configuration will create.
     */
    UPROPERTY(VisibleAnywhere, Category = "Permutation Settings")
    int NumberOfPermutations = 1;

    /**
     * Platforms to export the map to
     */
    UPROPERTY(EditAnywhere, Category = "Map Export Settings")
    FExportPlatforms ExportPlatforms;

    /**
     * Specifies glTF file type
     */
    UPROPERTY(EditAnywhere, Category = "Map Export Settings", meta = (DisplayName = "glTF File Type"))
    ;
    FString GltfType = GltfFileType::KGltf;

    /**
    * Choose the geographical AWS Region where the Amazon S3 bucket below is located.
    */
    UPROPERTY(EditAnywhere, Category = "AWS Settings", meta = (DisplayName = "AWS Region"))
    FString AwsRegion = Aws::Region::US_EAST_1;

    /**
     * Provide the name of Amazon S3 bucket to save generated permutations files. Please make sure this bucket exists in the Region you choose.
     * Amazon S3 is storage for the Internet. Buckets are the fundamental containers in Amazon S3 for data storage.
     * Amazon S3 can store an infinite amount of data in a S3 bucket.
     * For more information, please access Amazon S3 User Guide https://docs.aws.amazon.com/AmazonS3/latest/userguide/Welcome.html
     */
    UPROPERTY(EditAnywhere, Category = "AWS Settings", meta = (DisplayName = "S3 Bucket Name"))
    FString S3BucketName;

    void SetParent(FAmbitMode* NewParent)
    {
        ParentMode = NewParent;
    }
};
