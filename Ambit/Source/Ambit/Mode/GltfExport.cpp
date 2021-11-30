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

#include "CoreGlobals.h"
#include "Misc/FileHelper.h"
#include "Serialization/BufferArchive.h"

GltfExport::GltfExportReturnCode GltfExport::Export(UWorld* World, const FString& Filename)
{
    // Set the filename for the Exporter to use.
    // This is a global variable used by the UE exporter mechanism and needs to
    // be manually assigned.
    UExporter::CurrentFilename = Filename;

    // Find the exporter plugin object created during UE initialization.
    UGLTFLevelExporter* Exporter = Cast<UGLTFLevelExporter>(UGLTFLevelExporter::StaticClass()->GetDefaultObject(true));
    if (!Exporter)
    {
        // The chances of this failing are extremely low (almost impossible)
        // since the GLTF plugin is a required dependency for the Ambit plugin.
        return ExporterNotFound;
    }

    GltfExportReturnCode ReturnCode = Success;

    // Type, FileIndex, PortFlags are not used by ExportBinary() so they are
    // set to basic values.
    const TCHAR* Type = nullptr;
    const int32 FileIndex = 0;
    const int32 PortFlags = 0;

    // Archive buffer to collect file data and write to file.
    FBufferArchive Buffer;
    if (Exporter->ExportBinary(World, Type, Buffer, GWarn, FileIndex, PortFlags))
    {
        if (!FFileHelper::SaveArrayToFile(Buffer, *Filename))
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
