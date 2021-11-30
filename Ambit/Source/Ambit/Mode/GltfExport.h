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

/**
 * Class dedicated to performing a glTF export using the external GLTFExporter
 * plugin API's.
 */
class GltfExport
{
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
    static GltfExportReturnCode Export(UWorld* World, const FString& Filename);
};
