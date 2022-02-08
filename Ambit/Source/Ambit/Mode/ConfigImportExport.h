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

#include "ScenarioDefinition.h"
#include "Dom/JsonObject.h"
#include "Misc/AutomationTest.h"
#include "Templates/SharedPointer.h"
#include "UObject/Object.h"

#include "Ambit/Actors/SpawnedObjectConfigs/SpawnedObjectConfig.h"
#include "Ambit/Mode/ConfigImportExportInterface.h"
#include "Ambit/Utils/AmbitFileHelpers.h"
#include "Ambit/Utils/AWSWrapper.h"

#include "ConfigImportExport.generated.h"

class IGltfExportInterface;
class FAmbitMode;
class USpawnedObjectConfig;
struct FPedestrianTraffic;
struct FVehicleTraffic;

/**
 * Class dedicated to controlling the importing and exporting of configuration files for the AmbitSpawners.
 */
UCLASS()
class UConfigImportExport : public UObject, public IConfigImportExportInterface
{
    GENERATED_BODY()
public:
    //Constructor
    UConfigImportExport();

    /** @inheritDoc */
    FReply OnImportSdf();

    /** @inheritDoc */
    FReply OnExportSdf();

    /** @inheritDoc */
    bool ProcessSdfForExport(const TMap<FString, TSharedPtr<FJsonObject>>& AmbitSpawnerArray, bool bToS3);

    /** @inheritDoc */
    FReply OnImportBsc();

    /** @inheritDoc */
    FReply OnGeneratePermutations();

    /** @inheritDoc */
    FReply OnReadFromS3Bucket();

    /** @inheritDoc */
    FReply OnExportMap();

    /** @inheritDoc */
    FReply OnExportGltf();

    /**
     * Assign new dependency instances for use by this ConfigImportExport object.
     *
     * @param Exporter The glTF Exporter to be used.
     */
    void SetDependencies(IGltfExportInterface* Exporter);

    /**
    * Overrides the default behavior of S3ListBuckets, the function called to list all buckets in an account, to be overwritten with
    * the function passed in.
    */
    static void SetMockS3ListBuckets(TFunction<TSet<FString>()> MockFunction);

    /**
    * Overrides the default behavior of S3CreateBucket, the function called to create a new bucket for ____, to be overwritten with
    * the function passed in.
    */
    static void SetMockS3CreateBucket(TFunction<void(const FString& Region, const FString& BucketName)> MockFunction);

    /**
     * Overrides the default behavior of LambdaGetPathFromPopup, the function called that creates a popup for writing a file to disk,
     * to be the function passed in.
     */
    void SetMockGetPathFromPopup(TFunction<FString(const FString& FileExtension, const FString& DefaultPath,
                                                   const FString& FileName)> MockFunction);

    /**
     * Overrides the default behavior of LambdaWriteFileToDisk, the function called when a write to disk is actually happening in ConfigImportExport,
     * to be the function passed in.
     */
    void SetMockWriteFile(TFunction<void(const FString& FilePath, const FString& OutString)> MockFunction);

    /**
     * Overrides the default behavior of LambdaPutS3Object, the function called when uploading an object to Amazon S3 in ConfigImportExport,
     * to be the function passed in.
     */
    void SetMockPutObjectS3(TFunction<bool(const FString& Region, const FString& BucketName, const FString& ObjectName,
                                           const FString& Content)> MockFunction);

    /**
     * Overrides the default behavior of LambdaS3FileUpload, the function called when uploading a file to an Amazon S3 bucket in ConfigImportExport,
     * to be the function passed in.
     */
    void SetMockS3FileUpload(TFunction<bool(const FString& Region, const FString& BucketName, const FString& ObjectName,
                                            const FString& FilePath)> MockFunction);

    /**
     * Sets the DoneDelegate to be the value specified. Used in "Latent" Automation Tests.
     */
    void SetSdfProcessDone(FDoneDelegate const& DoneEvent);

protected:
    /**
     * Calls AWSWrapper::PutObject
     * Allows for injection of the function to be changed. Should only be changed in testing.
     */
    TFunction<bool(const FString& Region, const FString& BucketName, const FString& ObjectName, const FString& Content)>
    LambdaPutS3Object = AWSWrapper::PutObject;

    /**
     * Calls AWSWrapper::UploadFile
     * Allows for injection of the function to be changed. Should only be changed in testing.
     */
    TFunction<bool
        (const FString& Region, const FString& BucketName, const FString& ObjectName, const FString& FilePath)>
    LambdaS3FileUpload = AWSWrapper::UploadFile;

    /**
     * Calls AmbitFileHelpers::WriteFile
     * Allows for injection of the function to be changed. Should only be changed in testing.
     */
    TFunction<void(const FString& FilePath, const FString& OutString)> LambdaWriteFileToDisk =
            AmbitFileHelpers::WriteFile;

    /**
     * Calls AmbitFileHelpers::GetPathForFileFromPopup
     * Allows for injection of the function to be changed. Should only be changed in testing.
     */
    TFunction<FString(const FString& FileExtension, const FString& DefaultPath, const FString& Filename)>
    LambdaGetPathFromPopup = AmbitFileHelpers::GetPathForFileFromPopup;

    /**
    * For internal testing only. Returns when ProcessSdfForExport() has been completed and there are no more items in queue. 
    */
    FDoneDelegate SdfProcessDone;

private:
    // Spawner and Spawned Object Configuration Functions
    /**
     * Retrieves all spawners under the IAmbitSpawner and starts
     * to process them for an export. This uses asynchronous methods,
     * and merely kicks off the process.
     * This process will ultimately call ProcessSdfForExport() once all of the
     * objects are ready to be written.
     *
     * @param bToS3 A boolean to indicate whether this configuration
     * should be output to Amazon S3 or to local.
     */
    void PrepareAllSpawnersObjectConfigs(bool bToS3);

