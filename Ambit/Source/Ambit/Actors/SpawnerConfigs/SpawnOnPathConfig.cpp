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

#include "SpawnOnPathConfig.h"

#include "Components/SplineComponent.h"

#include "Ambit/AmbitModule.h"
#include "Ambit/Mode/Constant.h"

#include <AmbitUtils/JsonHelpers.h>

namespace JsonKeys = JsonConstants::AmbitSpawner;

TSharedPtr<FJsonObject> FSpawnOnPathConfig::SerializeToJson() const
{
    TSharedPtr<FJsonObject> Json = FSpawnerBaseConfig::SerializeToJson();
    // Serialize bSnapToSurfaceBelow as JSON bool
    Json->SetBoolField(JsonKeys::KSnapToSurfaceBelowKey, bSnapToSurfaceBelow);

    // Make sure there are spline points to serialize, and that spline point information
    // was configured correctly (create Null JSON field if otherwise)
    if (SplinePoints.Num() > 0)
    {
        // Serialize spline point transforms and types as array of JSON objects
        TArray<TSharedPtr<FJsonValue>> SplinePointsJson;
        int32 i = 0;
        for (const FSplinePoint& Point : SplinePoints)
        {
            const TSharedPtr<FJsonObject> PointJson = MakeShareable(new FJsonObject);

            PointJson->SetNumberField(JsonKeys::KSplinePointInputKey, Point.InputKey);

            const FVector& SplinePointLocation = Point.Position;
            PointJson->SetArrayField(JsonKeys::KSplinePointRelativeLocationKey,
                                     FJsonHelpers::SerializeVector3(SplinePointLocation));

            const FVector& SplinePointArriveTangent = Point.ArriveTangent;
            PointJson->SetArrayField(JsonKeys::KSplineArriveKey,
                                     FJsonHelpers::SerializeVector3(SplinePointArriveTangent));

            const FVector& SplinePointLeaveTangent = Point.LeaveTangent;
            PointJson->SetArrayField(JsonKeys::KSplineLeaveKey,
                                     FJsonHelpers::SerializeVector3(SplinePointLeaveTangent));

            const FRotator& SplinePointRotation = Point.Rotation;
            PointJson->SetArrayField(JsonKeys::KSplinePointRelativeRotationKey,
                                     FJsonHelpers::SerializeRotation(SplinePointRotation));

            const FVector& SplinePointScale = Point.Scale;
            PointJson->SetArrayField(JsonKeys::KSplinePointScaleKey, FJsonHelpers::SerializeVector3(SplinePointScale));

            // Serialize spline point type as JSON string
            FString SplinePointTypeString;
            const TEnumAsByte<ESplinePointType::Type> SplinePointType = Point.Type;
            if (SplinePointType == ESplinePointType::Linear)
            {
                SplinePointTypeString = JsonKeys::KSplineTypeLinear;
            }
            else if (SplinePointType == ESplinePointType::Curve)
            {
                SplinePointTypeString = JsonKeys::KSplineTypeCurve;
            }
            else if (SplinePointType == ESplinePointType::Constant)
            {
                SplinePointTypeString = JsonKeys::KSplineTypeConstant;
            }
            else if (SplinePointType == ESplinePointType::CurveClamped)
            {
                SplinePointTypeString = JsonKeys::KSplineTypeCurveClamped;
            }
            else if (SplinePointType == ESplinePointType::CurveCustomTangent)
            {
                SplinePointTypeString = JsonKeys::KSplineTypeCustom;
            }
            PointJson->SetStringField(JsonKeys::KSplinePointTypeKey, SplinePointTypeString);
            SplinePointsJson.Add(MakeShareable(new FJsonValueObject(PointJson)));
        }
        Json->SetArrayField(JsonKeys::KSplinePointsKey, SplinePointsJson);
    }
    else
    {
        UE_LOG(LogAmbit, Warning, TEXT("No spline point data to serialize."));
        Json->SetField(JsonKeys::KSplinePointsKey, MakeShareable(new FJsonValueNull));
    }
    return Json;
}

