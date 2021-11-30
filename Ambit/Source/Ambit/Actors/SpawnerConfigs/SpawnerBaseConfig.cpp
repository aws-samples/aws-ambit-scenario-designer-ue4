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

#include "SpawnerBaseConfig.h"

#include "Ambit/AmbitModule.h"
#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/MatchBy.h"
#include "Dom/JsonObject.h"
#include <AmbitUtils/JsonHelpers.h>

namespace JsonKeys = JsonConstants::AmbitSpawner;

TSharedPtr<FJsonObject> FSpawnerBaseConfig::SerializeToJson() const
{
    // Returns Json with SpawnerLocation and SpawnerRotation serialized
    // as well as BoxExtent (if SpawnerType is Bounding)
    // or SplinePoints (if SpawnerType is Spline)
    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);

    // Serialize location and rotation of spawner as JSON arrays
    Json->SetArrayField(JsonKeys::KSpawnerLocationKey,
        FJsonHelpers::SerializeVector3(SpawnerLocation));
    Json->SetArrayField(JsonKeys::KSpawnerRotationKey,
        FJsonHelpers::SerializeRotation(SpawnerRotation));

    // Serialize MatchByValue as JSON string
    const FString& MatchByValue = MatchBy == EMatchBy::NameOrTags
                                     ? "NameOrTags"
                                     : "NameAndTags";
    Json->SetStringField(JsonKeys::KMatchByKey, MatchByValue);

    // Serialize SurfaceNamePattern as JSON string
    Json->SetStringField(JsonKeys::KSurfaceNamePatternKey, SurfaceNamePattern);

    // Serialize SurfaceTags as JSON array of strings
    TArray<TSharedPtr<FJsonValue>> TagsJson;
    for (const FName& SurfaceTag : SurfaceTags)
    {
        const FString& TagAsString = SurfaceTag.ToString();
        TagsJson.Add(MakeShareable(new FJsonValueString(TagAsString)));
    }
    Json->SetArrayField(JsonKeys::KSurfaceTagsKey, TagsJson);

    // Serialize DensityMin as JSON number
    Json->SetNumberField(JsonKeys::KDensityMinKey, DensityMin);

    // Serialize DensityMax as JSON number
    Json->SetNumberField(JsonKeys::KDensityMaxKey, DensityMax);

    // Serialize RotationMin as JSON number
    Json->SetNumberField(JsonKeys::KRotationMinKey, RotationMin);

    // Serialize RotationMax as JSON number
    Json->SetNumberField(JsonKeys::KRotationMaxKey, RotationMax);

    // Serialize bAddPhysics as JSON bool
    Json->SetBoolField(JsonKeys::KAddPhysicsKey, bAddPhysics);

    // Serialize ActorsToSpawn as JSON string array
    if (ActorsToSpawn.Num() > 0 && !ActorsToSpawn.Contains(nullptr))
    {
        TArray<TSharedPtr<FJsonValue>> ActorsJson;
        for (const TSubclassOf<AActor>& Actor : ActorsToSpawn)
        {
            const FString& PathName = Actor->GetPathName();
            ActorsJson.Add(MakeShareable(
                new FJsonValueString(PathName)));
            
        }
        if (ActorsJson.Num() > 0)
        {
            Json->SetArrayField(JsonKeys::KActorsToSpawnKey, ActorsJson);
        }
        else
        {
            Json->SetField(JsonKeys::KActorsToSpawnKey,
                MakeShareable(new FJsonValueNull));
        }
    }
    else
    {
        if (ActorsToSpawn.Contains(nullptr))
        {
            UE_LOG(LogAmbit, Warning,
                TEXT("An element of the ActorsToSpawn set is not specified."));
        }
        Json->SetField(JsonKeys::KActorsToSpawnKey,
            MakeShareable(new FJsonValueNull));
    }

    // Serialize RemoveOverlaps as a JSON bool
    Json->SetBoolField(JsonKeys::KRemoveOverlapsKey, bRemoveOverlaps);

    // Serialize RandomSeed as JSON number
    Json->SetNumberField(JsonKeys::KRandomSeedKey, RandomSeed);

    return Json;
}

void FSpawnerBaseConfig::DeserializeFromJson(
    TSharedPtr<FJsonObject> JsonObject)
{
    // Configure location of spawner
    const TArray<TSharedPtr<FJsonValue>>& LocationJson = JsonObject->
        GetArrayField(
            JsonKeys::KSpawnerLocationKey);
    SpawnerLocation = FJsonHelpers::DeserializeToVector3(LocationJson);

    // Configure rotation of spawner
    const TArray<TSharedPtr<FJsonValue>>& RotationJson = JsonObject->
        GetArrayField(
            JsonKeys::KSpawnerRotationKey);
    SpawnerRotation = FJsonHelpers::DeserializeToRotation(RotationJson);

    // Configure MatchBy property.
    const FString& MatchByString = JsonObject->GetStringField(
        JsonKeys::KMatchByKey);
    MatchBy = MatchByString == "NameOrTags"
                  ? EMatchBy::NameOrTags
                  : EMatchBy::NameAndTags;

    // Configure SurfaceNamePattern property.
    SurfaceNamePattern = JsonObject->GetStringField(
        JsonKeys::KSurfaceNamePatternKey);

    // Configure SurfaceTags property.
    SurfaceTags.Empty();
    for (const TSharedPtr<FJsonValue>& JsonValue : JsonObject->GetArrayField(
             JsonKeys::KSurfaceTagsKey))
    {
        FString TagName = JsonValue->AsString();
        SurfaceTags.Add(*TagName);
    }

    // Configure density properties.
    DensityMin = JsonObject->GetNumberField(JsonKeys::KDensityMinKey);
    DensityMax = JsonObject->GetNumberField(JsonKeys::KDensityMaxKey);

    // Configure rotation properties
    RotationMin = JsonObject->GetNumberField(JsonKeys::KRotationMinKey);
    RotationMax = JsonObject->GetNumberField(JsonKeys::KRotationMaxKey);

    // Configure AddPhysics
    bAddPhysics = JsonObject->GetBoolField(JsonKeys::KAddPhysicsKey);

    // Configure actors to spawn
    ActorsToSpawn.Empty();
    const TArray<TSharedPtr<FJsonValue>>* ActorsPaths;
    if (JsonObject->TryGetArrayField("ActorsToSpawn", ActorsPaths))
    {
        for (const TSharedPtr<FJsonValue>& PathName : *ActorsPaths)
        {
            const FSoftClassPath ClassPath(PathName->AsString());
            const auto& Actor = ClassPath.TryLoadClass<UObject>();
            if (!Actor)
            {
                UE_LOG(LogAmbit, Error, TEXT("%s is not a valid path."),
                    *(PathName->AsString()))
            }
            else
            {
                if (ActorsToSpawn.Contains(Actor))
                {
                    UE_LOG(LogAmbit, Warning, TEXT("ActorsToSpawn contains duplicates"));
                }
                else
                {
                    ActorsToSpawn.Add(Actor);
                }
            }
        }
    }

    // Configure RemoveOverlaps property.
    bRemoveOverlaps = JsonObject->GetBoolField(JsonKeys::KRemoveOverlapsKey);

    // Configure RandomSeed property.
    RandomSeed = JsonObject->GetNumberField(JsonKeys::KRandomSeedKey);
}
