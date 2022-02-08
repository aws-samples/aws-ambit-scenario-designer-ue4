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

#include "BulkScenarioConfiguration.h"
#include "GltfExport.h"
#include "ScenarioDefinition.h"
#include "WeatherTypes.h"
#include "Containers/Queue.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/StaticMeshActor.h"
#include "HAL/PlatformProcess.h"
#include "HoudiniEngineEditor/Public/HoudiniPublicAPIAssetWrapper.h"
#include "Kismet/GameplayStatics.h"
#include "Math/NumericLimits.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Input/SNumericEntryBox.h"

#include <stdexcept>

#include "AmbitDetailCustomization.h"
#include "AmbitMode.h"
#include "AmbitObject.h"
#include "Ambit/AmbitModule.h"
#include "Ambit/Actors/SpawnerConfigs/SpawnerBaseConfig.h"
#include "Ambit/Actors/SpawnerConfigs/SpawnInVolumeConfig.h"
#include "Ambit/Actors/SpawnerConfigs/SpawnOnPathConfig.h"
#include "Ambit/Actors/SpawnerConfigs/SpawnVehiclePathConfig.h"
#include "Ambit/Actors/Spawners/SpawnInVolume.h"
#include "Ambit/Actors/Spawners/SpawnOnPath.h"
#include "Ambit/Actors/Spawners/SpawnOnSurface.h"
#include "Ambit/Actors/Spawners/SpawnVehiclePath.h"
#include "Ambit/Actors/Spawners/SpawnWithHoudini.h"
#include "Ambit/Mode/GltfExportInterface.h"
#include "Ambit/Utils/AmbitFileHelpers.h"
#include "Ambit/Utils/AWSWrapper.h"
#include "Ambit/Utils/UserMetricsSubsystem.h"

#include <AmbitUtils/JsonHelpers.h>
#include <AmbitUtils/MenuHelpers.h>

// INFO: Replace with an inline variable in header when Unreal allows them (C++ 17)
/**
  * A queue to keep track of all of the Configs that need exported to SDF.
 */
TQueue<TSharedPtr<FScenarioDefinition>> QueuedSdfConfigToExport;

// Static member handling.
//
// Calls AWSWrapper::ListBuckets
// Allows for injection of the function so that it can be changed for functional testing purposes.
static TFunction<TSet<FString>()> LambdaS3ListBuckets = AWSWrapper::ListBuckets;

// Calls AWSWrapper::CreateBucketWithEncryption
// Allows for injection of the function so that it can be changed for functional testing purposes. 
static TFunction<void(const FString& Region, const FString& BucketName)> LambdaS3CreateBucket =
        AWSWrapper::CreateBucketWithEncryption;

//Constructor
UConfigImportExport::UConfigImportExport()
{
    GltfExporter = NewObject<UGltfExport>();
};


FReply UConfigImportExport::OnImportSdf()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    const FString FullContents = AmbitFileHelpers::LoadSingleFileFromPopup("Json Files (*.json)|*.JSON");

    if (FullContents.IsEmpty())
    {
        return FReply::Handled();
    }

    // Parse JSON file
    const TSharedPtr<FJsonObject> DeserializedSdf = FJsonHelpers::DeserializeJson(FullContents);

    if (!DeserializedSdf)
    {
        FMenuHelpers::LogErrorAndPopup("Error Parsing Scenario Definition File.");
        return FReply::Handled();
    }

    FScenarioDefinition SdfScenario;
    SdfScenario.DeserializeFromJson(DeserializedSdf);

    // if the scenario name is empty, something went wrong with deserialization. Throw an error.
    if (SdfScenario.ScenarioName.IsEmpty())
    {
        FMenuHelpers::LogErrorAndPopup("Unable to Load Scenario Definition File. Please Check File and Try Again.");
        return FReply::Handled();
    }

    AmbitMode->UISettings->ScenarioName = SdfScenario.ScenarioName;
    AmbitMode->UISettings->TimeOfDay = SdfScenario.TimeOfDay;

    if (!TimeOfDay::NumberToPresetTimeOfDay.Contains(SdfScenario.TimeOfDay))
    {
        AmbitMode->UISettings->PresetTimeOfDay = TimeOfDay::KCustom;
    }
    else
    {
        AmbitMode->UISettings->PresetTimeOfDay = TimeOfDay::NumberToPresetTimeOfDay[SdfScenario.TimeOfDay];
    }

    AmbitMode->UISettings->WeatherParameters = SdfScenario.AmbitWeatherParameters;
    AmbitMode->UISettings->PresetWeather = AmbitWeatherParameters::GetWeatherType(SdfScenario.AmbitWeatherParameters);

    AmbitMode->UISettings->PedestrianDensity = SdfScenario.PedestrianDensity;
    AmbitMode->UISettings->VehicleDensity = SdfScenario.VehicleDensity;

    CreateAmbitSpawnersFromJson(DeserializedSdf);

    return FReply::Handled();
}

FReply UConfigImportExport::OnExportSdf()
{
    PrepareAllSpawnersObjectConfigs(false);

    GEngine->GetEngineSubsystem<UUserMetricsSubsystem>()->Track(UserMetrics::AmbitMode::KAmbitModeExportSDF,
                                                                UserMetrics::AmbitMode::KAmbitModeNameSpace);

    return FReply::Handled();
}

