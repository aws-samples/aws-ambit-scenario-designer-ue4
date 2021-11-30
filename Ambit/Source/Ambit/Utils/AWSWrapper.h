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

#include "CoreMinimal.h"

/**
 * This is a wrapper class to use AWS functions. The purpose is to decouple the AWS SDK with Ambit plugin.
 */
class AMBIT_API AWSWrapper
{
public:
    static TSet<FString> ListBuckets();
    static void CreateBucketWithEncryption(const FString& Region, const FString& BucketName);
    static TSet<FString> ListObjects(const FString& Region, const FString& BucketName);
    static bool PutObject(const FString& Region, const FString& BucketName, const FString& ObjectName,
                          const FString& Content);
    static bool UploadFile(const FString& Region, const FString& BucketName, const FString& ObjectName,
                          const FString& FilePath);
    static FString GetObject(const FString& Region, const FString& BucketName, const FString& ObjectName);
    static TArray<FString> GetAwsRegions();
};
