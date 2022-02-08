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

#include "ConfigImportExportInterface.generated.h"

class FJsonObject;
class FReply;
class FString;

/**
 * Interface for ConfigImportExport.h and ConfigImportExportMock.h
 */
UINTERFACE()
class UConfigImportExportInterface : public UInterface
{
    GENERATED_BODY()
};

class IConfigImportExportInterface
{
    GENERATED_BODY()
public:
    /**
     * Starts the process to import an SDF file specified on screen.
     *
     * @return FReply When the process has finished.
     */
    virtual FReply OnImportSdf() = 0;

    /**
     * Starts the process to export the SDF file specified. This process triggers asynchronous functions
     * to handle the exporting, and does not actually export itself.
     *
     * @return FReply::Finished When the process has been started for the asynchronous handlings.
     */
    virtual FReply OnExportSdf() = 0;

    /**
     * Based on the config settings from screen or the queued config settings from QueuedSdfConfigToExport,
     * will combine those with the passed AmbitSpawnerArray into a standardized SDF format, and then
     * writes the file.
     * This function will attempt to create the S3 bucket, if bToS3 is selected.
     * At the end of this function, if there are any more items in QueuedSdfConfigToExport, it will call
     * PrepareAllSpawnersObjectConfigs to continue to the process of the queue.
     *
     *
     * @param AmbitSpawnerArray A Mapping of FJsonValue (coming from SpawnerObjects), and the name of their JSON
     * format to output for their location in the array.
     * @param bToS3 Specifies whether the file should attempt to upload to S3 or save to disk.
     *
     * @return Boolean to indicate if the write was successful or not.
     */
    virtual bool ProcessSdfForExport(const TMap<FString, TSharedPtr<FJsonObject>>& AmbitSpawnerArray, bool bToS3) = 0;

    // Generate Permutations
    /**
     * Imports on a BSC file on disk.
     *
     * @return FReply::Finished When the process has finished.
     */
    virtual FReply OnImportBsc() = 0;

    /**
     * Starts the process to export a BSC file and all permutations of
     * it to a SDF into the on-screen specified S3 bucket location. The BSC
     * process is synchronous, but the SDF process is asynchronous.
     *
     * @return FReply When the process has either finished (through error)
     * or when the process was successful and the SDF has started.
     */
    virtual FReply OnGeneratePermutations() = 0;

    /**
     * Imports on a BSC file in the specified Amazon S3.
     *
     * @return FReply::Finished when the process has finished (success or failure).
     */
    virtual FReply OnReadFromS3Bucket() = 0;

    /**
     * Exports the current map into the Amazon S3 account specified.
     */
    virtual FReply OnExportMap() = 0;

    /**
     * Exports the current scene as a glTF file (*.gltf or *.glb) into the Amazon S3 account specified.
     */
    virtual FReply OnExportGltf() = 0;
};