bool UConfigImportExport::ProcessSdfForExport(const TMap<FString, TSharedPtr<FJsonObject>>& AmbitSpawnerArray,
                                              bool bToS3)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    bool bWriteSuccess = false;

    const TSharedPtr<FScenarioDefinition> ScenarioToProcess = DequeueOrDefaultNextSdfConfigToProcess();

    if (ScenarioToProcess.IsValid())
    {
        ScenarioToProcess->AllSpawnersConfigs = MakeShareable(new FJsonObject);
        SerializeSpawnerConfigs<ASpawnOnSurface, FSpawnerBaseConfig>(ScenarioToProcess->AllSpawnersConfigs,
                                                                     JsonConstants::KSpawnerSurfaceKey);
        SerializeSpawnerConfigs<ASpawnInVolume, FSpawnInVolumeConfig>(ScenarioToProcess->AllSpawnersConfigs,
                                                                      JsonConstants::KSpawnerVolumeKey);
        SerializeSpawnerConfigs<ASpawnOnPath, FSpawnOnPathConfig>(ScenarioToProcess->AllSpawnersConfigs,
                                                                  JsonConstants::KSpawnerPathKey);
        SerializeSpawnerConfigs<ASpawnWithHoudini, FSpawnWithHoudiniConfig>(
            ScenarioToProcess->AllSpawnersConfigs, JsonConstants::KSpawnerSurfaceHoudiniKey);
        SerializeSpawnerConfigs<ASpawnVehiclePath, FSpawnVehiclePathConfig>(
            ScenarioToProcess->AllSpawnersConfigs, JsonConstants::KSpawnerVehiclePathKey);

        const TSharedPtr<FJsonObject> JsonObject = ScenarioToProcess->SerializeToJson();

        for (const auto& SpawnerKeyValue : AmbitSpawnerArray)
        {
            JsonObject->SetObjectField(SpawnerKeyValue.Key, SpawnerKeyValue.Value);
        }

        const FString Name = ScenarioToProcess->ScenarioName;

        bWriteSuccess = WriteJsonFile(JsonObject, Name, FileExtensions::KSDFExtension, bToS3);
    }

    // Once we have finished, we cycle to the next item in the queue for its SDF creation.
    if (!QueuedSdfConfigToExport.IsEmpty())
    {
        PrepareAllSpawnersObjectConfigs(bToS3);
    }
    else if (bToS3)
    {
        const FText NotificationText = NSLOCTEXT("Ambit", "ScenariosUploadComplete",
                                                 "Scenarios successfully uploaded to Amazon S3.");
        FAmbitModule::CreateAmbitNotification(NotificationText);
    }
    else
    {
        SdfProcessDone.ExecuteIfBound();
    }

    return bWriteSuccess;
}

FReply UConfigImportExport::OnImportBsc()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    const FString FullContents = AmbitFileHelpers::LoadSingleFileFromPopup("Json Files (*.json)|*.JSON");

    if (FullContents.IsEmpty())
    {
        return FReply::Handled();
    }

    // Parse JSON file
    const TSharedPtr<FJsonObject> DeserializedBsc = FJsonHelpers::DeserializeJson(FullContents);

    if (!DeserializedBsc)
    {
        FMenuHelpers::LogErrorAndPopup("Error Parsing Bulk Scenario Configuration.");
        return FReply::Handled();
    }

    FBulkScenarioConfiguration BscScenario;
    BscScenario.DeserializeFromJson(DeserializedBsc);

    if (BscScenario.ConfigurationName.IsEmpty())
    {
        FMenuHelpers::LogErrorAndPopup("Unable to Load Bulk Scenario Configuration. Please Check File and Try Again.");
        return FReply::Handled();
    }

    AmbitMode->UISettings->ConfigurationName = BscScenario.ConfigurationName;
    AmbitMode->UISettings->BatchName = BscScenario.BatchName;
    AmbitMode->UISettings->WeatherTypes = BscScenario.WeatherTypes;
    AmbitMode->UISettings->BulkPedestrianTraffic = BscScenario.PedestrianDensity;
    AmbitMode->UISettings->BulkVehicleTraffic = BscScenario.VehicleDensity;
    AmbitMode->UISettings->NumberOfPermutations = BscScenario.NumberOfPermutations;

    CreateAmbitSpawnersFromJson(DeserializedBsc);

    return FReply::Handled();
}

