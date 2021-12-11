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

#include <utility>

#include "Ambit/Mode/ConfigImportExport.h"

#include "MockableConfigImportExport.generated.h"

UCLASS()
class UMockableConfigImportExport : public UConfigImportExport
{
    GENERATED_BODY()
public:
    /**
     * Overrides the default behavior of LambdaGetPathFromPopup, the function called that creates a popup for writing a file to disk,
     * to be the function passed in.
     */
    void SetMockGetPathFromPopup(TFunction<FString(const FString& FileExtension, const FString& DefaultPath,
                                                   const FString& FileName)> MockFunction)
    {
        LambdaGetPathFromPopup = std::move(MockFunction);
    };

    /**
     * Overrides the default behavior of LambdaWriteFileToDisk, the function called when a write to disk is actually happening in ConfigImportExport,
     * to be the function passed in.
     */
    void SetMockWriteFile(TFunction<void(const FString& FilePath, const FString& OutString)> MockFunction)
    {
        LambdaWriteFileToDisk = std::move(MockFunction);
    };

    /**
     * Overrides the default behavior of LambdaPutS3Object, the function called when uploading an object to Amazon S3 in ConfigImportExport,
     * to be the function passed in.
     */
    void SetMockPutObjectS3(TFunction<bool(const FString& Region, const FString& BucketName, const FString& ObjectName,
                                           const FString& Content)> MockFunction)
    {
        LambdaPutS3Object = std::move(MockFunction);
    };

    /**
     * Sets the DoneDelegate to be the value specified. Used in "Latent" Automation Tests.
     */
    void SetSdfProcessDone(FDoneDelegate const& DoneEvent)
    {
        SdfProcessDone = DoneEvent;
    }
};
