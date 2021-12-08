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

#include "SpawnInVolumeConfig.h"

#include "Ambit/Mode/Constant.h"

#include <AmbitUtils/JsonHelpers.h>

namespace JsonKeys = JsonConstants::AmbitSpawner;

TSharedPtr<FJsonObject> FSpawnInVolumeConfig::SerializeToJson() const
{
    TSharedPtr<FJsonObject> Json = FSpawnerBaseConfig::SerializeToJson();
    // Serialize bSnapToSurfaceBelow as JSON bool
    Json->SetBoolField(JsonKeys::KSnapToSurfaceBelowKey,
                       bSnapToSurfaceBelow);

    // Serialize BoxExtent as JSON array
    Json->SetArrayField(JsonKeys::KBoxExtentKey,
                        FJsonHelpers::SerializeVector3(BoxExtent));

    return Json;
}


void FSpawnInVolumeConfig::DeserializeFromJson(
    TSharedPtr<FJsonObject> JsonObject)
{
    FSpawnerBaseConfig::DeserializeFromJson(JsonObject);
    bSnapToSurfaceBelow = JsonObject->GetBoolField(JsonKeys::KSnapToSurfaceBelowKey);
    // Configure box extent
    const TArray<TSharedPtr<FJsonValue>>& BoxExtentJson = JsonObject->
            GetArrayField(JsonKeys::KBoxExtentKey);
    BoxExtent = FJsonHelpers::DeserializeToVector3(BoxExtentJson);
}