FReply UConfigImportExport::OnGeneratePermutations()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    // If the user hasn't input their own name, default to this name
    FString Name = GetDefaultConfigurationName();
    if (!AmbitMode->UISettings->ConfigurationName.IsEmpty())
    {
        Name = AmbitMode->UISettings->ConfigurationName;
    }

    // Sets up batch name for exported configuration file.
    FString ScenarioNamePrefix = GetDefaultScenarioName();
    if (!AmbitMode->UISettings->BatchName.IsEmpty())
    {
        ScenarioNamePrefix = AmbitMode->UISettings->BatchName;
    }

    // Passes all information in Ambit Mode into BSC Struct and serializes to Json file
    FBulkScenarioConfiguration BscScenario;
    BscScenario.ConfigurationName = Name;
    BscScenario.BatchName = ScenarioNamePrefix;
    BscScenario.TimeOfDayTypes = AmbitMode->UISettings->TimeOfDayTypes;
    BscScenario.WeatherTypes = AmbitMode->UISettings->WeatherTypes;
    BscScenario.PedestrianDensity = AmbitMode->UISettings->BulkPedestrianTraffic;
    BscScenario.VehicleDensity = AmbitMode->UISettings->BulkVehicleTraffic;
    BscScenario.NumberOfPermutations = AmbitMode->UISettings->NumberOfPermutations;

    BscScenario.AllSpawnersConfigs = MakeShareable(new FJsonObject);
    SerializeSpawnerConfigs<ASpawnOnSurface, FSpawnerBaseConfig>(BscScenario.AllSpawnersConfigs,
                                                                 JsonConstants::KSpawnerSurfaceKey);
    SerializeSpawnerConfigs<ASpawnInVolume, FSpawnInVolumeConfig>(BscScenario.AllSpawnersConfigs,
                                                                  JsonConstants::KSpawnerVolumeKey);
    SerializeSpawnerConfigs<ASpawnOnPath, FSpawnOnPathConfig>(BscScenario.AllSpawnersConfigs,
                                                              JsonConstants::KSpawnerPathKey);
    SerializeSpawnerConfigs<ASpawnWithHoudini, FSpawnWithHoudiniConfig>(BscScenario.AllSpawnersConfigs,
                                                                        JsonConstants::KSpawnerSurfaceHoudiniKey);
    SerializeSpawnerConfigs<ASpawnVehiclePath, FSpawnVehiclePathConfig>(BscScenario.AllSpawnersConfigs,
                                                                        JsonConstants::KSpawnerVehiclePathKey);

    // Serialize the whole object into Json format
    TSharedPtr<FJsonObject> JsonObject = BscScenario.SerializeToJson();

    FString BucketName = AmbitMode->UISettings->S3BucketName;

    const bool BscWroteSuccess = WriteJsonFile(JsonObject, Name, FileExtensions::KBSCExtension, true);
    if (!BscWroteSuccess)
    {
        return FReply::Handled();
    }

    TSharedRef<FJsonObject> BscMetricContextData = MakeShareable(new FJsonObject);
    BscMetricContextData->SetStringField(UserMetrics::AmbitMode::KAmbitS3BucketNameContextData, BucketName);
    GEngine->GetEngineSubsystem<UUserMetricsSubsystem>()->Track(UserMetrics::AmbitMode::KAmbitUploadBSCEvent,
                                                                UserMetrics::AmbitMode::KAmbitModeNameSpace,
                                                                BscMetricContextData);

    const TArray<FScenarioDefinition> GeneratedScenarios = BscScenario.GenerateScenarios();

    TSharedRef<FJsonObject> SdfMetricContextData = MakeShareable(new FJsonObject);
    SdfMetricContextData->SetNumberField(UserMetrics::AmbitMode::KAmbitBulkSDFExportContextData,
                                         GeneratedScenarios.Num());
    GEngine->GetEngineSubsystem<UUserMetricsSubsystem>()->Track(UserMetrics::AmbitMode::KAmbitBulkSDFExportEvent,
                                                                UserMetrics::AmbitMode::KAmbitModeNameSpace,
                                                                SdfMetricContextData);

    FRandomStream SRand = FRandomStream();
    int32 ScenarioRandomSeed = 1;
    SRand.Initialize(ScenarioRandomSeed);

    for (int32 i = 0; i < GeneratedScenarios.Num(); ++i)
    {
        TSharedPtr<FScenarioDefinition> SharedScenario = MakeShareable(new FScenarioDefinition);
        FScenarioDefinition ScenarioObject = GeneratedScenarios[i];

        SharedScenario->ScenarioName = ScenarioNamePrefix + FString::FromInt(i + 1);
        SharedScenario->Seed = SRand.RandHelper(TNumericLimits<int32>::Max());

        SharedScenario->TimeOfDay = ScenarioObject.TimeOfDay;
        SharedScenario->AmbitWeatherParameters = ScenarioObject.AmbitWeatherParameters;
        SharedScenario->PedestrianDensity = ScenarioObject.PedestrianDensity;
        SharedScenario->VehicleDensity = ScenarioObject.VehicleDensity;
        SharedScenario->AllSpawnersConfigs = BscScenario.AllSpawnersConfigs;

        QueuedSdfConfigToExport.Enqueue(SharedScenario);
    }

    // Start the process for SDF output
    PrepareAllSpawnersObjectConfigs(true);

    return FReply::Handled();
}

