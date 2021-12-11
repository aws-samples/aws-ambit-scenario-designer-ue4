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

#include "Containers/UnrealString.h"

namespace FileExtensions
{
    const static FString KSDFExtension = ".sdf.json";
    const static FString KBSCExtension = ".bsc.json";
}

namespace AmbitSpawner
{
    const static FName KAmbitCollisionProfileName = "AmbitSpawnedObstacle";
}

namespace JsonConstants
{
    // Common
    const static FString KVersionKey = "Version";

    // Bulk Scenario Configuration
    const static FString KConfigurationNameKey = "ConfigurationName";
    const static FString KBatchNameKey = "BulkScenarioName";
    const static FString KTimeOfDayTypesKey = "TimeOfDayTypes";
    const static FString KWeatherTypesKey = "WeatherTypes";
    const static FString KBatchPedestrianDensityKey = "PedestrianDensity";
    const static FString KBatchTrafficDensityKey = "TrafficDensity";
    const static FString KNumberOfPermutationsKey = "NumberOfPermutations";
    const static FString KAllSpawnersConfigsKey = "AllSpawnersConfigs";
    const static FString KSpawnerSurfaceKey = "AmbitSpawnerSurface";
    const static FString KSpawnerVolumeKey = "AmbitSpawnerVolume";
    const static FString KSpawnerPathKey = "AmbitSpawnerPath";
    const static FString KSpawnerSurfaceHoudiniKey = "AmbitSpawnerSurfaceHoudini";
    const static FString KSpawnerVehiclePathKey = "AmbitSpawnerVehiclePath";
    const static FString KAmbitSpawnerKey = "AmbitSpawner";
    const static FString KAmbitSpawnerObjectsKey = "SpawnedObjects";
    const static FString KAmbitSpawnerLocationsKey = "Location";
    const static FString KAmbitSpawnerRotationsKey = "Rotation";

    // Permutation Object Details
    const static FString KMinKey = "Min";
    const static FString KMaxKey = "Max";
    const static FString KIncrementKey = "Increment";

    // Scenario Definition
    const static FString KScenarioNameKey = "ScenarioName";
    const static FString KPresetLocationKey = "Location";
    const static FString KPresetTimeOfDayKey = "PresetTimeOfDay";
    const static FString KTimeOfDayKey = "TimeOfDay";
    const static FString KPresetWeatherKey = "WeatherType";
    const static FString KWeatherParametersKey = "WeatherParameters";
    const static FString KPedestrianDensityKey = "PedestrianDensity";
    const static FString KTrafficDensityKey = "TrafficDensity";

    namespace AmbitSpawner
    {
        const static FString KSnapToSurfaceBelowKey = "SnapToSurfaceBelow";
        const static FString KMatchByKey = "MatchBy";
        const static FString KSurfaceNamePatternKey = "SurfaceNamePattern";
        const static FString KSurfaceTagsKey = "SurfaceTags";
        const static FString KDensityMinKey = "DensityMin";
        const static FString KDensityMaxKey = "DensityMax";
        const static FString KRotationMinKey = "RotationMin";
        const static FString KRotationMaxKey = "RotationMax";
        const static FString KAddPhysicsKey = "AddPhysics";
        const static FString KActorToSpawnKey = "ActorToSpawn";
        const static FString KActorsToSpawnKey = "ActorsToSpawn";
        const static FString KRemoveOverlapsKey = "RemoveOverlaps";
        const static FString KRandomSeedKey = "RandomSeed";

        const static FString KSpawnerLocationKey = "SpawnerLocation";
        const static FString KSpawnerRotationKey = "SpawnerRotation";

        const static FString KBoxExtentKey = "BoxExtent";

        const static FString KSplinePointsKey = "SplinePoints";
        const static FString KSplinePointRelativeLocationKey = "SplinePointRelativeLocation";
        const static FString KSplinePointRelativeRotationKey = "SplinePointRelativeRotation";
        const static FString KSplinePointScaleKey = "SplinePointRelativeScale";
        const static FString KSplinePointTypeKey = "SplinePointType";
        const static FString KSplinePointInputKey = "SplinePointInputKey";
        const static FString KSplineArriveKey = "SplinePointArriveTangent";
        const static FString KSplineLeaveKey = "SplinePointLeaveTangent";
        const static FString KSplineCloseLoopKey = "CloseLoop";

