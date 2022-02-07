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

#include "GltfExportInterface.generated.h"

/**
 * Interface for GltfExport and GltfExportMock
 */
UINTERFACE()
class UGltfExportInterface : public UInterface
{
    GENERATED_BODY()
};

class IGltfExportInterface
{
    GENERATED_BODY()
public:
    /**
     * Performs the export of the whole scene.
     *
     * @param World A UObject containing objects to be added to the export.
     * @param Filename The file (gltf or glb) to be written.
     *
     * @return True If export succeeds.
     */
    virtual bool Export(UWorld* World, const FString& Filename) const = 0;
};