    /**
     * Given a JSON object describing a Bulk Scenario Configuration, this method recreates
     * the Ambit Spawners described by that JSON.
     */
    void CreateAmbitSpawnersFromJson(const TSharedPtr<FJsonObject>& JsonObject);

    /**
     * Given a JSON object describing all Ambit Spawners in the BSC file, this method
     * recreates and configures any spawner of ClassType using the StructType configuration.
     */
    template <typename ClassType, typename StructType>
    void ConfigureSpawnersByType(const TSharedPtr<FJsonObject>& Spawners, const FString& TypeKey, UWorld*& World);

    /**
     * Given a JSON object, adds an array field to the object containing JSON objects
     * serialized from StructType configuration that describe each Ambit Spawner of ClassType
     */
    template <typename ClassType, typename StructType>
    void SerializeSpawnerConfigs(TSharedPtr<FJsonObject>& SpawnersJson, const FString& SpawnerTypeKey);

    /**
     * Retrieves the next item in QueuedSdfConfigToExport if one exist, else
     * takes the values on screen to create one.
     * Assumes AmbitMode is valid, initialized, and correct.
     *
     * @return Shared Pointer of a FScenarioDefinition for the next SDF that should be
     * written. If the queue exists, it will return the next object from that and remove it from
     * the queue. If the queue doesn't exist, it will take the values from the screen
     * to create a defaulted one.
     */
    TSharedPtr<FScenarioDefinition> DequeueOrDefaultNextSdfConfigToProcess() const;

    // JSON Helpers
    /**
     * Writes the OutputContents with the FileName and FileExtension to either Amazon S3 or to disk.
     * Assumes AmbitMode is valid, initialized, and correct.
     *
     * @param OutputContents A valid FJsonObject that will be written to disk.
     * @param FileName The output file's name, without extension.
     * @param FileExtension The output file's extension. If bToS3 is specified, and this
     * is an SDF extension, it will sub-folder the file automatically.
     * @param bToS3 Specifies whether the file should attempt to upload to S3 or save to disk.
     * This will also try to create the bucket if one is not found.
     *
     * @return True if the file was successfully written. False otherwise.
     */
    bool WriteJsonFile(const TSharedPtr<FJsonObject>& OutputContents, const FString& FileName,
                       const FString& FileExtension, bool bToS3);

    // AWS Helpers
    /**
     * Retrieves the user defined AWS Bucket and Region. Will attempt to create a bucket if
     * bucket name is not available.
     *
     * @param OutAwsRegion Returns the specified user-inputted AWS Region.
     * @param OutAwsBucketName Returns the specified user-inputted AWS Bucket Name.
     * @param bCreateBucketOnGet Specifies if while retrieving the settings, the function should
     * try to find if the bucket actively exists, and, if not, attempts to create it.
     *
     * @return Boolean indicating that the bucket is available and ready to be utilized. Will
     * always return True if CreateBucketOnGet is False.
     */
    static bool GetAwsSettings(FString& OutAwsRegion, FString& OutAwsBucketName, bool bCreateBucketOnGet);

    /**
     * Create Bucket if the bucket name doesn't exist.
     *
     * @param Region The AWS Region that the S3 bucket is in.
     * @param BucketName The name of the bucket to search for or create.
     *
     * @return True if the bucket already exists or if the bucket was created. False otherwise.
     */
    static bool CreateBucket(const FString& Region, const FString& BucketName);

    // Global Defaults
    /**
     * @return The FString of what the default, if not specified, name for the Configuration
     */
    static FString GetDefaultConfigurationName()
    {
        return "AmbitScenarioConfiguration";
    }

    /**
     * @return The FString of what the default, if not specified, name for the Scenario Name
     */
    static FString GetDefaultScenarioName()
    {
        return "AmbitScenario";
    }

    /**
     * @return The prefix for the folder for the BSC's SDF location.
     */
    static FString GetS3ExportFolderPrefix()
    {
        return "GeneratedScenarios-";
    }

private:
    IGltfExportInterface* GltfExporter;
};

/**
 * Intermediate class to keep track of delegates for the spawners for their configuration spawning.
 */
UCLASS()
class AMBIT_API UAmbitExporterDelegateWatcher : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Must be set to the current number of actors that are being tracked.
     * Used to determine when to fire the returning event.
     */
    int SpawnerCount;

    /**
     * Determines if the output of the delegate handler should send to Amazon S3 on output.
     */
    bool bSendToS3 = false;

    /**
     * The parent instance that created this. This must be set on instance creation.
     */
    UPROPERTY()
    UConfigImportExport* Parent;

    /**
     * Handles the return delegate response from the spawners, and calls ProcessSdfForExport
     * when all spawners have finished calling the delegate handler.
     * If any item fails, returns a message and will not call WriteJsonFile.
     *
     * @param Config A reference to a class that implements the interface IConfigJsonSerializer.
     * @param bSuccess The result of the delegate if it succeeded.
     */
    UFUNCTION(Category = "Ambit Detail Customization Delegate Handler")
    void SpawnedObjectConfigCompleted_Handler(UPARAM(ref) TScriptInterface<IConfigJsonSerializer>& Config,
                                              bool bSuccess);

private:
    /**
     * The current number of spawners that have completed their response.
     */
    int CurrentCompleted;

    /**
     * An internal mapping of the output configuration name for the JSON object, and a list of the output JSON results.
     */
    TMap<FString, TSharedPtr<FJsonObject>> AllSpawnerConfiguration;
};