FReply UConfigImportExport::OnReadFromS3Bucket()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    FString BucketName;
    FString AwsRegion;
    GetAwsSettings(AwsRegion, BucketName, false);

    FString ConfigurationName = GetDefaultConfigurationName();
    if (!AmbitMode->UISettings->ConfigurationName.IsEmpty())
    {
        ConfigurationName = AmbitMode->UISettings->ConfigurationName;
    }

    FString BscPath = ConfigurationName + FileExtensions::KBSCExtension;

    TSet<FString> ObjectsSet;
    try
    {
        ObjectsSet = AWSWrapper::ListObjects(AwsRegion, BucketName);
    }
    catch (const std::invalid_argument& Ia)
    {
        FMenuHelpers::LogErrorAndPopup(Ia.what());
        return FReply::Handled();
    }
    catch (const std::runtime_error& Re)
    {
        FMenuHelpers::LogErrorAndPopup(Re.what());
        return FReply::Handled();
    }

    // If the bucket is empty
    if (ObjectsSet.Num() == 0)
    {
        FMenuHelpers::LogErrorAndPopup("There is no object in this bucket. Please check the input information again.");
        return FReply::Handled();
    }

    // If the bucket doesn't have this file
    if (!ObjectsSet.Contains(BscPath))
    {
        FMenuHelpers::LogErrorAndPopup(
            "Do not have this object. Please check the bucket name, object name and region again.");
        return FReply::Handled();
    }

    FString FullContents;

    try
    {
        FullContents = AWSWrapper::GetObject(AwsRegion, BucketName, BscPath);
    }
    catch (const std::invalid_argument& Ia)
    {
        FMenuHelpers::LogErrorAndPopup(Ia.what());
        return FReply::Handled();
    }
    catch (const std::runtime_error& Re)
    {
        FMenuHelpers::LogErrorAndPopup(Re.what());
        return FReply::Handled();
    }

    // Parse JSON file
    TSharedPtr<FJsonObject> DeserializedBsc = FJsonHelpers::DeserializeJson(FullContents);

    if (!DeserializedBsc)
    {
        FMenuHelpers::LogErrorAndPopup("Error Parsing Bulk Scenario Configuration.");
        return FReply::Handled();
    }

    FBulkScenarioConfiguration BscScenario;
    BscScenario.DeserializeFromJson(DeserializedBsc);

    if (BscScenario.ConfigurationName.IsEmpty())
    {
        FMenuHelpers::LogErrorAndPopup("Unable to Load Bulk Scenario Configuration. Please Check File and Try Again.");
        return FReply::Handled();
    }

    AmbitMode->UISettings->ConfigurationName = BscScenario.ConfigurationName;
    AmbitMode->UISettings->BatchName = BscScenario.BatchName;
    AmbitMode->UISettings->TimeOfDayTypes = BscScenario.TimeOfDayTypes;
    AmbitMode->UISettings->WeatherTypes = BscScenario.WeatherTypes;
    AmbitMode->UISettings->BulkPedestrianTraffic = BscScenario.PedestrianDensity;
    AmbitMode->UISettings->BulkVehicleTraffic = BscScenario.VehicleDensity;

    FAmbitDetailCustomization::UpdateNumberOfPermutations();

    CreateAmbitSpawnersFromJson(DeserializedBsc);

    return FReply::Handled();
}

FReply UConfigImportExport::OnExportMap()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    const FString Executable = FPlatformProcess::ExecutablePath();
    FString ErrorMessage;

    const UWorld* World = GEngine->GetWorldContexts()[0].World();
    const FString MapName = World->GetName();
    const FString MapPath = World->GetPathName();

    TArray<FString> TargetPlatforms = AmbitMode->UISettings->ExportPlatforms.GetSelectedPlatforms();

    FString AwsRegion;
    FString BucketName;
    const bool bFoundBucket = GetAwsSettings(AwsRegion, BucketName, true);
    if (!bFoundBucket)
    {
        return FReply::Handled();
    }

    for (const auto& TargetPlatform : TargetPlatforms)
    {
        try
        {
            const FString& OutputDir = FPaths::Combine(*FPaths::ProjectDir(), TEXT("Saved"), TEXT("Cooked"), *MapName,
                                                       *TargetPlatform);
            FString Arguments = FPaths::GetProjectFilePath() + " -run=cook -map=" + MapPath +
                    " -cooksinglepackage -targetplatform=" + TargetPlatform + " -OutputDir=" + OutputDir;
            int32 ReturnCode = 0;
            FProcHandle ProcHandle = FPlatformProcess::CreateProc(*Executable, *Arguments, false, false, false, nullptr,
                                                                  0, nullptr, nullptr);
            if (!ProcHandle.IsValid())
            {
                ErrorMessage = "Failed to create process with command " + Executable + " " + Arguments + ".";
                throw std::runtime_error(
                    "Cook map " + std::string(TCHAR_TO_UTF8(*MapName)) + " Failed. " + std::string(
                        TCHAR_TO_UTF8(*ErrorMessage)));
            }
            FPlatformProcess::WaitForProc(ProcHandle);
            FPlatformProcess::GetProcReturnCode(ProcHandle, &ReturnCode);

            if (ReturnCode != 0)
            {
                ErrorMessage = "Command " + Executable + " " + Arguments + " failed with error code " +
                        FString::FromInt(ReturnCode) + ".";
                throw std::runtime_error(
                    "Cook map " + std::string(TCHAR_TO_UTF8(*MapName)) + " Failed. " + std::string(
                        TCHAR_TO_UTF8(*ErrorMessage)));
            }
            UE_LOG(LogAmbit, Display, TEXT("Cook Map %s for %s Successfully!"), *MapName, *TargetPlatform);

            const FString& CompressedFile = AmbitFileHelpers::CompressFile(OutputDir, FPaths::ProjectIntermediateDir(),
                                                                           MapName + "_" + TargetPlatform,
                                                                           TargetPlatform);

            LambdaS3FileUpload(AwsRegion, BucketName, CompressedFile,
                               FPaths::Combine(*FPaths::ProjectIntermediateDir(), *CompressedFile));
        }
        catch (const std::invalid_argument& Ia)
        {
            ErrorMessage = "Export Map " + MapName + " for " + TargetPlatform + " failed. ";
            FMenuHelpers::LogErrorAndPopup(ErrorMessage + Ia.what());
        }
        catch (const std::runtime_error& Re)
        {
            ErrorMessage = "Export Map " + MapName + " for " + TargetPlatform + " failed. ";
            FMenuHelpers::LogErrorAndPopup(ErrorMessage + Re.what());
        }
    }

    return FReply::Handled();
}

