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

#include "GltfExport.h"

#include "Tests/AutomationEditorCommon.h"

#include "Ambit/Mode/TestClasses/GltfExporterExternalMock.h"

BEGIN_DEFINE_SPEC(GltfExportSpec, "Ambit.Unit.GltfExport",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    FString Filename;
    UWorld* World;
    UGltfExport* Exporter;
    UGltfExporterExternalMock* ExternalExporter;
END_DEFINE_SPEC(GltfExportSpec)

void GltfExportSpec::Define()
{
    Describe("Test glTF export process failure", [this]()
    {
        BeforeEach([this]()
        {
            World = FAutomationEditorCommonUtils::CreateNewMap();

            ExternalExporter = NewObject<UGltfExporterExternalMock>();

            Exporter = NewObject<UGltfExport>();
            Exporter->SetDependencies(ExternalExporter);

            Filename = "";
        });

        It("Should return error if Exporter doesn't exist", [this]()
        {
            ExternalExporter->SetOutputs(false, false, false);

            AddExpectedError("glTF Exporter plugin is not installed.", EAutomationExpectedErrorFlags::Contains);
            Exporter->Export(World, Filename);
        });

        It("Should return error if ExportBinary() fails", [this]()
        {
            ExternalExporter->SetOutputs(true, false, false);

            AddExpectedError("Error completing export to", EAutomationExpectedErrorFlags::Contains);
            Exporter->Export(World, Filename);
        });

         It("Should fail if file name is empty", [this]()
        {
             ExternalExporter->SetOutputs(true, true, false);

             AddExpectedError("Error writing to file", EAutomationExpectedErrorFlags::Contains);
             Exporter->Export(World, Filename);
        });

        AfterEach([this]()
        {
            Exporter = nullptr;
        });
    });

    Describe("Test glTF export process success", [this]()
    {
        BeforeEach([this]()
        {
            World = FAutomationEditorCommonUtils::CreateNewMap();

            ExternalExporter = NewObject<UGltfExporterExternalMock>();
            ExternalExporter->SetOutputs(true, true, true);

            Exporter = NewObject<UGltfExport>();
            Exporter->SetDependencies(ExternalExporter);
        });

        It("Should return success code on completion", [this]()
        {
            const bool IsSuccess = Exporter->Export(World, Filename);
            TestTrue("Export Succeeds", IsSuccess);
        });

        AfterEach([this]()
        {
            Exporter = nullptr;
        });
    });
}
