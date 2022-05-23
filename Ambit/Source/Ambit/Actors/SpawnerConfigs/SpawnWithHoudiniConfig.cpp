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

#include "SpawnWithHoudiniConfig.h"

#include "Dom/JsonObject.h"

#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/MatchBy.h"

#include <AmbitUtils/JsonHelpers.h>

namespace JsonKeys = JsonConstants::AmbitSpawner;

TSharedPtr<FJsonObject> FSpawnWithHoudiniConfig::SerializeToJson() const
{
    // Returns Json with SpawnerLocation and SpawnerRotation serialized
    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);

    // Serialize location and rotation of spawner as JSON arrays
    Json->SetArrayField(JsonKeys::KSpawnerLocationKey, FJsonHelpers::SerializeVector3(SpawnerLocation));
    Json->SetArrayField(JsonKeys::KSpawnerRotationKey, FJsonHelpers::SerializeRotation(SpawnerRotation));

    // Serialize MatchByValue as JSON string
    const FString& MatchByValue = MatchBy == EMatchBy::NameOrTags ? "NameOrTags" : "NameAndTags";
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

    // Serialize RandomSeed as JSON number
    Json->SetNumberField(JsonKeys::KRandomSeedKey, RandomSeed);

    return Json;
}

void FSpawnWithHoudiniConfig::DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject)
{
    // Configure location of spawner
    const TArray<TSharedPtr<FJsonValue>>& LocationJson = JsonObject->GetArrayField(JsonKeys::KSpawnerLocationKey);
    SpawnerLocation = FJsonHelpers::DeserializeToVector3(LocationJson);

    // Configure rotation of spawner
    const TArray<TSharedPtr<FJsonValue>>& RotationJson = JsonObject->GetArrayField(JsonKeys::KSpawnerRotationKey);
    SpawnerRotation = FJsonHelpers::DeserializeToRotation(RotationJson);

    // Configure MatchBy property.
    const FString& MatchByString = JsonObject->GetStringField(JsonKeys::KMatchByKey);
    MatchBy = MatchByString == "NameOrTags" ? EMatchBy::NameOrTags : EMatchBy::NameAndTags;

    // Configure SurfaceNamePattern property.
    SurfaceNamePattern = JsonObject->GetStringField(JsonKeys::KSurfaceNamePatternKey);

    // Configure SurfaceTags property.
    SurfaceTags.Empty();
    for (const TSharedPtr<FJsonValue>& JsonValue : JsonObject->GetArrayField(JsonKeys::KSurfaceTagsKey))
    {
        FString TagName = JsonValue->AsString();
        SurfaceTags.Add(*TagName);
    }

    // Configure density properties.
    DensityMin = JsonObject->GetNumberField(JsonKeys::KDensityMinKey);
    DensityMax = JsonObject->GetNumberField(JsonKeys::KDensityMaxKey);

    // Configure RandomSeed property.
    RandomSeed = JsonObject->GetNumberField(JsonKeys::KRandomSeedKey);
}