FReply UConfigImportExport::OnExportGltf()
{
    FString ErrorMessage;
    if (GltfExporter == nullptr)
    {
        ErrorMessage = "glTF Exporter not initialized.";
        FMenuHelpers::LogErrorAndPopup(ErrorMessage);

        return FReply::Handled();
    }

    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    const TArray<FString> TargetPlatforms = AmbitMode->UISettings->ExportPlatforms.GetSelectedPlatforms();

    FString AwsRegion;
    FString BucketName;
    const bool bFoundBucket = GetAwsSettings(AwsRegion, BucketName, true);
    if (!bFoundBucket)
    {
        return FReply::Handled();
    }

    // Find World and compare to CurrentWorldContext to make sure we're not accidentally referring to valid static
    // object from another world context.
    UWorld* CurrentWorldContext = GEngine->GetWorldContexts()[0].World();
    bool bFoundWorld = false;
    for (TObjectIterator<UStaticMeshComponent> Itr; Itr; ++Itr)
    {
        AActor* Parent = (*Itr)->GetOwner();
        if (Parent == nullptr)
        {
            continue;
        }

        // Two steps to determine validity of actor as a static mesh actor:
        // 1. It should be of type AStaticMeshActor or AHoudiniAssetActor.
        // 2. It should not be marked for deletion in the editor.
        const bool bIsStaticMeshActor = Parent->IsA(AStaticMeshActor::StaticClass());
        const bool bIsHoudiniAssetActor = UHoudiniPublicAPIAssetWrapper::CanWrapHoudiniObject(Parent);
        const bool bIsValidActor = bIsStaticMeshActor || bIsHoudiniAssetActor;
        const bool bIsActorBeingDeleted = Parent->IsActorBeingDestroyed();
        if (!bIsValidActor || bIsActorBeingDeleted)
        {
            continue;
        }

        // Important to check if ParentWorld == nullptr because it is possible for an Actor to be of type
        // AStaticMeshActor or AHoudiniAssetActor but not be associated with a World object,
        // (e.g) AInteractiveFoliageActor, which derives from AStaticMeshActor but doesn't point to a World object.
        UWorld* ParentWorld = Parent->GetWorld();
        if (ParentWorld != nullptr && ParentWorld == CurrentWorldContext)
        {
            bFoundWorld = true;
            break;
        }
    }

    if (!bFoundWorld)
    {
        ErrorMessage = "Cannot find a static mesh to export.";
        FMenuHelpers::LogErrorAndPopup(ErrorMessage);

        return FReply::Handled();
    }

    const FString GltfExtension = AmbitMode->UISettings->GltfType.ToLower();

    const FString MapName = CurrentWorldContext->GetName();
    const FString FolderName = MapName + TEXT("_") + GltfExtension;
    const FString Filename = MapName + TEXT(".") + GltfExtension;

    const FString OutputDir = FPaths::Combine(*FPaths::ProjectDir(), TEXT("Saved"), TEXT("Cooked"), FolderName);
    const FString FilePath = FPaths::Combine(OutputDir, Filename);

    // Perform the export to glTF.
    const bool IsExportSuccess = GltfExporter->Export(CurrentWorldContext, FilePath);
    if (!IsExportSuccess)
    {
        ErrorMessage = "glTF Export Failed.";
        FMenuHelpers::LogErrorAndPopup(ErrorMessage);

        return FReply::Handled();
    }

    for (const FString& TargetPlatform : TargetPlatforms)
    {
        const FString& CompressedFile = AmbitFileHelpers::CompressFile(OutputDir, FPaths::ProjectIntermediateDir(),
                                                                       FolderName, TargetPlatform);

        LambdaS3FileUpload(AwsRegion, BucketName, CompressedFile,
                           FPaths::Combine(*FPaths::ProjectIntermediateDir(), *CompressedFile));
    }

    const FText NotificationText = NSLOCTEXT("Ambit", "MapUploadComplete", "Successfully uploaded to S3.");
    FAmbitModule::CreateAmbitNotification(NotificationText);

    return FReply::Handled();
}