void FSpawnOnPathConfig::DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject)
{
    FSpawnerBaseConfig::DeserializeFromJson(JsonObject);
    bSnapToSurfaceBelow = JsonObject->GetBoolField(JsonKeys::KSnapToSurfaceBelowKey);

    //Configure spline points
    SplinePoints.Empty();
    FVector SplinePointLocation(0, 0, 0);
    FVector ArriveTangent(0, 0, 0);
    FVector LeaveTangent(0, 0, 0);
    FRotator SplinePointRotation(0, 0, 0);
    FVector SplinePointScale(0, 0, 0);
    TEnumAsByte<ESplinePointType::Type> SplinePointType = ESplinePointType::Curve;
    const TArray<TSharedPtr<FJsonValue>>* SplinePointsJson;
    if (JsonObject->TryGetArrayField(JsonKeys::KSplinePointsKey, SplinePointsJson))
    {
        for (const TSharedPtr<FJsonValue>& JsonValue : *SplinePointsJson)
        {
            const TSharedPtr<FJsonObject>& PointObject = JsonValue->AsObject();

            const float SplinePointInputKey = PointObject->GetNumberField(JsonKeys::KSplinePointInputKey);

            const TArray<TSharedPtr<FJsonValue>>& SplinePointLocationJson = PointObject->GetArrayField(
                JsonKeys::KSplinePointRelativeLocationKey);
            SplinePointLocation = FJsonHelpers::DeserializeToVector3(SplinePointLocationJson);

            const TArray<TSharedPtr<FJsonValue>>& ArriveTangentJson = PointObject->GetArrayField(
                JsonKeys::KSplineArriveKey);
            ArriveTangent = FJsonHelpers::DeserializeToVector3(ArriveTangentJson);

            const TArray<TSharedPtr<FJsonValue>>& LeaveTangentJson = PointObject->GetArrayField(
                JsonKeys::KSplineLeaveKey);
            LeaveTangent = FJsonHelpers::DeserializeToVector3(LeaveTangentJson);

            const TArray<TSharedPtr<FJsonValue>>& SplinePointRotationJson = PointObject->GetArrayField(
                JsonKeys::KSplinePointRelativeRotationKey);
            SplinePointRotation = FJsonHelpers::DeserializeToRotation(SplinePointRotationJson);

            const TArray<TSharedPtr<FJsonValue>>& SplinePointScaleJson = PointObject->GetArrayField(
                JsonKeys::KSplinePointScaleKey);
            SplinePointScale = FJsonHelpers::DeserializeToVector3(SplinePointScaleJson);

            const FString& SplinePointTypeString = PointObject->GetStringField(JsonKeys::KSplinePointTypeKey);

            if (SplinePointTypeString.Equals(JsonKeys::KSplineTypeLinear))
            {
                SplinePointType = ESplinePointType::Linear;
            }
            else if (SplinePointTypeString.Equals(JsonKeys::KSplineTypeCurve))
            {
                SplinePointType = ESplinePointType::Curve;
            }
            else if (SplinePointTypeString.Equals(JsonKeys::KSplineTypeConstant))
            {
                SplinePointType = ESplinePointType::Constant;
            }
            else if (SplinePointTypeString.Equals(JsonKeys::KSplineTypeCurveClamped))
            {
                SplinePointType = ESplinePointType::CurveClamped;
            }
            else if (SplinePointTypeString.Equals(JsonKeys::KSplineTypeCustom))
            {
                SplinePointType = ESplinePointType::CurveCustomTangent;
            }

            const FSplinePoint Point(SplinePointInputKey, SplinePointLocation, ArriveTangent, LeaveTangent,
                                     SplinePointRotation, SplinePointScale, SplinePointType);
            SplinePoints.Add(Point);
        }
    }
    else
    {
        UE_LOG(LogAmbit, Warning, TEXT("No spline point data found in JSON."));
    }
}
