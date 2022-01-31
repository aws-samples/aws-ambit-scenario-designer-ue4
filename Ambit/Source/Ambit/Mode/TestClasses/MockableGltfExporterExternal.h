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

#include <utility>

#include "Ambit/Mode/GltfExporterExternal.h"

#include "MockableGltfExporterExternal.generated.h"

/**
 * Mock class for GltfExport.h
 * To be used only during testing.
 */
UCLASS()
class UGltfExporterExternalMock : public UObject, public IGltfExporterExternal
{
    GENERATED_BODY()
public:
    bool DoesExporterExist() override
    {
        return bExporterExists;
    }

    bool ExportBinary(UWorld* World, FBufferArchive& Buffer) override
    {
        return bExportResult;
    }

    bool WriteToFile(FBufferArchive& Buffer, const FString& Filename) override
    {
        return bWriteResult;
    }

    void SetOutputs(bool ExporterExists, bool ExportResult, bool WriteResult)
    {
        bExporterExists = ExporterExists;
        bExportResult = ExportResult;
        bWriteResult = WriteResult;
    }

private:
    bool bExporterExists;
    bool bExportResult;
    bool bWriteResult;
};
