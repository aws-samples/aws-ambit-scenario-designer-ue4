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

#include "GltfExporterExternalInterface.generated.h"

class FBufferArchive;
class FString;
class UWorld;

/**
 * Interface class for GltfExporter and GltfExporterMock.
 * This class acts as a wrapper to the external GLTF Exporter plugin so it can be mocked for unit tests.
 */
UINTERFACE()
class UGltfExporterExternalInterface : public UInterface
{
    GENERATED_BODY()
};

class IGltfExporterExternalInterface
{
    GENERATED_BODY()
public:
    /**
     * Checks whether the external exporter object exists.
     *
     * @return True If it exists.
     */
    virtual bool DoesExporterExist() = 0;

    /**
     * Forwards the call to the external exporter plugin.
     *
     * @param World The contents to be exported.
     * @param Buffer The buffer to write to.
     *
     * @return True If the process succeeds.
     */
    virtual bool ExportBinary(UWorld* World, FBufferArchive& Buffer) = 0;

    /**
     * Forwards the call to write details to file.
     *
     * @param Buffer The contents to be written to a file.
     * @param Filename The file to be written to.
     *
     * @param True If the process succeeds.
     */
    virtual bool WriteToFile(FBufferArchive& Buffer, const FString& Filename) = 0;
};