void UConfigImportExport::PrepareAllSpawnersObjectConfigs(bool bToS3)
{
    TArray<AActor*> AllActorsToSerialize;
    // Further reading: https://exiin.com/blog/unreal-c-interface-and-what-to-do-with-it/
    UGameplayStatics::GetAllActorsWithInterface(GEngine->GetWorldContexts()[0].World(), UAmbitSpawner::StaticClass(),
                                                AllActorsToSerialize);

    TArray<AActor*> ValidActors = AllActorsToSerialize.FilterByPredicate([](AActor* Actor)
    {
        return Cast<IAmbitSpawner>(Actor)->HasActorsToSpawn();
    });

    // If we have no spawners, we can just call the config and no handle the asynchronous process.
    if (ValidActors.Num() == 0)
    {
        const TMap<FString, TSharedPtr<FJsonObject>> EmptyConfiguration;
        ProcessSdfForExport(EmptyConfiguration, bToS3);
        return;
    }

    UAmbitExporterDelegateWatcher* ConfigurationDelegateWatcher = NewObject<UAmbitExporterDelegateWatcher>();
    ConfigurationDelegateWatcher->SpawnerCount = ValidActors.Num();
    ConfigurationDelegateWatcher->bSendToS3 = bToS3;
    ConfigurationDelegateWatcher->Parent = this;

    const TSharedPtr<FScenarioDefinition>* ExistingDefinition = QueuedSdfConfigToExport.Peek();
    const int32 Seed = ExistingDefinition != nullptr && ExistingDefinition->IsValid()
                           ? ExistingDefinition->Get()->Seed
                           : INDEX_NONE;

    for (AActor* Actor : ValidActors)
    {
        IAmbitSpawner* CastedActor = Cast<IAmbitSpawner>(Actor);
        CastedActor->GetOnSpawnedObjectConfigCompletedDelegate().BindUObject(
            ConfigurationDelegateWatcher, &UAmbitExporterDelegateWatcher::SpawnedObjectConfigCompleted_Handler);

        // Create the asynchronous call to the object so that the delegate handles the return.
        if (Seed == INDEX_NONE)
        {
            CastedActor->GenerateSpawnedObjectConfiguration();
        }
        else
        {
            CastedActor->GenerateSpawnedObjectConfiguration(Seed);
        }
    }
}

void UConfigImportExport::CreateAmbitSpawnersFromJson(const TSharedPtr<FJsonObject>& JsonObject)
{
    UWorld* World = GEngine->GetWorldContexts()[0].World();

    // Remove any existing AmbitSpawners.
    TArray<AActor*> ExistingSpawners;
    UGameplayStatics::GetAllActorsWithInterface(World, UAmbitSpawner::StaticClass(), ExistingSpawners);
    for (AActor* Spawner : ExistingSpawners)
    {
        Spawner->Destroy();
    }

    const TSharedPtr<FJsonObject> Spawners = JsonObject->GetObjectField(JsonConstants::KAllSpawnersConfigsKey);

    ConfigureSpawnersByType<ASpawnOnSurface, FSpawnerBaseConfig>(Spawners, JsonConstants::KSpawnerSurfaceKey, World);
    ConfigureSpawnersByType<ASpawnInVolume, FSpawnInVolumeConfig>(Spawners, JsonConstants::KSpawnerVolumeKey, World);
    ConfigureSpawnersByType<ASpawnOnPath, FSpawnOnPathConfig>(Spawners, JsonConstants::KSpawnerPathKey, World);
    ConfigureSpawnersByType<ASpawnWithHoudini, FSpawnWithHoudiniConfig>(
        Spawners, JsonConstants::KSpawnerSurfaceHoudiniKey, World);
    ConfigureSpawnersByType<ASpawnVehiclePath, FSpawnVehiclePathConfig>(Spawners, JsonConstants::KSpawnerVehiclePathKey,
                                                                        World);
}

template <typename ClassType, typename StructType>
void UConfigImportExport::ConfigureSpawnersByType(const TSharedPtr<FJsonObject>& Spawners, const FString& TypeKey,
                                                  UWorld*& World)
{
    if (Spawners->HasField(TypeKey))
    {
        for (const TSharedPtr<FJsonValue>& JsonValue : Spawners->GetArrayField(TypeKey))
        {
            TSharedPtr<StructType> SpawnerConfig = MakeShareable(new StructType);
            SpawnerConfig->DeserializeFromJson(JsonValue->AsObject());
            const FVector& Location = SpawnerConfig->SpawnerLocation;
            const FRotator& Rotation = SpawnerConfig->SpawnerRotation;
            // Spawn an ClassType Spawner and configure.
            ClassType* Spawner = World->SpawnActor<ClassType>(Location, Rotation);
            Spawner->Configure(SpawnerConfig);
        }
    }
}

