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

#include "S3UEClient.h"

#include "AWSUE4Module.h"
#include "AWSUEStringUtils.h"
#include "Misc/MessageDialog.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/PutBucketEncryptionRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

TSet<FString> S3UEClient::ListBuckets()
{
    Aws::S3::S3Client S3Client;
    auto Outcome = S3Client.ListBuckets();
    TSet<FString> BucketsSet;

    if (!Outcome.IsSuccess())
    {
        auto Err = Outcome.GetError();
        UE_LOG(LogAWSUE4Module, Error, TEXT("ListBuckets: %s : %s"), *FString(Err.GetExceptionName().c_str()),
               *FString(Err.GetMessage().c_str()));
        throw std::runtime_error(Err.GetMessage().c_str());
    }

    UE_LOG(LogAWSUE4Module, Display, TEXT("Bucket names: "));

    Aws::Vector<Aws::S3::Model::Bucket> Buckets = Outcome.GetResult().GetBuckets();

    for (const Aws::S3::Model::Bucket& Bucket : Buckets)
    {
        FString BucketName = AWSUEStringUtils::AwsStringToFString(Bucket.GetName());
        BucketsSet.Add(BucketName);
        UE_LOG(LogAWSUE4Module, Display, TEXT("%s"), *BucketName);
    }
    UE_LOG(LogAWSUE4Module, Display, TEXT("The num of %d"), static_cast<int>(Buckets.size()));
    return BucketsSet;
}

bool S3UEClient::CreateBucket(const FString& Region, const FString& BucketName)
{
    const Aws::String S3Region = AWSUEStringUtils::FStringToAwsString(Region);
    const Aws::String S3BucketName = AWSUEStringUtils::FStringToAwsString(BucketName);

    if (S3BucketName.empty() || S3Region.empty())
    {
        UE_LOG(LogAWSUE4Module, Error, TEXT("The bucket name or region is empty. Please check them again."));
        throw std::invalid_argument("The bucket name or region is empty. Please check them again.");
    }

    // create s3 bucket in given region with given name
    Aws::Client::ClientConfiguration Config;
    Config.region = S3Region;

    const Aws::S3::S3Client S3Client(Config);
    Aws::S3::Model::CreateBucketRequest Request;
    Request.SetBucket(S3BucketName);

    // transfer bucket region to a specific type which AWS S3 CreateBucket function will use
    const Aws::S3::Model::BucketLocationConstraint RegionConstraint =
            Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(S3Region);

    // By default, buckets are created in the us-east-1(N. Virginia) region.
    // If you use a Region other than the US East (N. Virginia) endpoint to create a bucket,
    // you must set the LocationConstraint bucket parameter to the same Region.
    // https://docs.aws.amazon.com/general/latest/gr/s3.html
    if (RegionConstraint != Aws::S3::Model::BucketLocationConstraint::us_east_1)
    {
        Aws::S3::Model::CreateBucketConfiguration BucketConfig;
        BucketConfig.SetLocationConstraint(RegionConstraint);

        Request.SetCreateBucketConfiguration(BucketConfig);
    }

    const auto Outcome = S3Client.CreateBucket(Request);

    if (!Outcome.IsSuccess())
    {
        const auto Err = Outcome.GetError();
        UE_LOG(LogAWSUE4Module, Error, TEXT("Create Bucket: %s : %s"), *FString(Err.GetExceptionName().c_str()),
               *FString(Err.GetMessage().c_str()));
        throw std::runtime_error(Err.GetMessage().c_str());
    }

    UE_LOG(LogAWSUE4Module, Display, TEXT("Successfully create bucket: %s."), *BucketName);
    return true;
}


void S3UEClient::CreateBucketWithEncryption(const FString& Region, const FString& BucketName)
{
    CreateBucket(Region, BucketName);
    PutBucketEncryption(BucketName);
}

