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

#include "ConfigImportExport.h"

#include "EditorModeManager.h"
#include "Dom/JsonObject.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"

#include <stdexcept>

#include "AmbitObject.h"
#include "Ambit/Actors/Spawners/SpawnInVolume.h"
#include "Ambit/Actors/Spawners/SpawnOnPath.h"
#include "Ambit/Actors/Spawners/SpawnOnSurface.h"
#include "Ambit/Actors/Spawners/SpawnWithHoudini.h"
#include "Ambit/Actors/Spawners/TestClasses/MockableSpawner.h"
#include "Ambit/Mode/AmbitMode.h"
#include "Ambit/Mode/TestClasses/MockableConfigImportExport.h"
#include "AmbitUtils/JsonHelpers.h"

BEGIN_DEFINE_SPEC(ConfigImportExportSpec, "Ambit.Unit.ConfigImportExport",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

    TSharedPtr<FJsonObject> Json;

    UWorld* World;
    AStaticMeshActor* SurfaceActor;
    FString SurfaceActorName;
    FString Tag;
    UMockableConfigImportExport* Exporter;

    FString JsonContent;

END_DEFINE_SPEC(ConfigImportExportSpec)

class UAmbitObject;

void ConfigImportExportSpec::Define()
{
    Describe("OnExportSdf()", [this]()
    {
        BeforeEach([this]()
        {
            // Create Mode Settings
            GLevelEditorModeTools().ActivateMode(FAmbitMode::EM_AmbitModeId);

            // Generate World
            World = FAutomationEditorCommonUtils::CreateNewMap();

            // Generate Exporter
            Exporter = NewObject<UMockableConfigImportExport>();

            auto MockWrite = [](const FString& FilePath, const FString& OutString) -> bool
            {
                return true;
            };
            auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                const FString& Filename) -> FString
            {
                return "Test";
            };

            Exporter->SetMockGetPathFromPopup(MockPopup);
            Exporter->SetMockWriteFile(MockWrite);
        });

        It("Should Return FReply::Handled", [this]()
        {
            const FReply Reply = Exporter->OnExportSdf();

            TestTrue("Reply should be true on return", Reply.IsEventHandled());
        });

        AfterEach([this]()
        {
            GLevelEditorModeTools().DeactivateMode(FAmbitMode::EM_AmbitModeId);
            Exporter = nullptr;
            JsonContent = "";
        });
    });

    Describe("DequeueOrDefaultNextSdfConfigToProcess()", [this]()
    {
        BeforeEach([this]()
        {
            // Create Mode Settings
            GLevelEditorModeTools().ActivateMode(FAmbitMode::EM_AmbitModeId);

            // Generate World
            World = FAutomationEditorCommonUtils::CreateNewMap();

            // Generate Exporter
            Exporter = NewObject<UMockableConfigImportExport>();
        });

        Describe("When DequeueOrDefaultNextSdfConfigToProcess() Defaults", [this]()
        {
            BeforeEach([this]()
            {
                // Generate Exporter
                Exporter = NewObject<UMockableConfigImportExport>();

                const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
                AmbitMode->UISettings->TimeOfDay = 0.1;
                AmbitMode->UISettings->PedestrianDensity = 0.2;
                AmbitMode->UISettings->VehicleDensity = 0.3;

                auto MockWrite = [this](const FString& FilePath, const FString& OutString) -> bool
                {
                    JsonContent = *OutString;
                    return true;
                };
                auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                    const FString& Filename) -> FString
                {
                    return "Test";
                };

                Exporter->SetMockGetPathFromPopup(MockPopup);
                Exporter->SetMockWriteFile(MockWrite);
            });

            It("Should Give On Screen Values (Name Default)", [this]()
            {
                const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
                AmbitMode->UISettings->ScenarioName = "";
                const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;

                Exporter->ProcessSdfForExport(FakeArray, false);

                TestTrue("Json contains Mode's Time of Day", JsonContent.Contains("\"TimeOfDay\": 0.1"));
                TestTrue("Json contains Mode's Pedestrian Density", JsonContent.Contains("\"PedestrianDensity\": 0.2"));
                TestTrue("Json contains Mode's Vehicle Density", JsonContent.Contains("\"TrafficDensity\": 0.3"));
                TestTrue("Json contains Default Scenario Name",
                         JsonContent.Contains("\"ScenarioName\": \"AmbitScenario\""));
            });

            It("Should Give On Screen Values (Name Specified)", [this]()
            {
                const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
                AmbitMode->UISettings->ScenarioName = "FakeName";
                const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;

                Exporter->ProcessSdfForExport(FakeArray, false);

                TestTrue("Json contains input scenario came", JsonContent.Contains("\"ScenarioName\": \"FakeName\""));
            });
        });

        AfterEach([this]()
        {
            GLevelEditorModeTools().DeactivateMode(FAmbitMode::EM_AmbitModeId);
            Exporter = nullptr;
            JsonContent = nullptr;
        });
    });

    Describe("SerializeSpawnerConfigs()", [this]()
    {
        BeforeEach([this]()
        {
            // Create Mode Settings
            GLevelEditorModeTools().ActivateMode(FAmbitMode::EM_AmbitModeId);

            // Generate World
            World = FAutomationEditorCommonUtils::CreateNewMap();

            // Generate Exporter
            Exporter = NewObject<UMockableConfigImportExport>();

            auto MockWrite = [this](const FString& FilePath, const FString& OutString) -> bool
            {
                JsonContent = *OutString;
                return true;
            };
            auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                const FString& Filename) -> FString
            {
                return "Test";
            };
            auto MockWriteToS3 = [](const FString& Region, const FString& BucketName, const FString& ObjectName,
                                    const FString& Content) -> bool
            {
                return true;
            };

            Exporter->SetMockGetPathFromPopup(MockPopup);
            Exporter->SetMockWriteFile(MockWrite);
            Exporter->SetMockPutObjectS3(MockWriteToS3);
        });

        Describe("When Spawners Exists", [this]()
        {
            BeforeEach([this]()
            {
                // ProcessSdfForExport Current Manually looks for Spawners, so we need to use a their instances for testing
                // Replace with AMockableSpawner after unifying this process
                World->SpawnActor<ASpawnOnSurface>();
                World->SpawnActor<ASpawnOnPath>();
                World->SpawnActor<ASpawnInVolume>();
                World->SpawnActor<ASpawnWithHoudini>();
            });

            It("Should Contain Spawners in Output", [this]()
            {
                const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;

                Exporter->ProcessSdfForExport(FakeArray, false);

                const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                const TSharedPtr<FJsonObject>* Spawners;
                const bool FoundField = ConvertedJson->TryGetObjectField(
                    JsonConstants::KAllSpawnersConfigsKey, Spawners);

                TestTrue("Spawners should be contained in the output", FoundField);

                if (FoundField)
                {
                    TestTrue("Surface Spawner should exist in SDF",
                             Spawners->Get()->HasField(JsonConstants::KSpawnerSurfaceKey));
                    TestTrue("Path Spawner should exist in SDF",
                             Spawners->Get()->HasField(JsonConstants::KSpawnerPathKey));
                    TestTrue("Volume Spawner should exist in SDF",
                             Spawners->Get()->HasField(JsonConstants::KSpawnerVolumeKey));
                    TestTrue("Houdini Spawner should exist in SDF",
                             Spawners->Get()->HasField(JsonConstants::KSpawnerSurfaceHoudiniKey));
                }
            });

            It("Should Condense the Same Spawner Type Down to One Array", [this]()
            {
                World->SpawnActor<ASpawnOnSurface>();
                World->SpawnActor<ASpawnOnSurface>();
                const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;

                Exporter->ProcessSdfForExport(FakeArray, false);
                const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                const TArray<TSharedPtr<FJsonValue>>* OutputArray = {};
                const TSharedPtr<FJsonObject> Spawners = ConvertedJson->GetObjectField(
                    JsonConstants::KAllSpawnersConfigsKey);
                if (Spawners)
                {
                    Spawners->TryGetArrayField(JsonConstants::KSpawnerSurfaceKey, OutputArray);
                }

                TestEqual("Surface Spawner be an array of 3", OutputArray->Num(), 3);
            });
        });

        Describe("When Spawners Do Not Exist", [this]()
        {
            It("Should Have the AllSpawnersConfigs Object Inserted", [this]()
            {
                const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;

                Exporter->ProcessSdfForExport(FakeArray, false);
                const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                const TSharedPtr<FJsonObject>* Spawners;
                const bool FoundField = ConvertedJson->TryGetObjectField(
                    JsonConstants::KAllSpawnersConfigsKey, Spawners);

                TestTrue("Spawner Config object should exist", FoundField);
            });
        });

        AfterEach([this]()
        {
            GLevelEditorModeTools().DeactivateMode(FAmbitMode::EM_AmbitModeId);
            Exporter = nullptr;
            JsonContent = nullptr;
        });
    });

    Describe("ProcessSdfForExport()", [this]()
    {
        BeforeEach([this]()
        {
            // Create Mode Settings
            GLevelEditorModeTools().ActivateMode(FAmbitMode::EM_AmbitModeId);

            // Generate Exporter
            Exporter = NewObject<UMockableConfigImportExport>();

            // Generate World
            World = FAutomationEditorCommonUtils::CreateNewMap();
        });

        Describe("Serialize SpawnedObjectsConfig", [this]()
        {
            BeforeEach([this]()
            {
                // Generate Exporter
                Exporter = NewObject<UMockableConfigImportExport>();

                auto MockWrite = [this](const FString& FilePath, const FString& OutString) -> bool
                {
                    JsonContent = *OutString;
                    return true;
                };
                auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                    const FString& Filename) -> FString
                {
                    return "Test";
                };

                Exporter->SetMockGetPathFromPopup(MockPopup);
                Exporter->SetMockWriteFile(MockWrite);
            });

            Describe("When Spawned Objects Exists", [this]()
            {
                It("Should Contain Spawned Objects in Output (Single)", [this]()
                {
                    const FString Key = "Testing";
                    const FString ValueString = "{\"Test\": 123}";
                    const TSharedPtr<FJsonObject> ValueJson = FJsonHelpers::DeserializeJson(ValueString);
                    TMap<FString, TSharedPtr<FJsonObject>> TestSpawnedObjects;
                    TestSpawnedObjects.Add(Key, ValueJson);

                    Exporter->ProcessSdfForExport(TestSpawnedObjects, false);

                    const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                    const TSharedPtr<FJsonObject>* SpawnedItem;
                    const bool FoundField = ConvertedJson->TryGetObjectField("Testing", SpawnedItem);

                    TestTrue("Spawned item should be contained in the output", FoundField);
                });

                It("Should Contain Spawned Objects in Output (Multiple)", [this]()
                {
                    const FString Key1 = "Testing";
                    const FString ValueString1 = "{\"Test\": 123}";
                    const TSharedPtr<FJsonObject> ValueJson1 = FJsonHelpers::DeserializeJson(ValueString1);

                    const FString Key2 = "FakeItem";
                    const FString ValueString2 = "{\"Fake\": false}";
                    const TSharedPtr<FJsonObject> ValueJson2 = FJsonHelpers::DeserializeJson(ValueString1);

                    TMap<FString, TSharedPtr<FJsonObject>> TestSpawnedObjects;
                    TestSpawnedObjects.Add(Key1, ValueJson1);
                    TestSpawnedObjects.Add(Key2, ValueJson2);

                    Exporter->ProcessSdfForExport(TestSpawnedObjects, false);

                    const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                    const TSharedPtr<FJsonObject>* SpawnedItem1;
                    const bool FoundField1 = ConvertedJson->TryGetObjectField("Testing", SpawnedItem1);
                    const TSharedPtr<FJsonObject>* SpawnedItem2;
                    const bool FoundField2 = ConvertedJson->TryGetObjectField("Testing", SpawnedItem2);

                    TestTrue("Spawned items should be contained in the output", FoundField1);
                    TestTrue("Spawned items should be contained in the output", FoundField2);
                });

                It("Should Contain Spawned Object Details", [this]()
                {
                    const FString Key = "Testing";
                    const FString ValueString = "\"Test\": 123";
                    const TSharedPtr<FJsonObject> ValueJson = FJsonHelpers::DeserializeJson(ValueString);
                    TMap<FString, TSharedPtr<FJsonObject>> TestSpawnedObjects;
                    TestSpawnedObjects.Add(Key, ValueJson);

                    Exporter->ProcessSdfForExport(TestSpawnedObjects, false);

                    const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                    const TSharedPtr<FJsonObject>* SpawnedItem;
                    const bool FoundField = ConvertedJson->TryGetObjectField("Testing", SpawnedItem);

                    if (FoundField)
                    {
                        int OutputItem;
                        const bool FoundSubField = SpawnedItem->Get()->TryGetNumberField("Test", OutputItem);
                        TestTrue("Key is valid", FoundSubField);
                        TestEqual("Value matches expected value", OutputItem, 123);
                    }
                });
            });
        });

        AfterEach([this]()
        {
            GLevelEditorModeTools().DeactivateMode(FAmbitMode::EM_AmbitModeId);
            Exporter = nullptr;
            JsonContent = nullptr;
        });
    });

    Describe("WriteJsonFile()", [this]()
    {
        BeforeEach([this]()
        {
            // Create Mode Settings
            GLevelEditorModeTools().ActivateMode(FAmbitMode::EM_AmbitModeId);

            // Generate World
            World = FAutomationEditorCommonUtils::CreateNewMap();

            // Generate Exporter
            Exporter = NewObject<UMockableConfigImportExport>();

            auto MockWrite = [this](const FString& FilePath, const FString& OutString) -> bool
            {
                JsonContent = *OutString;
                return true;
            };

            Exporter->SetMockWriteFile(MockWrite);

            auto MockListBuckets = []() -> TSet<FString>
            {
                TSet<FString> setBuckets;
                setBuckets.Add("BucketName");
                return setBuckets;
            };

            Exporter->SetMockS3ListBuckets(MockListBuckets);

            auto MockCreateBucket = [](const FString& Region, const FString& BucketName) -> void
            {
                return;
            };

            Exporter->SetMockS3CreateBucket(MockCreateBucket);
        });

        Describe("When Writing to Disk", [this]()
        {
            Describe("When a File Path is not Specified from Popup", [this]()
            {
                It("Should not Write to Disk", [this]()
                {
                    auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                        const FString& Filename) -> FString
                    {
                        return "";
                    };
                    Exporter->SetMockGetPathFromPopup(MockPopup);

                    const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                    const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, false);

                    TestFalse("No write should occur", bWroteSuccess);
                });
            });
        });

        Describe("When Writing to Amazon S3", [this]()
        {
            Describe("When a Bucket is Not Valid", [this]()
            {
                BeforeEach([this]()
                {
                    const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();

                    AmbitMode->UISettings->AwsRegion = "";
                    AmbitMode->UISettings->S3BucketName = "";
                    AmbitMode->UISettings->BatchName = "";
                });

                It("Should Return False and Not Write", [this]()
                {
                    bool bHitS3 = false;
                    auto MockWriteToS3 = [&bHitS3](const FString& Region, const FString& BucketName,
                                                   const FString& ObjectName, const FString& Content) -> bool
                    {
                        bHitS3 = true;
                        return true;
                    };
                    Exporter->SetMockPutObjectS3(MockWriteToS3);

                    auto MockCreateBucket = [](const FString& Region, const FString& BucketName) -> void
                    {
                        throw std::invalid_argument("The bucket name or region is empty");
                    };
                    Exporter->SetMockS3CreateBucket(MockCreateBucket);

                    // Must be set before we call the function.
                    AddExpectedError("The bucket name or region is empty", EAutomationExpectedErrorFlags::Contains, 1);

                    const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                    const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, true);

                    TestFalse("Amazon S3 should not be written to", bHitS3);
                    TestFalse("Process should return failure", bWroteSuccess);
                });
            });

            Describe("When a Bucket is Valid", [this]()
            {
                BeforeEach([this]()
                {
                    // Generate Exporter
                    Exporter = NewObject<UMockableConfigImportExport>();

                    const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
                    AmbitMode->UISettings->AwsRegion = Aws::Region::US_EAST_1;
                    AmbitMode->UISettings->S3BucketName = "test--config-import-export--process-sdf-for-export";
                    AmbitMode->UISettings->BatchName = "AmbitTest" + FDateTime::UtcNow().ToString() +
                            "ProcessSdfForExport";
                });

                Describe("When an Error Occurs During Put", [this]()
                {
                    It("Should Catch and Log Error (Invalid Argument)", [this]()
                    {
                        auto MockWriteToS3 = [](const FString& Region, const FString& BucketName,
                                                const FString& ObjectName, const FString& Content) -> bool
                        {
                            throw std::invalid_argument("Test Fail");
                        };
                        Exporter->SetMockPutObjectS3(MockWriteToS3);

                        // Must be set before we call the function.
                        AddExpectedError("WriteJsonFile failed with invalid argument error: Test Fail",
                                         EAutomationExpectedErrorFlags::Exact, 1);

                        const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                        const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, true);

                        TestFalse("Process should return failure", bWroteSuccess);
                    });

                    It("Should Catch and Log Error (Runtime Argument)", [this]()
                    {
                        auto MockWriteToS3 = [](const FString& Region, const FString& BucketName,
                                                const FString& ObjectName, const FString& Content) -> bool
                        {
                            throw std::runtime_error("Test Fail");
                        };
                        Exporter->SetMockPutObjectS3(MockWriteToS3);

                        // Must be set before we call the function.
                        AddExpectedError("WriteJsonFile failed with runtime error: Test Fail",
                                         EAutomationExpectedErrorFlags::Exact, 1);

                        const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                        const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, true);

                        TestFalse("Process should return failure", bWroteSuccess);
                    });

                    It("Should Throw Other Error To Caller", [this]()
                    {
                        auto MockWriteToS3 = [](const FString& Region, const FString& BucketName,
                                                const FString& ObjectName, const FString& Content) -> bool
                        {
                            throw std::domain_error("Test Fail");
                        };
                        Exporter->SetMockPutObjectS3(MockWriteToS3);

                        const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                        bool bHitError = false;
                        try
                        {
                            const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, true);
                        }
                        catch (std::domain_error test)
                        {
                            bHitError = true;
                        }

                        TestTrue("Error should throw to caller", bHitError);
                    });
                });

                It("Should Use Default Configuration Name When No Name is Set", [this]()
                {
                    const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
                    AmbitMode->UISettings->ConfigurationName = "";

                    FString SpecifiedBscConfigName;
                    auto MockWriteToS3 = [&SpecifiedBscConfigName](const FString& Region, const FString& BucketName,
                                                                   const FString& ObjectName,
                                                                   const FString& Content) -> bool
                    {
                        SpecifiedBscConfigName = ObjectName;
                        return true;
                    };
                    Exporter->SetMockPutObjectS3(MockWriteToS3);

                    const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                    const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, true);

                    TestTrue("Should write successfully", bWroteSuccess);
                    TestTrue("Name should contain default name",
                             SpecifiedBscConfigName.Contains("AmbitScenarioConfiguration"));
                });

                It("Should Use Specified Configuration Name When Name is Set", [this]()
                {
                    const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
                    AmbitMode->UISettings->ConfigurationName = "ConfigImportExportUnitTest";

                    FString SpecifiedBscConfigName;
                    auto MockWriteToS3 = [&SpecifiedBscConfigName](const FString& Region, const FString& BucketName,
                                                                   const FString& ObjectName,
                                                                   const FString& Content) -> bool
                    {
                        SpecifiedBscConfigName = ObjectName;
                        return true;
                    };
                    Exporter->SetMockPutObjectS3(MockWriteToS3);

                    const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                    const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, true);

                    TestTrue("Should write successfully", bWroteSuccess);
                    TestTrue("Name should contain default name",
                             SpecifiedBscConfigName.Contains("ConfigImportExportUnitTest"));
                });

                It("Should Create A Path for SDF Configuration", [this]()
                {
                    const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
                    AmbitMode->UISettings->ConfigurationName = "ConfigImportExportUnitTest";
                    AmbitMode->UISettings->ScenarioName = "ConfigTest";

                    FString ActualPathName;
                    auto MockWriteToS3 = [&ActualPathName](const FString& Region, const FString& BucketName,
                                                           const FString& ObjectName, const FString& Content) -> bool
                    {
                        ActualPathName = ObjectName;
                        return true;
                    };
                    Exporter->SetMockPutObjectS3(MockWriteToS3);

                    const TMap<FString, TSharedPtr<FJsonObject>> FakeArray;
                    const bool bWroteSuccess = Exporter->ProcessSdfForExport(FakeArray, true);

                    const FString ExpectedOutputName = FPaths::Combine(
                        "GeneratedScenarios-" + AmbitMode->UISettings->ConfigurationName,
                        AmbitMode->UISettings->ScenarioName + ".sdf.json");
                    TestTrue("Should write successfully", bWroteSuccess);
                    TestEqual("File Name should be expected path", ActualPathName, ExpectedOutputName);
                });
            });
        });

        AfterEach([this]()
        {
            GLevelEditorModeTools().DeactivateMode(FAmbitMode::EM_AmbitModeId);
            Exporter = nullptr;
            JsonContent = nullptr;
        });
    });

    Describe("PrepareAllSpawnersObjectConfigs()", [this]()
    {
        BeforeEach([this]()
        {
            // Create Mode Settings
            GLevelEditorModeTools().ActivateMode(FAmbitMode::EM_AmbitModeId);

            // Generate Exporter
            Exporter = NewObject<UMockableConfigImportExport>();

            auto MockWrite = [this](const FString& FilePath, const FString& OutString) -> bool
            {
                JsonContent = *OutString;
                return true;
            };
            auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                const FString& Filename) -> FString
            {
                return "Test";
            };
            auto MockWriteToS3 = [](const FString& Region, const FString& BucketName, const FString& ObjectName,
                                    const FString& Content) -> bool
            {
                return true;
            };

            Exporter->SetMockGetPathFromPopup(MockPopup);
            Exporter->SetMockWriteFile(MockWrite);
            Exporter->SetMockPutObjectS3(MockWriteToS3);
        });

        Describe("When there are no items", [this]()
        {
            It("Should call ProcessSdfForExport() Immediately", [this]()
            {
                Exporter->OnExportSdf();

                TestFalse("Config should be not empty", JsonContent.IsEmpty());
            });
        });

        Describe("When there are items", [this]()
        {
            BeforeEach([this]()
            {
                // Generate World
                World = FAutomationEditorCommonUtils::CreateNewMap();

                // Generate Exporter
                Exporter = NewObject<UMockableConfigImportExport>();

                auto MockWrite = [this](const FString& FilePath, const FString& OutString) -> bool
                {
                    JsonContent = *OutString;
                    return true;
                };
                auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                    const FString& Filename) -> FString
                {
                    return "Test";
                };
                auto MockWriteToS3 = [](const FString& Region, const FString& BucketName, const FString& ObjectName,
                                        const FString& Content) -> bool
                {
                    return true;
                };

                Exporter->SetMockGetPathFromPopup(MockPopup);
                Exporter->SetMockWriteFile(MockWrite);
                Exporter->SetMockPutObjectS3(MockWriteToS3);
            });

            LatentBeforeEach([this](FDoneDelegate const& Done)
            {
                // Generate items
                auto* Spawner = World->SpawnActor<AMockableSpawner>();
                auto HasSpawnedActors = []() -> bool
                {
                    return true;
                };
                Spawner->LambdaHasActorToSpawn = HasSpawnedActors;

                auto SpawnedConfig = [Spawner]()
                {
                    USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();
                    TArray<FTransform> TransformArray;
                    const FTransform CurrentTransform = Spawner->GetTransform();
                    TransformArray.Add(CurrentTransform);
                    Config->SpawnedObjects.Add("Test", TransformArray);

                    auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);

                    Spawner->EmitConfigCompleted(FinalConfig, true);
                };
                Spawner->LambdaGenerateSpawnedObjectConfiguration = SpawnedConfig;

                Exporter->SetSdfProcessDone(Done);
                Exporter->OnExportSdf();
            });

            It("Should Call ProcessSdfForExport() Asynchronously With Actors", [this]()
            {
                const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                const TSharedPtr<FJsonObject>* AmbitSpawnerSection;
                const bool FoundField = ConvertedJson->TryGetObjectField(
                    JsonConstants::KAmbitSpawnerKey, AmbitSpawnerSection);

                TestTrue("Spawned items should be contained in the output", FoundField);

                const TArray<TSharedPtr<FJsonValue>>* OutputArray = {};
                if (FoundField)
                {
                    const bool FoundItems = AmbitSpawnerSection->Get()->TryGetArrayField(
                        JsonConstants::KAmbitSpawnerObjectsKey, OutputArray);
                }

                TestEqual("Spawned Object count should be consistent", OutputArray->Num(), 1);
                TestTrue("Contains Expected Field", JsonContent.Contains("\"ActorToSpawn\": \"Test\","));
            });

            It("Should Contain the Spawned Actor Information", [this]()
            {
                const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);

                TestTrue("Contains Expected Field", JsonContent.Contains("\"ActorToSpawn\": \"Test\","));
            });
        });

        AfterEach([this]()
        {
            GLevelEditorModeTools().DeactivateMode(FAmbitMode::EM_AmbitModeId);
            Exporter = nullptr;
            JsonContent = nullptr;
        });
    });

    Describe("SpawnedObjectConfigCompleted_Handler()", [this]()
    {
        BeforeEach([this]()
        {
            // Create Mode Settings
            GLevelEditorModeTools().ActivateMode(FAmbitMode::EM_AmbitModeId);

            // Generate World
            World = FAutomationEditorCommonUtils::CreateNewMap();

            // Generate Exporter
            Exporter = NewObject<UMockableConfigImportExport>();

            auto MockWrite = [this](const FString& FilePath, const FString& OutString) -> bool
            {
                JsonContent = *OutString;
                return true;
            };
            auto MockPopup = [](const FString& FileExtension, const FString& DefaultPath,
                                const FString& Filename) -> FString
            {
                return "Test";
            };
            auto MockWriteToS3 = [](const FString& Region, const FString& BucketName, const FString& ObjectName,
                                    const FString& Content) -> bool
            {
                return true;
            };

            Exporter->SetMockGetPathFromPopup(MockPopup);
            Exporter->SetMockWriteFile(MockWrite);
            Exporter->SetMockPutObjectS3(MockWriteToS3);
        });

        Describe("On Spawner Failure To Generate", [this]()
        {
            It("Should Generate Error", [this]()
            {
                // Generate items
                auto* Spawner = World->SpawnActor<AMockableSpawner>();
                auto HasSpawnedActors = []() -> bool
                {
                    return true;
                };
                Spawner->LambdaHasActorToSpawn = HasSpawnedActors;

                auto SpawnedConfig = [Spawner]()
                {
                    USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();
                    auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);

                    Spawner->EmitConfigCompleted(FinalConfig, false);
                };
                Spawner->LambdaGenerateSpawnedObjectConfiguration = SpawnedConfig;

                UAmbitExporterDelegateWatcher* ConfigurationDelegateWatcher = NewObject<
                    UAmbitExporterDelegateWatcher>();
                ConfigurationDelegateWatcher->SpawnerCount = 1;
                ConfigurationDelegateWatcher->bSendToS3 = false;
                ConfigurationDelegateWatcher->Parent = Exporter;

                Spawner->GetOnSpawnedObjectConfigCompletedDelegate().BindUObject(
                    ConfigurationDelegateWatcher, &UAmbitExporterDelegateWatcher::SpawnedObjectConfigCompleted_Handler);

                // Assert
                AddExpectedError("One of the SDF configurations have failed to generate properly",
                                 EAutomationExpectedErrorFlags::Exact, 1);

                Spawner->GenerateSpawnedObjectConfiguration();
            });
        });

        Describe("On Spawner Success", [this]()
        {
            Describe("With Single Spawner", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Exporter->SetSdfProcessDone(Done);

                    // Generate items
                    auto* Spawner = World->SpawnActor<AMockableSpawner>();
                    auto HasSpawnedActors = []() -> bool
                    {
                        return true;
                    };
                    Spawner->LambdaHasActorToSpawn = HasSpawnedActors;

                    auto SpawnedConfig = [Spawner]()
                    {
                        USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();
                        TArray<FTransform> TransformArray;
                        const FTransform CurrentTransform = Spawner->GetTransform();
                        TransformArray.Add(CurrentTransform);
                        Config->SpawnedObjects.Add("Test", TransformArray);

                        auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);

                        Spawner->EmitConfigCompleted(FinalConfig, true);
                    };
                    Spawner->LambdaGenerateSpawnedObjectConfiguration = SpawnedConfig;

                    UAmbitExporterDelegateWatcher* ConfigurationDelegateWatcher = NewObject<
                        UAmbitExporterDelegateWatcher>();
                    ConfigurationDelegateWatcher->SpawnerCount = 1;
                    ConfigurationDelegateWatcher->bSendToS3 = false;
                    ConfigurationDelegateWatcher->Parent = Exporter;

                    Spawner->GetOnSpawnedObjectConfigCompletedDelegate().BindUObject(
                        ConfigurationDelegateWatcher,
                        &UAmbitExporterDelegateWatcher::SpawnedObjectConfigCompleted_Handler);

                    Spawner->GenerateSpawnedObjectConfiguration();
                });

                It("Should Generate Config from ProcessSdfForExport (Single)", [this]()
                {
                    TestTrue("JSON Contains expected test field", JsonContent.Contains("\"ActorToSpawn\": \"Test\","));
                });
            });

            Describe("With Multiple Spawners", [this]()
            {
                LatentBeforeEach([this](FDoneDelegate const& Done)
                {
                    Exporter->SetSdfProcessDone(Done);

                    // Generate items
                    auto* Spawner = World->SpawnActor<AMockableSpawner>();
                    auto HasSpawnedActors = []() -> bool
                    {
                        return true;
                    };
                    Spawner->LambdaHasActorToSpawn = HasSpawnedActors;

                    auto SpawnedConfig = [Spawner]()
                    {
                        USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();
                        TArray<FTransform> TransformArray;
                        const FTransform CurrentTransform = Spawner->GetTransform();
                        TransformArray.Add(CurrentTransform);
                        Config->SpawnedObjects.Add("Test", TransformArray);

                        auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);

                        Spawner->EmitConfigCompleted(FinalConfig, true);
                    };
                    Spawner->LambdaGenerateSpawnedObjectConfiguration = SpawnedConfig;

                    auto* Spawner2 = World->SpawnActor<AMockableSpawner>();
                    Spawner2->LambdaHasActorToSpawn = HasSpawnedActors;

                    auto SpawnedConfig2 = [Spawner2]()
                    {
                        USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();
                        TArray<FTransform> TransformArray;
                        const FTransform CurrentTransform = Spawner2->GetTransform();
                        TransformArray.Add(CurrentTransform);
                        Config->SpawnedObjects.Add("Test2", TransformArray);

                        auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);

                        Spawner2->EmitConfigCompleted(FinalConfig, true);
                    };
                    Spawner2->LambdaGenerateSpawnedObjectConfiguration = SpawnedConfig2;

                    UAmbitExporterDelegateWatcher* ConfigurationDelegateWatcher = NewObject<
                        UAmbitExporterDelegateWatcher>();
                    ConfigurationDelegateWatcher->SpawnerCount = 2;
                    ConfigurationDelegateWatcher->bSendToS3 = false;
                    ConfigurationDelegateWatcher->Parent = Exporter;

                    Spawner->GetOnSpawnedObjectConfigCompletedDelegate().BindUObject(
                        ConfigurationDelegateWatcher,
                        &UAmbitExporterDelegateWatcher::SpawnedObjectConfigCompleted_Handler);

                    Spawner2->GetOnSpawnedObjectConfigCompletedDelegate().BindUObject(
                        ConfigurationDelegateWatcher,
                        &UAmbitExporterDelegateWatcher::SpawnedObjectConfigCompleted_Handler);

                    Spawner->GenerateSpawnedObjectConfiguration();
                    Spawner2->GenerateSpawnedObjectConfiguration();
                });

                It("Should Generate Config from ProcessSdfForExport", [this]()
                {
                    TestTrue("JSON Contains expected test field (first item)",
                             JsonContent.Contains("\"ActorToSpawn\": \"Test\","));
                    TestTrue("JSON Contains expected test field (second item)",
                             JsonContent.Contains("\"ActorToSpawn\": \"Test2\","));
                });

                It("Should Condense Arrays of Spawned Objects", [this]()
                {
                    const TSharedPtr<FJsonObject> ConvertedJson = FJsonHelpers::DeserializeJson(JsonContent);
                    const TArray<TSharedPtr<FJsonValue>>* OutputArray = {};

                    const TSharedPtr<FJsonObject> AmbitSpawnerObject = ConvertedJson->GetObjectField(
                        JsonConstants::KAmbitSpawnerKey);
                    if (AmbitSpawnerObject)
                    {
                        AmbitSpawnerObject->TryGetArrayField(JsonConstants::KAmbitSpawnerObjectsKey, OutputArray);
                    }

                    TestEqual("Spawned Objects should be merged into one field", OutputArray->Num(), 2);
                });
            });
        });

        AfterEach([this]()
        {
            GLevelEditorModeTools().DeactivateMode(FAmbitMode::EM_AmbitModeId);
            Exporter = nullptr;
            JsonContent = nullptr;
        });
    });
}