template <typename ClassType, typename StructType>
void UConfigImportExport::SerializeSpawnerConfigs(TSharedPtr<FJsonObject>& SpawnersJson,
                                                  const FString& SpawnerTypeKey)
{
    TArray<TSharedPtr<FJsonValue>> SpawnerTypeSpecificArray;
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GEngine->GetWorldContexts()[0].World(), ClassType::StaticClass(), AllActors);
    // Get each AmbitSpawner's configuration and add to AmbitSpawnerArray
    for (AActor* Actor : AllActors)
    {
        TSharedPtr<StructType> AmbitSpawnerConfig = static_cast<ClassType*>(Actor)->GetConfiguration();
        const TSharedPtr<FJsonObject> curJson = AmbitSpawnerConfig->SerializeToJson();
        TSharedRef<FJsonValueObject> JsonValue = MakeShareable(new FJsonValueObject(curJson));
        SpawnerTypeSpecificArray.Add(JsonValue);
    }
    if (SpawnerTypeSpecificArray.Num() > 0)
    {
        SpawnersJson->SetArrayField(SpawnerTypeKey, SpawnerTypeSpecificArray);
    }
}

TSharedPtr<FScenarioDefinition> UConfigImportExport::DequeueOrDefaultNextSdfConfigToProcess() const
{
    const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();

    TSharedPtr<FScenarioDefinition> PoppedItem;
    const bool bQueueHasPopped = QueuedSdfConfigToExport.Dequeue(PoppedItem);

    // Pop the latest item. If there are no items, utilize on-screen defaults.
    if (bQueueHasPopped && PoppedItem.IsValid())
    {
        return PoppedItem;
    }

    TSharedPtr<FScenarioDefinition> SharedScenario = MakeShareable(new FScenarioDefinition);

    // If the user hasn't input their own name, default to this tone
    FString Name = GetDefaultScenarioName();
    if (!AmbitMode->UISettings->ScenarioName.IsEmpty())
    {
        Name = AmbitMode->UISettings->ScenarioName;
    }

    SharedScenario->ScenarioName = Name;
    SharedScenario->TimeOfDay = AmbitMode->UISettings->TimeOfDay;
    SharedScenario->AmbitWeatherParameters = AmbitMode->UISettings->WeatherParameters;
    SharedScenario->PedestrianDensity = AmbitMode->UISettings->PedestrianDensity;
    SharedScenario->VehicleDensity = AmbitMode->UISettings->VehicleDensity;

    return SharedScenario;
}

bool UConfigImportExport::WriteJsonFile(const TSharedPtr<FJsonObject>& OutputContents, const FString& FileName,
                                        const FString& FileExtension, bool bToS3)
{
    const FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();

    const FString& OutputString = FJsonHelpers::SerializeJson(OutputContents);

    if (OutputString.IsEmpty() || FileName.IsEmpty() || FileExtension.IsEmpty())
    {
        FMenuHelpers::LogErrorAndPopup("Error Serializing File.");
        return false;
    }
    const FString OutputFileName = FileName + FileExtension;

    if (bToS3)
    {
        try
        {
            FString AwsRegion;
            FString BucketName;
            const bool bFoundBucket = GetAwsSettings(AwsRegion, BucketName, true);
            if (!bFoundBucket)
            {
                return false;
            }

            FString BscConfigurationName = GetDefaultConfigurationName();
            if (!AmbitMode->UISettings->ConfigurationName.IsEmpty())
            {
                BscConfigurationName = AmbitMode->UISettings->ConfigurationName;
            }

            // Write the file into the specified path. For SDF, we layer one step down into the sub-folder.
            FString S3Path;
            if (FileExtension == FileExtensions::KSDFExtension)
            {
                S3Path = FPaths::Combine(GetS3ExportFolderPrefix() + BscConfigurationName, OutputFileName);
            }
            else
            {
                S3Path = OutputFileName;
            }

            UE_LOG(LogAmbit, Display, TEXT("Writing Json object to Amazon S3 Path: %s"), *S3Path);

            return LambdaPutS3Object(AwsRegion, BucketName, S3Path, OutputString);
        }
        catch (const std::invalid_argument& Ia)
        {
            const FString ErrorMessage = "WriteJsonFile failed with invalid argument error: " + FString(Ia.what());
            FMenuHelpers::LogErrorAndPopup(ErrorMessage);
            return false;
        }
        catch (const std::runtime_error& Re)
        {
            const FString ErrorMessage = "WriteJsonFile failed with runtime error: " + FString(Re.what());
            FMenuHelpers::LogErrorAndPopup(ErrorMessage);
            return false;
        }
    }
    const FString OutFile = LambdaGetPathFromPopup(FileExtension, "", OutputFileName);

    if (!OutFile.IsEmpty())
    {
        LambdaWriteFileToDisk(OutFile, OutputString);
    }
    else
    {
        return false;
    }

    return true;
}

bool UConfigImportExport::GetAwsSettings(FString& OutAwsRegion, FString& OutAwsBucketName, bool bCreateBucketOnGet)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    // Gets Amazon S3 information to save files into the S3 bucket
    OutAwsRegion = AmbitMode->UISettings->AwsRegion;
    UE_LOG(LogAmbit, Display, TEXT("Region: %s"), *OutAwsBucketName);

    OutAwsBucketName = AmbitMode->UISettings->S3BucketName;
    UE_LOG(LogAmbit, Display, TEXT("BucketName: %s"), *OutAwsBucketName);

    return !(bCreateBucketOnGet && !CreateBucket(OutAwsRegion, OutAwsBucketName));
}

