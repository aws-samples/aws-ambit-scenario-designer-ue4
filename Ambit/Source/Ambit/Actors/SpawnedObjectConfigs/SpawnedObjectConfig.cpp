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

#include "SpawnedObjectConfig.h"

#include "Containers/Map.h"
#include "Dom/JsonObject.h"

#include "Ambit/Mode/Constant.h"

#include <AmbitUtils/JsonHelpers.h>

TSharedPtr<FJsonObject> USpawnedObjectConfig::SerializeToJson() const
{
    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);

    if (SpawnedObjects.Num() > 0)
    {
        TArray<TSharedPtr<FJsonValue>> SpawnTransformsJson;
        TArray<FString> ActorsPaths;
        SpawnedObjects.GenerateKeyArray(ActorsPaths);
        for (const FString& PathName : ActorsPaths)
        {
            const TArray<FTransform>* PathNameTransforms = SpawnedObjects.Find(PathName);
            if (PathNameTransforms != nullptr)
            {
                for (const FTransform& Transform : *PathNameTransforms)
                {
                    const TSharedPtr<FJsonObject> TransformJson = MakeShareable(new FJsonObject);
                    // The unit is centimeter
                    TransformJson->SetStringField(JsonConstants::AmbitSpawner::KActorToSpawnKey, PathName);
                    const FVector& Location = Transform.GetLocation();
                    TransformJson->SetArrayField(JsonConstants::KAmbitSpawnerLocationsKey,
                                                 FJsonHelpers::SerializeVector3(Location));
                    const FRotator& Rotation = Transform.Rotator();
                    TransformJson->SetArrayField(JsonConstants::KAmbitSpawnerRotationsKey,
                                                 FJsonHelpers::SerializeRotation(Rotation));
                    SpawnTransformsJson.Add(MakeShareable(new FJsonValueObject(TransformJson)));
                }
            }
        }

        Json->SetArrayField(JsonConstants::KAmbitSpawnerObjectsKey, SpawnTransformsJson);
    }

    return Json;
}

void USpawnedObjectConfig::DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject)
{
    // Does nothing since this class is not supposed to be deserialized
}
