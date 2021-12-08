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
#include "Dom/JsonObject.h"

/**
 * This namespace contains helper functions to deal with Json Serialization and Deserialization.
 */
namespace FJsonHelpers
{
    /**
     * Serialize the JsonObject to a readable string.
     *
     * @return
     *  an empty string if unable to parse JsonObject
     */
    AMBITUTILS_API FString SerializeJson(const TSharedPtr<FJsonObject>& JsonObject);

    /**
     * Serialize the JsonObject to a condensed string.
     *
     * @return
     *  an empty string if unable to parse JsonObject
     */
    AMBITUTILS_API FString SerializeJsonCondense(const TSharedPtr<FJsonObject>& JsonObject);

    /**
     * Deserialize a readable string into a JsonObject
     *
     * @return
     *  a nullptr if unable to parse JsonString.
     */
    AMBITUTILS_API TSharedPtr<FJsonObject> DeserializeJson(
        const FString& JsonString);

    /**
     * Serialize a Vector3 to an array of JsonValue
     *
     * @param
     *  an FVector
     * @return
     *  a JSON array containing three numeric values. Example, [0.0, 5.2, 101].
     */
    AMBITUTILS_API TArray<TSharedPtr<FJsonValue>> SerializeVector3(
        const FVector& Vector);

    /**
    * Deserialize an array of JsonValue to a Vector3
    *
    * @param JsonValues
    *  a JSON array containing three numeric values. Example, [0.0, 5.2, 101]
    * @return
    *  an FVector
    *  throws an exception and pops an error message if the JSON array is not size 3
    */
    AMBITUTILS_API FVector DeserializeToVector3(
        const TArray<TSharedPtr<FJsonValue>>& JsonValues);

    /**
     * Serialize a Rotator to an array of JsonValue
     *
     * @param
     *  an FRotator
     * @return
     *  a JSON array containing three numeric values representing pitch, yaw, and roll respectively,
     *  expressed in degrees.
     *  Example: [90.0, 45.0, 0]
     */
    AMBITUTILS_API TArray<TSharedPtr<FJsonValue>> SerializeRotation(
        const FRotator& Rotation);

    /**
     * Deserialize an array of JsonValue to a Rotator
     *
     * @param
     *  a JSON array containing three numeric values representing pitch, yaw, and roll respectively,
     *  expressed in degrees.
     *  Example: [90.0, 45.0, 0]
     * @return
     *  an FRotator
     *  throws an exception and pops an error message if the JSON array is not size 3
     */
    AMBITUTILS_API FRotator DeserializeToRotation(
        const TArray<TSharedPtr<FJsonValue>>& JsonValues);
}
