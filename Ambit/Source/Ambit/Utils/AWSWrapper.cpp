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

#include "AWSWrapper.h"

#include <AWSUE4Module/Public/AWSHelpers.h>
#include <AWSUE4Module/Public/S3UEClient.h>

TSet<FString> AWSWrapper::ListBuckets()
{
    return S3UEClient::ListBuckets();
}

void AWSWrapper::CreateBucketWithEncryption(const FString& Region, const FString& BucketName)
{
    return S3UEClient::CreateBucketWithEncryption(Region, BucketName);
}

TSet<FString> AWSWrapper::ListObjects(const FString& Region, const FString& BucketName)
{
    return S3UEClient::ListObjects(Region, BucketName);
}

bool AWSWrapper::PutObject(const FString& Region, const FString& BucketName, const FString& ObjectName,
                           const FString& Content)
{
    return S3UEClient::PutObject(Region, BucketName, ObjectName, Content);
}

bool AWSWrapper::UploadFile(const FString& Region, const FString& BucketName, const FString& ObjectName,
                            const FString& FilePath)
{
    return S3UEClient::PutLocalObject(Region, BucketName, ObjectName, FilePath);
}

FString AWSWrapper::GetObject(const FString& Region, const FString& BucketName, const FString& ObjectName)
{
    return S3UEClient::GetObjectAsString(Region, BucketName, ObjectName);
}

TArray<FString> AWSWrapper::GetAwsRegions()
{
    return AWSHelpers::GetAwsRegions();
}