        const static FString KSplineTypeLinear = "Linear";
        const static FString KSplineTypeCurve = "Curve";
        const static FString KSplineTypeConstant = "Constant";
        const static FString KSplineTypeCurveClamped = "CurveClamped";
        const static FString KSplineTypeCustom = "CurveCustomTangent";
    }

    namespace AmbitPathGenerator
    {
        const static FString KVehicleToSpawnKey = "VehicleToSpawn";
        const static FString KWaypoints = "Waypoints";
        const static FString KSpeedLimit = "SpeedLimit";
        const static FString KShowDebugWaypoints = "ShowDebugWaypoints";
        const static FString KDistanceBetweenWaypoints = "DistanceBetweenWaypoints";
        const static FString KAmbitVehiclePathGenerator = "VehiclePathGenerator";
    }
}

namespace TimeOfDay
{
    static const FString KCustom = "Custom";
    static const FString KMorning = "Morning";
    static const FString KMidDay = "Noon";
    static const FString KEvening = "Evening";
    static const FString KNight = "Night";

    static const float KDefaultMorningSolarTime = 6.0f;
    static const float KDefaultMidDaySolarTime = 12.0f;
    static const float KDefaultEveningSolarTime = 18.0f;
    static const float KDefaultNightSolarTime = 0.0;

    static TMap<FString, float> PresetTimeOfDayToNumber = {
        {KMorning, KDefaultMorningSolarTime},
        {KMidDay, KDefaultMidDaySolarTime},
        {KEvening, KDefaultEveningSolarTime},
        {KNight, KDefaultNightSolarTime}
    };

    static TMap<float, FString> NumberToPresetTimeOfDay = {
        {KDefaultMorningSolarTime, KMorning},
        {KDefaultMidDaySolarTime, KMidDay},
        {KDefaultEveningSolarTime, KEvening},
        {KDefaultNightSolarTime, KNight}
    };
}

namespace ExportPlatform
{
    static const FString KLinux = "LinuxNoEditor";
    static const FString KWindows = "WindowsNoEditor";
}

namespace GltfFileType
{
    static const FString KGltf = "glTF";
    static const FString KGlb = "GLB";
}

namespace AmbitIcon
{
    static const TCHAR* KPath = TEXT("Texture2D'/Ambit/Icons/AmbitIcon.AmbitIcon'");
}

namespace UserMetrics
{
    //Hard coded configurations for AWS Resources
    static const FString KFirehoseRegion = "us-west-2";
    static const FString KCognitoAccountID = "838632552908";
    static const FString KCognitoIdentityPoolID = "us-west-2:7cb8c952-9dfb-49c3-a1da-8825928520f8";
    static const FString KFirehoseStreamName = "AmbitClientMetricsStack-AmbitClientMetricsDeliveryS-Q0BxmvuJrnkQ";
    static const int32 KMaxFirehoseRecordPerBatch = 500;

    static const FString KEventInstanceIDKey = "InstanceID";
    static const FString KEventDateTimeKey = "DateTime";
    static const FString KEventNumberKey = "EventNumber";
    static const FString KEventNameKey = "EventName";
    static const FString KEventNameSpaceKey = "NameSpace";
    static const FString KEventContextDataKey = "ContextData";
    static const FString KEventEngineVersionKey = "EngineVersion";
    static const FString KEventPluginVersionKey = "PluginVersion";

    namespace AmbitGeneral
    {
        static const FString KAmbitGeneralNameSpace = "AmbitGeneral";

        static const FString KAmbitStartEngineEvent = "StartEngine";
    }

    namespace AmbitMode
    {
        static const FString KAmbitModeNameSpace = "AmbitMode";

        static const FString KAmbitModeOpenEvent = "OpenAmbitMode";
        static const FString KAmbitModeCloseEvent = "CloseAmbitMode";

        static const FString KAmbitModeExportSDF = "ExportSDF";

        static const FString KAmbitS3BucketNameContextData = "BucketName";
        static const FString KAmbitUploadBSCEvent = "UploadBSCFiles";

        static const FString KAmbitBulkSDFExportEvent = "ExportBulkSDF";
        static const FString KAmbitBulkSDFExportContextData = "SDFExported";
    }

    namespace AmbitSpawner
    {
        static const FString KAmbitSpawnerNameSpace = "AmbitSpawner";

        static const FString KAmbitSpawnerPlacedEvent = "PlaceAmbitSpawner";
        static const FString KAmbitSpawnerRunEvent = "RunAmbitSpawner";
        static const FString KAmbitSpawnerSpawnNumberContextData = "ActorsSpawned";
    }
}