bool S3UEClient::PutBucketEncryption(const FString& BucketName)
{
    Aws::String S3BucketName = AWSUEStringUtils::FStringToAwsString(BucketName);
    Aws::S3::S3Client S3Client;

    // set server-side encryption for the created bucket
    Aws::S3::Model::ServerSideEncryptionByDefault SSEByDefault;
    SSEByDefault.SetSSEAlgorithm(Aws::S3::Model::ServerSideEncryption::AES256);
    Aws::S3::Model::ServerSideEncryptionRule SSERule;
    SSERule.SetApplyServerSideEncryptionByDefault(SSEByDefault);

    Aws::S3::Model::ServerSideEncryptionConfiguration SSEConfiguration;
    Aws::Vector<Aws::S3::Model::ServerSideEncryptionRule> SSERules;
    SSERules.emplace_back(SSERule);
    SSEConfiguration.WithRules(SSERules);

    Aws::S3::Model::PutBucketEncryptionRequest BucketEncryptionRequest;
    BucketEncryptionRequest.SetBucket(S3BucketName);
    BucketEncryptionRequest.SetServerSideEncryptionConfiguration(SSEConfiguration);

    Aws::S3::Model::PutBucketEncryptionOutcome BucketEncryptionOutcome = S3Client.PutBucketEncryption(
        BucketEncryptionRequest);
    if (!BucketEncryptionOutcome.IsSuccess())
    {
        auto Err = BucketEncryptionOutcome.GetError();
        UE_LOG(LogAWSUE4Module, Error, TEXT("Put Bucket Encryption: %s : %s"), *FString(Err.GetExceptionName().c_str()),
               *FString(Err.GetMessage().c_str()));
        throw std::runtime_error(Err.GetMessage().c_str());
    }
    UE_LOG(LogAWSUE4Module, Display, TEXT("Successfully put encryption to bucket: %s."), *BucketName);
    return true;
}

TSet<FString> S3UEClient::ListObjects(const FString& Region, const FString& BucketName)
{
    Aws::String S3Region = AWSUEStringUtils::FStringToAwsString(Region);
    Aws::String S3BucketName = AWSUEStringUtils::FStringToAwsString(BucketName);

    TSet<FString> ObjectsSet;

    if (S3BucketName.empty() || S3Region.empty())
    {
        UE_LOG(LogAWSUE4Module, Error, TEXT("The bucket name or region is empty. Please check them again."));
        throw std::invalid_argument("The bucket name or region is empty. Please check them again.");
    }

    Aws::Client::ClientConfiguration Config;
    Config.region = S3Region;

    Aws::S3::S3Client S3Client(Config);

    Aws::S3::Model::ListObjectsRequest Request;
    Request.WithBucket(S3BucketName);

    auto Outcome = S3Client.ListObjects(Request);

    if (!Outcome.IsSuccess())
    {
        auto Err = Outcome.GetError();
        UE_LOG(LogAWSUE4Module, Error, TEXT("ListObjects: %s : %s"), *FString(Err.GetExceptionName().c_str()),
               *FString(Err.GetMessage().c_str()));
        throw std::runtime_error(Err.GetMessage().c_str());
    }

    UE_LOG(LogAWSUE4Module, Display, TEXT("Objects in bucket: "));

    Aws::Vector<Aws::S3::Model::Object> Objects = Outcome.GetResult().GetContents();

    for (const Aws::S3::Model::Object& Object : Objects)
    {
        FString ObjectName = AWSUEStringUtils::AwsStringToFString(Object.GetKey());
        UE_LOG(LogAWSUE4Module, Display, TEXT("%s"), *ObjectName);
        ObjectsSet.Add(ObjectName);
    }
    UE_LOG(LogAWSUE4Module, Display, TEXT("The num of %d"), static_cast<int>(Objects.size()));
    return ObjectsSet;
}

FString S3UEClient::GetObjectAsString(const FString& Region, const FString& BucketName, const FString& ObjectName)
{
    Aws::String S3Region = AWSUEStringUtils::FStringToAwsString(Region);
    Aws::String S3BucketName = AWSUEStringUtils::FStringToAwsString(BucketName);
    Aws::String S3ObjectName = AWSUEStringUtils::FStringToAwsString(ObjectName);

    if (S3Region.empty() || S3BucketName.empty() || S3ObjectName.empty())
    {
        UE_LOG(LogAWSUE4Module, Error,
               TEXT("The region, bucket name or object name is empty. Please check them again."));
        throw std::invalid_argument("The region, bucket name or object name is empty. Please check them again.");
    }

    Aws::Client::ClientConfiguration Config;
    Config.region = S3Region;

    Aws::S3::S3Client S3Client(Config);
    Aws::S3::Model::GetObjectRequest ObjectRequest;
    ObjectRequest.SetBucket(S3BucketName);
    ObjectRequest.SetKey(S3ObjectName);

    Aws::S3::Model::GetObjectOutcome GetObjectOutcome = S3Client.GetObject(ObjectRequest);

    if (!GetObjectOutcome.IsSuccess())
    {
        auto Err = GetObjectOutcome.GetError();
        UE_LOG(LogAWSUE4Module, Error, TEXT("GetObjectAsString: %s : %s"), *FString(Err.GetExceptionName().c_str()),
               *FString(Err.GetMessage().c_str()));
        throw std::runtime_error(Err.GetMessage().c_str());
    }

    auto& RetrievedFile = GetObjectOutcome.GetResultWithOwnership().GetBody();

    std::ostringstream StringStream;
    StringStream << RetrievedFile.rdbuf();
    std::string FileContents = StringStream.str();
    UE_LOG(LogAWSUE4Module, Display, TEXT("Get object successfully!"));
    return FString(FileContents.c_str());
}

