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

#pragma once

#include "Ambit/Mode/GltfExportInterface.h"

#include "GltfExport.generated.h"

class IGltfExporterExternalInterface;

/**
 * Class dedicated to performing a glTF export using the external GLTFExporter
 * plugin API's.
 */
UCLASS()
class UGltfExport : public UObject, public IGltfExportInterface
{
    GENERATED_BODY()
public:
    UGltfExport();

    /** @inheritDoc */
    bool Export(UWorld* World, const FString& Filename) const override;

    /**
     * Assign new dependency instance for use by this GltfExport object.
     *
     * @param Exporter The glTF Exporter to be used.
     */
    void SetDependencies(IGltfExporterExternalInterface* Exporter);

private:
    IGltfExporterExternalInterface* ExternalExporter;
};
