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
#include "UObject/Interface.h"

#include "Ambit/Mode/Constant.h"

#include "ConfigJsonSerializer.generated.h"

class FJsonObject;

/**
 * Used as a base interface for any object that is JsonSerializable
 */
UINTERFACE(BlueprintType)
class AMBITUTILS_API UConfigJsonSerializer : public UInterface
{
    GENERATED_BODY()
};

class AMBITUTILS_API IConfigJsonSerializer
{
    GENERATED_BODY()
public:
    /**
     * Serialize the class information into a Json object
     */
    virtual TSharedPtr<FJsonObject> SerializeToJson() const
    {
        return nullptr;
    }

    /**
    * Deserialize the Json object into Unreal Engine editor
    */
    virtual void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject)
    {
    };

    /**
     * Retrieves the Configuration Name for the Json to be serialized.
     *
     * @return An FString of the object that is meant to be the key for the serialization of it. 
     */
    virtual FString GetOutputConfigurationName() const
    {
        return JsonConstants::KAmbitSpawnerKey;
    }
};

/**
 * Struct implementation of IConfigJsonSerializer.
 */
struct AMBITUTILS_API FConfigJsonSerializer
{
    virtual ~FConfigJsonSerializer() = default;

    /**
     * Serialize the struct information into a Json object
     */
    virtual TSharedPtr<FJsonObject> SerializeToJson() const = 0;

    /**
    * Deserialize the Json object into Unreal Engine editor
    */
    virtual void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) = 0;
};
