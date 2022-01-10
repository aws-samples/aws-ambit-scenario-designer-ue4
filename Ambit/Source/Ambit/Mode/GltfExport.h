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

#include "GltfExport.generated.h"

/**
 * Class dedicated to performing a glTF export using the external GLTFExporter
 * plugin API's.
 */
UCLASS()
class UGltfExport : public UObject
{
    GENERATED_BODY()
public:
    /**
     * Return codes for GLTF Export.
     */
    enum GltfExportReturnCode
    {
        ExporterNotFound,
        WriteToFileError,
        Failed,
        Success
    };

    /**
     * Performs the export of the whole scene.
     *
     * @param World A UObject containing objects to be added to the export.
     * @param Filename The file (gltf or glb) to be written.
     *
     * @return GltfExportReturnCode An enum value describing the return state.
     */
    GltfExportReturnCode Export(UWorld* World, const FString& Filename) const;

protected:
    /**
     * Calls ExportBinary from the GLTF Exporter plugin
     * Allows for injection of the function to be changed. Should only be used in testing.
     */
    TFunction<bool(UGLTFLevelExporter* Exporter, UWorld* World, FBufferArchive& Buffer)> LambdaExportBinary = [this](
        UGLTFLevelExporter* Exporter, UWorld* World, FBufferArchive& Buffer)-> bool
    {
        return this->ExportBinary(Exporter, World, Buffer);
    };

    /**
     * Calls a function to write Buffer contents to a file.
     * Allows for injection of the function to be changed. Should only be used in testing.
     */
    TFunction<bool(FBufferArchive& Buffer, const FString& Filename)>
    LambdaWriteToFile = [this](FBufferArchive& Buffer, const FString& Filename) -> bool
    {
        return this->WriteToFile(Buffer, Filename);
    };

private:
    /**
     * Calls ExportBinary() from the GLTF Exporter plugin object.
     *
     * @param Exporter The GLTFExporter plugin object.
     * @param World UObject containing geometry to be exported.
     * @param Buffer Stores details to be written to file.
     *
     * @return True if export process succeeds and object details written to buffer.
     */
    bool ExportBinary(UGLTFLevelExporter* Exporter, UWorld* World, FBufferArchive& Buffer);

    /**
     * Writes buffer data to a file.
     *
     * @param Buffer Data to be written to a file.
     * @param Filename The file to be written.
     *
     * @return True if write to file succeeds.
     */
    bool WriteToFile(FBufferArchive& Buffer, const FString& Filename) const;
};