bool UConfigImportExport::CreateBucket(const FString& AwsRegion, const FString& BucketName)
{
    // List all buckets for this account, save them into Set, check whether this bucket is in the set.
    TSet<FString> BucketsSet;
    try
    {
        BucketsSet = LambdaS3ListBuckets();
    }
    catch (const std::runtime_error& Re)
    {
        const FString ErrorMessage = "CreateBucket failed with runtime error: " + FString(Re.what());
        FMenuHelpers::LogErrorAndPopup(ErrorMessage);
        return false;
    }

    if (!BucketsSet.Contains(BucketName))
    {
        try
        {
            LambdaS3CreateBucket(AwsRegion, BucketName);
        }
        catch (const std::invalid_argument& Ia)
        {
            const FString ErrorMessage = "CreateBucket failed with invalid argument error: " + FString(Ia.what());
            FMenuHelpers::LogErrorAndPopup(ErrorMessage);
            return false;
        }
        catch (const std::runtime_error& Re)
        {
            const FString ErrorMessage = "CreateBucket failed with runtime error: " + FString(Re.what());
            FMenuHelpers::LogErrorAndPopup(ErrorMessage);
            return false;
        }
    }
    return true;
}

void UAmbitExporterDelegateWatcher::SpawnedObjectConfigCompleted_Handler(
    TScriptInterface<IConfigJsonSerializer>& Config, bool bSuccess)
{
    if (!bSuccess)
    {
        const FString NotificationText = "The SDF failed to be created.";
        FMenuHelpers::DisplayMessagePopup(NotificationText, "Error");

        UE_LOG(LogAmbit, Error, TEXT("One of the SDF configurations have failed to generate properly"));

        AllSpawnerConfiguration.Empty();
        return;
    }

    const TSharedPtr<FJsonObject> SerializedJsonObject = Config->SerializeToJson();

    const FString ConfigName = Config->GetOutputConfigurationName();

    if (!AllSpawnerConfiguration.Contains(ConfigName))
    {
        AllSpawnerConfiguration.Add(ConfigName, SerializedJsonObject);
    }
    else
    {
        const TSharedPtr<FJsonObject> OldSpawnerObjects = AllSpawnerConfiguration[ConfigName];
        TArray<TSharedPtr<FJsonValue>> OldSpawnerObjectsJsonArray;
        if (OldSpawnerObjects->HasField(JsonConstants::KAmbitSpawnerObjectsKey))
        {
            OldSpawnerObjectsJsonArray = OldSpawnerObjects->GetArrayField(JsonConstants::KAmbitSpawnerObjectsKey);
        }

        TArray<TSharedPtr<FJsonValue>> NewSpawnerObjectsJsonArray;
        if (SerializedJsonObject->HasField(JsonConstants::KAmbitSpawnerObjectsKey))
        {
            NewSpawnerObjectsJsonArray = SerializedJsonObject->GetArrayField(JsonConstants::KAmbitSpawnerObjectsKey);
        }
        OldSpawnerObjectsJsonArray.Append(NewSpawnerObjectsJsonArray);
        OldSpawnerObjects->SetArrayField(JsonConstants::KAmbitSpawnerObjectsKey, OldSpawnerObjectsJsonArray);
        AllSpawnerConfiguration[ConfigName] = OldSpawnerObjects;
    }

    CurrentCompleted++;

    if (SpawnerCount == CurrentCompleted)
    {
        Parent->ProcessSdfForExport(AllSpawnerConfiguration, bSendToS3);
        AllSpawnerConfiguration.Empty();
    }
}

void UConfigImportExport::SetDependencies(IGltfExportInterface* Exporter)
{
    GltfExporter = Exporter;
}

void UConfigImportExport::SetMockGetPathFromPopup(TFunction<FString(const FString& FileExtension,
                                                                    const FString& DefaultPath,
                                                                    const FString& FileName)> MockFunction)
{
    LambdaGetPathFromPopup = std::move(MockFunction);
};

void UConfigImportExport::SetMockWriteFile(
    TFunction<void(const FString& FilePath, const FString& OutString)> MockFunction)
{
    LambdaWriteFileToDisk = std::move(MockFunction);
};

void UConfigImportExport::SetMockPutObjectS3(TFunction<bool(const FString& Region, const FString& BucketName,
                                                            const FString& ObjectName,
                                                            const FString& Content)> MockFunction)
{
    LambdaPutS3Object = std::move(MockFunction);
};

void UConfigImportExport::SetMockS3FileUpload(TFunction<bool(const FString& Region, const FString& BucketName,
                                                             const FString& ObjectName,
                                                             const FString& FilePath)> MockFunction)
{
    LambdaS3FileUpload = std::move(MockFunction);
}

void UConfigImportExport::SetSdfProcessDone(FDoneDelegate const& DoneEvent)
{
    SdfProcessDone = DoneEvent;
}

void UConfigImportExport::SetMockS3ListBuckets(TFunction<TSet<FString>()> MockFunction)
{
    LambdaS3ListBuckets = std::move(MockFunction);
}

void UConfigImportExport::SetMockS3CreateBucket(
    TFunction<void(const FString& Region, const FString& BucketName)> MockFunction)
{
    LambdaS3CreateBucket = std::move(MockFunction);
}
