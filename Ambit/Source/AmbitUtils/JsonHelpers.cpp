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

#include "JsonHelpers.h"

#include <stdexcept>

#include "MenuHelpers.h"
#include "Serialization/JsonSerializer.h"

namespace FJsonHelpers
{
    FString SerializeJson(TSharedPtr<FJsonObject> JsonObject)
    {
        typedef TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>> FStringWriter;
        typedef TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>
            FStringWriterFactory;

        FString OutputString;
        const TSharedRef<FStringWriter> Writer = FStringWriterFactory::Create(
            &OutputString);
        if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
        {
            return "";
        }

        return OutputString;
    }

    FString SerializeJsonCondense(TSharedPtr<FJsonObject> JsonObject)
    {
        typedef TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> FStringWriter;
        typedef TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>
            FStringWriterFactory;

        FString OutputString;
        const TSharedRef<FStringWriter> Writer = FStringWriterFactory::Create(
            &OutputString);
        if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
        {
            return "";
        }

        return OutputString;
    }

    TSharedPtr<FJsonObject> DeserializeJson(const FString& JsonString)
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(
            JsonString);

        if (!FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            return nullptr;
        }

        return JsonObject;
    }

    FVector DeserializeToVector3(const TArray<TSharedPtr<FJsonValue>>& JsonValues)
    {
        FVector Vector;
        try
        {
            if (JsonValues.Num() != 3) throw JsonValues.Num();
            Vector.X = JsonValues[0]->AsNumber();
            Vector.Y = JsonValues[1]->AsNumber();
            Vector.Z = JsonValues[2]->AsNumber();
            return Vector;
        }
        catch (int32 n)
        {
            FString Message =
                "Instead of size 3, the JSON array is of size ";
            Message.AppendInt(n);
            FMenuHelpers::LogErrorAndPopup(Message);
        }
        return Vector;
    }

    FRotator DeserializeToRotation(const TArray<TSharedPtr<FJsonValue>>& JsonValues)
    {
        FRotator Rotation;
        try
        {
            if (JsonValues.Num() != 3) throw JsonValues.Num();
            Rotation.Pitch = JsonValues[0]->AsNumber();
            Rotation.Yaw = JsonValues[1]->AsNumber();
            Rotation.Roll = JsonValues[2]->AsNumber();
            return Rotation;
        }
        catch (const int32 n)
        {
            FString Message =
                "Instead of size 3, the JSON array is of size ";
            Message.AppendInt(n);
            FMenuHelpers::LogErrorAndPopup(Message);

        }
        return Rotation;
    }

    TArray<TSharedPtr<FJsonValue>> SerializeVector3(const FVector& Vector)
    {
        TArray<TSharedPtr<FJsonValue>> JsonValues;
        JsonValues.Add(MakeShareable(new FJsonValueNumber(Vector.X)));
        JsonValues.Add(MakeShareable(new FJsonValueNumber(Vector.Y)));
        JsonValues.Add(MakeShareable(new FJsonValueNumber(Vector.Z)));
        return JsonValues;
    }

    TArray<TSharedPtr<FJsonValue>> SerializeRotation(const FRotator& Rotation)
    {
        TArray<TSharedPtr<FJsonValue>> JsonValues;
        JsonValues.Add(MakeShareable(new FJsonValueNumber(Rotation.Pitch)));
        JsonValues.Add(MakeShareable(new FJsonValueNumber(Rotation.Yaw)));
        JsonValues.Add(MakeShareable(new FJsonValueNumber(Rotation.Roll)));
        return JsonValues;
    }
}