bool S3UEClient::PutObject(const FString& Region, const FString& BucketName, const FString& ObjectName,
                           const FString& ObjectContent)
{
    Aws::String S3Region = AWSUEStringUtils::FStringToAwsString(Region);
    Aws::String S3BucketName = AWSUEStringUtils::FStringToAwsString(BucketName);
    Aws::String S3ObjectName = AWSUEStringUtils::FStringToAwsString(ObjectName);

    if (S3Region.empty() || S3BucketName.empty() || S3ObjectName.empty())
    {
        UE_LOG(LogAWSUE4Module, Error,
               TEXT("The region, bucket name or object name is empty. Please check them again."));
        throw std::invalid_argument("The region, bucket name or object name is empty. Please check them again.");
    }

    Aws::Client::ClientConfiguration Config;
    Config.region = S3Region;

    Aws::S3::S3Client S3Client(Config);

    Aws::S3::Model::PutObjectRequest Request;
    Request.SetBucket(S3BucketName);
    Request.SetKey(S3ObjectName);

    std::string ObjectContentString = std::string(TCHAR_TO_UTF8(*ObjectContent));

    const std::shared_ptr<Aws::IOStream> InputData = Aws::MakeShared<Aws::StringStream>("");
    *InputData << ObjectContentString.c_str();

    Request.SetBody(InputData);

    auto Outcome = S3Client.PutObject(Request);

    if (!Outcome.IsSuccess())
    {
        auto Err = Outcome.GetError();
        UE_LOG(LogAWSUE4Module, Error, TEXT("PutObject: %s : %s"), *FString(Err.GetExceptionName().c_str()),
               *FString(Err.GetMessage().c_str()));
        throw std::runtime_error(Err.GetMessage().c_str());
    }

    UE_LOG(LogAWSUE4Module, Display, TEXT("Added object to bucket."));
    return true;
}

bool S3UEClient::PutLocalObject(const FString& Region, const FString& BucketName, const FString& ObjectName,
                                const FString& LocalFilePath)
{
    Aws::String S3Region = AWSUEStringUtils::FStringToAwsString(Region);
    Aws::String S3BucketName = AWSUEStringUtils::FStringToAwsString(BucketName);
    Aws::String S3ObjectName = AWSUEStringUtils::FStringToAwsString(ObjectName);

    if (S3Region.empty() || S3BucketName.empty() || S3ObjectName.empty())
    {
        UE_LOG(LogAWSUE4Module, Error,
               TEXT("The region, bucket name or object name is empty. Please check them again."));
        throw std::invalid_argument("The region, bucket name or object name is empty. Please check them again.");
    }

    std::string FileNameString = std::string(TCHAR_TO_UTF8(*LocalFilePath));
    struct stat buffer;
    if (stat(FileNameString.c_str(), &buffer) == -1)
    {
        UE_LOG(LogAWSUE4Module, Error, TEXT("Specified file %s does not exist. Please check it again"), *LocalFilePath);
        throw std::invalid_argument("Specified file does not exist. Please check it again.");
    }

    Aws::Client::ClientConfiguration Config;
    Config.region = S3Region;

    Aws::S3::S3Client S3Client(Config);

    Aws::S3::Model::PutObjectRequest Request;
    Request.SetBucket(S3BucketName);
    Request.SetKey(S3ObjectName);

    std::shared_ptr<Aws::IOStream> InputData = Aws::MakeShared<Aws::FStream>(
        "SampleTag", FileNameString.c_str(), std::ios_base::in | std::ios_base::binary);

    Request.SetBody(InputData);

    auto Outcome = S3Client.PutObject(Request);

    if (!Outcome.IsSuccess())
    {
        auto Err = Outcome.GetError();
        UE_LOG(LogAWSUE4Module, Error, TEXT("PutLocalObject: %s : %s"), *FString(Err.GetExceptionName().c_str()),
               *FString(Err.GetMessage().c_str()));
        throw std::runtime_error(Err.GetMessage().c_str());
    }
    UE_LOG(LogAWSUE4Module, Display, TEXT("Added object to bucket."));
    return true;
}
