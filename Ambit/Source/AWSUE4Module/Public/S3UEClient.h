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

namespace S3UEClient
{
    /**
     *Lists S3 buckets.
     *@return
     *  Returns a TSet indicating current buckets.
     */
    AWSUE4MODULE_API TSet<FString> ListBuckets();

    /**
     *Create S3 bucket.
     *@return
     * Returns a Boolean variable indicating whether successfully creating the bucket.
     */
    AWSUE4MODULE_API bool CreateBucket(const FString& Region, const FString& BucketName);

    /**
     *Create S3 bucket with server-side encryption.
     */
    AWSUE4MODULE_API void CreateBucketWithEncryption(const FString& Region, const FString& BucketName);

    /**
     *Add server-side encryption to the bucket.
     *@return
     * Returns a Boolean variable indicating whether successfully putting the encryption.
     */
    AWSUE4MODULE_API bool PutBucketEncryption(const FString& BucketName);

    /**
     *Lists objects in the given S3 bucket.
     *@return
     *  Returns a TSet indicating objects in the given bucket.
     */
    AWSUE4MODULE_API TSet<FString> ListObjects(const FString& Region, const FString& BucketName);

    /**
     *Gets Object content as a string from the given S3 bucket
     *@return
     *  Returns a FString indicating object content by given bucket name and object path in the bucket
     */
    AWSUE4MODULE_API FString GetObjectAsString(const FString& Region, const FString& BucketName,
                                               const FString& ObjectName);

    /**
     *Writes a string value(ObjectContent) into the bucket
     *@return
     *  Returns a Boolean variable indicating whether successfully writing the object into the bucket by given bucket name,
     *  object name, object content and region
     */
    AWSUE4MODULE_API bool PutObject(const FString& Region, const FString& BucketName,
                                    const FString& ObjectName, const FString& ObjectContent);

    /**
     * Upload a local file to the bucket
     *
     * @param Region region the bucket sits in
     * @param BucketName bucket name 
     * @param ObjectName key name when the object is uploaded
     * @param LocalFilePath path to the local file to be uploaded
     * @return Returns a Boolean variable indicating whether successfully uploading the file onto the bucket by given bucket name,
     *  object name, file name and region
     */
    AWSUE4MODULE_API bool PutLocalObject(const FString& Region, const FString& BucketName,
                                         const FString& ObjectName, const FString& LocalFilePath);
};
