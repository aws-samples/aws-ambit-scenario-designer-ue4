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

#include "Exporters/GLTFLevelExporter.h"
#include "Serialization/BufferArchive.h"

#include "GltfExporterExternal.generated.h"

UINTERFACE()
class UGltfExporterExternal : public UInterface
{
    GENERATED_BODY()
};

class IGltfExporterExternal
{
    GENERATED_BODY()
public:
    virtual bool DoesExporterExist() = 0;

    virtual bool ExportBinary(UWorld* World, FBufferArchive& Buffer) = 0;

    virtual bool WriteToFile(FBufferArchive& Buffer, const FString& Filename) = 0;
};

UCLASS()
class UGltfExporterExternalImpl : public UObject, public IGltfExporterExternal
{
    GENERATED_BODY()
public:
    UGltfExporterExternalImpl();

    bool DoesExporterExist() override;

    bool ExportBinary(UWorld* World, FBufferArchive& Buffer) override;

    bool WriteToFile(FBufferArchive& Buffer, const FString& Filename) override;

private:
    UGLTFLevelExporter* Exporter;
};
