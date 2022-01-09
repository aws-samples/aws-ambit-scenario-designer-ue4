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

#include "Ambit/Mode/TestClasses/MockableGltfExport.h"

BEGIN_DEFINE_SPEC(GltfExportSpec, "Ambit.GltfExport",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)
    FString Filename;
    UMockableGltfExport* Exporter;
    UWorld* World;
END_DEFINE_SPEC(GltfExportSpec)

void GltfExportSpec::Define()
{
    Describe("Test glTF export process failure", [this]()
    {
        BeforeEach([this]()
        {
            World = FAutomationEditorCommonUtils::CreateNewMap();

            Exporter = NewObject<UMockableGltfExport>();
        });

        Describe("Test GLTFExporter plugin failure", [this]()
        {
            BeforeEach([this]()
            {
                auto MockFunction = [](UGLTFLevelExporter* LevelExporter, UWorld* World, FBufferArchive& Buffer) -> bool
                {
                    return false;
                };
                Exporter->SetExportBinary(MockFunction);
            });

            It("Should return error if ExportBinary() fails", [this]()
            {
                UGltfExport::GltfExportReturnCode ReturnCode = Exporter->Export(World, Filename);

                TestEqual("Export should fail", ReturnCode, UGltfExport::Failed);
            });
        });

        Describe("Test write to file", [this]()
        {
            BeforeEach([this]()
            {
                auto MockBinaryFunction = [](UGLTFLevelExporter* LevelExporter, UWorld* World, FBufferArchive& Buffer) -> bool
                {
                    return true;
                };
                Exporter->SetExportBinary(MockBinaryFunction);

                auto MockWriteFunction = [](FBufferArchive& Buffer, const FString& Filename) -> bool
                {
                    return false;
                };
                Exporter->SetWriteToFile(MockWriteFunction);
            });

            It("Should fail if file name is empty", [this]()
            {
                UGltfExport::GltfExportReturnCode ReturnCode = Exporter->Export(World, Filename);

                TestEqual("Write to file should fail", ReturnCode, UGltfExport::WriteToFileError);
            });
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

            Exporter = NewObject<UMockableGltfExport>();

            auto MockBinaryFunction = [](UGLTFLevelExporter* LevelExporter, UWorld* World, FBufferArchive& Buffer) -> bool
            {
                return true;
            };
            Exporter->SetExportBinary(MockBinaryFunction);

            auto MockWriteFunction = [](FBufferArchive& Buffer, const FString& Filename) -> bool
            {
                return true;
            };
            Exporter->SetWriteToFile(MockWriteFunction);
        });

        It("Should return success code on completion", [this]()
        {
            UGltfExport::GltfExportReturnCode ReturnCode = Exporter->Export(World, Filename);

            TestEqual("Export should complete successfully", ReturnCode, UGltfExport::Success);
        });

        AfterEach([this]()
        {
            Exporter = nullptr;
        });
    });
}
