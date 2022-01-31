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

#include "GltfExport.h"

#include "GLTFExporter/Public/Exporters/GLTFLevelExporter.h"
#include "Misc/FileHelper.h"

#include "Ambit/Mode/GltfExporterExternal.h"

UGltfExportImpl::UGltfExportImpl() : IGltfExport()
{
    Exporter = NewObject<UGltfExporterExternalImpl>();
};

GltfExportReturnCode UGltfExportImpl::Export(UWorld* World, const FString& Filename) const
{
    // Set the filename for the Exporter to use.
    // This is a global variable used by the UE exporter mechanism and needs to
    // be manually assigned.
    UExporter::CurrentFilename = Filename;

    // Find the exporter plugin object created during UE initialization.
    if(!Exporter->DoesExporterExist())
    {
        // The chances of this failing are extremely low (almost impossible)
        // since the GLTF plugin is a required dependency for the Ambit plugin.
        return ExporterNotFound;
    }

    GltfExportReturnCode ReturnCode = Success;

    // Archive buffer to collect file data and write to file.
    FBufferArchive Buffer;
    const bool IsExportSuccess = Exporter->ExportBinary(World, Buffer);
    if (IsExportSuccess)
    {
        const bool IsWriteToFileSuccess = Exporter->WriteToFile(Buffer, *Filename);
        if (!IsWriteToFileSuccess)
        {
            ReturnCode = WriteToFileError;
        }
    }
    else
    {
        // There are two scenarios where the export fails:
        // 1. The user cancels the export process.
        // 2. Objects could not be written to the Buffer used when writing to a
        // file.
        //
        // There is no straightforward way to determine which reason causes the
        // ExportBinary function to fail so a generic failure error code is
        // returned.
        ReturnCode = Failed;
    }

    // Reset the UExporter filename to empty, to not clash with other export
    // mechanisms.
    UExporter::CurrentFilename = TEXT("");

    return ReturnCode;
}

void UGltfExportImpl::SetMockObjects(IGltfExporterExternal* MockExporter)
{
    Exporter = MockExporter;
}
