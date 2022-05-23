//   Copyright 2022 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include "GltfExporterExternal.h"

#include "Misc/FileHelper.h"

UGltfExporterExternal::UGltfExporterExternal()
{
    Exporter = Cast<UGLTFLevelExporter>(UGLTFLevelExporter::StaticClass()->GetDefaultObject(true));
}

bool UGltfExporterExternal::DoesExporterExist()
{
    return Exporter ? true : false;
}

bool UGltfExporterExternal::ExportBinary(UWorld* World, FBufferArchive& Buffer)
{
    // Type, FileIndex, PortFlags are not used by ExportBinary() so they are
    // set to basic values.
    const TCHAR* Type = nullptr;
    const int32 FileIndex = 0;
    const int32 PortFlags = 0;

    return Exporter->ExportBinary(World, Type, Buffer, GWarn, FileIndex, PortFlags);
}

bool UGltfExporterExternal::WriteToFile(FBufferArchive& Buffer, const FString& Filename)
{
    return FFileHelper::SaveArrayToFile(Buffer, *Filename);
}
