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

#include "HAL/PlatformFilemanager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/GetBucketEncryptionRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

BEGIN_DEFINE_SPEC(S3UEClientSpec,
                  "AWSUE4Module.S3UEClient",
                  EAutomationTestFlags::ProductFilter | EAutomationTestFlags::
                  ApplicationContextMask)
    FString Region;
    FString BucketName;
    FString ObjectName;
    FString ObjectContent;
    FString FilePath;

    FString BucketName_1;
    FString BucketName_2;
    FString BucketName_3;
END_DEFINE_SPEC(S3UEClientSpec)

void S3UEClientSpec::Define()
{
    Describe("ListBuckets()", [this]()
    {
        BeforeEach([this]()
        {
            Region = "us-west-2";
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            const Aws::S3::Model::BucketLocationConstraint& RegionBucketLocationConstraint =
                Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(RegionAwsString);

            BucketName_1 = "ambit-listbuckets-test1";
            BucketName_2 = "ambit-listbuckets-test2";
            BucketName_3 = "ambit-listbuckets-test3";

            std::string BucketNameStdString_1 = std::string(TCHAR_TO_UTF8(*BucketName_1));
            Aws::String BucketNameAwsString_1(BucketNameStdString_1.c_str(), BucketNameStdString_1.size());
            std::string BucketNameStdString_2 = std::string(TCHAR_TO_UTF8(*BucketName_2));
            Aws::String BucketNameAwsString_2(BucketNameStdString_2.c_str(), BucketNameStdString_2.size());
            std::string BucketNameStdString_3 = std::string(TCHAR_TO_UTF8(*BucketName_3));
            Aws::String BucketNameAwsString_3(BucketNameStdString_3.c_str(), BucketNameStdString_3.size());

            // Create BucketName_1
            Aws::S3::S3Client S3Client;

            Aws::S3::Model::CreateBucketRequest CreateRequest_1;
            CreateRequest_1.SetBucket(BucketNameAwsString_1);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig_1;
            BucketConfig_1.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest_1.SetCreateBucketConfiguration(BucketConfig_1);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome_1 =
                S3Client.CreateBucket(CreateRequest_1);
            if (!CreateOutcome_1.IsSuccess())
            {
                auto Err = CreateOutcome_1.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating bucket fails: %s, %s"), *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            // Create BucketName_2
            Aws::S3::Model::CreateBucketRequest CreateRequest_2;
            CreateRequest_2.SetBucket(BucketNameAwsString_2);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig_2;
            BucketConfig_2.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest_2.SetCreateBucketConfiguration(BucketConfig_2);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome_2 =
                S3Client.CreateBucket(CreateRequest_2);
            if (!CreateOutcome_2.IsSuccess())
            {
                auto Err = CreateOutcome_2.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating bucket_2 fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            // Create BucketName_3
            Aws::S3::Model::CreateBucketRequest CreateRequest_3;
            CreateRequest_3.SetBucket(BucketNameAwsString_3);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig_3;
            BucketConfig_3.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest_3.SetCreateBucketConfiguration(BucketConfig_3);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome_3 =
                S3Client.CreateBucket(CreateRequest_3);
            if (!CreateOutcome_3.IsSuccess())
            {
                auto Err = CreateOutcome_3.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating bucket_3 fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });

        It("should contains three new Buckets.", [this]()
        {
            TestTrue(TEXT("ListBuckets"), S3UEClient::ListBuckets().Contains(BucketName_1));
            TestTrue(TEXT("ListBuckets"), S3UEClient::ListBuckets().Contains(BucketName_2));
            TestTrue(TEXT("ListBuckets"), S3UEClient::ListBuckets().Contains(BucketName_3));
        });

        AfterEach([this]()
        {
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            std::string BucketNameStdString_1 = std::string(TCHAR_TO_UTF8(*BucketName_1));
            Aws::String BucketNameAwsString_1(BucketNameStdString_1.c_str(), BucketNameStdString_1.size());
            std::string BucketNameStdString_2 = std::string(TCHAR_TO_UTF8(*BucketName_2));
            Aws::String BucketNameAwsString_2(BucketNameStdString_2.c_str(), BucketNameStdString_2.size());
            std::string BucketNameStdString_3 = std::string(TCHAR_TO_UTF8(*BucketName_3));
            Aws::String BucketNameAwsString_3(BucketNameStdString_3.c_str(), BucketNameStdString_3.size());

            //Delete BucketName_1
            Aws::Client::ClientConfiguration Config;
            Config.region = RegionAwsString;

            Aws::S3::S3Client S3Client(Config);

            Aws::S3::Model::DeleteBucketRequest DeleteRequest_1;
            DeleteRequest_1.SetBucket(BucketNameAwsString_1);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome_1 =
                S3Client.DeleteBucket(DeleteRequest_1);

            if (!DeleteBucketOutcome_1.IsSuccess())
            {
                auto Err = DeleteBucketOutcome_1.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting bucket_1 fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            //Delete BucketName_2
            Aws::S3::Model::DeleteBucketRequest DeleteRequest_2;
            DeleteRequest_2.SetBucket(BucketNameAwsString_2);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome_2 =
                S3Client.DeleteBucket(DeleteRequest_2);

            if (!DeleteBucketOutcome_2.IsSuccess())
            {
                auto Err = DeleteBucketOutcome_2.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting bucket_2 fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            //Delete BucketName_3
            Aws::S3::Model::DeleteBucketRequest DeleteRequest_3;
            DeleteRequest_3.SetBucket(BucketNameAwsString_3);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome_3 =
                S3Client.DeleteBucket(DeleteRequest_3);

            if (!DeleteBucketOutcome_3.IsSuccess())
            {
                auto Err = DeleteBucketOutcome_3.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting bucket_3 fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });
    });

    Describe("CreateBucket()", [this]()
    {
        BeforeEach([this]()
        {
            Region = "us-west-2";
            BucketName = "ambit-createbucket-test";
        });

        Describe("With right region and unique bucket name", [this]()
        {
            It("should return true which represents creating bucket successfully", [this]()
            {
                TestTrue(
                    TEXT("CreateBucket"),
                    S3UEClient::CreateBucket(Region, BucketName));
            });

            AfterEach([this]()
            {
                std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
                Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

                std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
                Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

                //Delete Bucket
                Aws::Client::ClientConfiguration Config;
                Config.region = RegionAwsString;

                Aws::S3::S3Client S3Client(Config);

                Aws::S3::Model::DeleteBucketRequest DeleteRequest;
                DeleteRequest.SetBucket(BucketNameAwsString);

                Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome =
                    S3Client.DeleteBucket(DeleteRequest);

                if (!DeleteBucketOutcome.IsSuccess())
                {
                    auto Err = DeleteBucketOutcome.GetError();
                    UE_LOG(LogTemp, Error, TEXT("Deleting bucket fails: %s, %s"),
                           *FString(Err.GetExceptionName().c_str()),
                           *FString(Err.GetMessage().c_str()));
                }
            });
        });

        Describe("With us-east-1 region and not unique bucket name(exists in personal account)", [this]()
        {
            It("should catch the exception: BucketAlreadyExist", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT(
                            "Create Bucket: BucketAlreadyExists : The requested bucket name is not available. The bucket namespace is shared by all users of the system. Please select a different name and try again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::CreateBucket("us-east-1", "ambit-output-bucket");
                }
                catch (const std::runtime_error& Re)
                {
                    FString Err = Re.what();
                    TestEqual(
                        TEXT("The BucketAlreadyExist exception should be equal"),
                        Err,
                        "The requested bucket name is not available. The bucket namespace is shared by all users of the system. Please select a different name and try again."
                    );
                }
            });
        });

        Describe("With empty region or bucket name", [this]()
        {
            It("should catch invalid argument exception when region is empty", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT("The bucket name or region is empty. Please check them again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::CreateBucket("", BucketName);
                }
                catch (const std::invalid_argument& Ia)
                {
                    FString Err = Ia.what();
                    TestEqual(
                        TEXT("The invalid argument exception should be equal"),
                        Err,
                        "The bucket name or region is empty. Please check them again."
                    );
                }
            });

            It("should catch invalid argument exception when bucket name is empty", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT("The bucket name or region is empty. Please check them again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::CreateBucket(Region, "");
                }
                catch (const std::invalid_argument& Ia)
                {
                    FString Err = Ia.what();
                    TestEqual(
                        TEXT("The invalid argument exception should be equal"),
                        Err,
                        "The bucket name or region is empty. Please check them again."
                    );
                }
            });
        });
    });

    Describe("PutBucketEncryption()", [this]()
    {
        BeforeEach([this]()
        {
            Region = "us-west-2";
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            const Aws::S3::Model::BucketLocationConstraint& RegionBucketLocationConstraint =
                Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(RegionAwsString);

            BucketName = "ambit-putbucketencryption-test";

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            Aws::S3::S3Client S3Client;

            // Create a test bucket
            Aws::S3::Model::CreateBucketRequest CreateRequest;
            CreateRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig;
            BucketConfig.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest.SetCreateBucketConfiguration(BucketConfig);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome =
                S3Client.CreateBucket(CreateRequest);

            if (!CreateOutcome.IsSuccess())
            {
                auto Err = CreateOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating put-bucket-encryption-test fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });

        It("should return true because the bucket was encrypted", [this]()
        {
            S3UEClient::PutBucketEncryption(BucketName);
            Aws::S3::Model::GetBucketEncryptionRequest BucketEncryptionRequest;
            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            BucketEncryptionRequest.WithBucket(BucketAwsString);
            Aws::S3::S3Client S3Client;
            TestTrue(TEXT("GetBucketEncryption"),
                S3Client.GetBucketEncryption(BucketEncryptionRequest).IsSuccess());
        });

        It("should catch the exception because the bucket was not exist and cannot be put encryption", [this]()
        {
            FString NotExistingBucketName = "not-existing-bucket-name";
            while (S3UEClient::ListBuckets().Contains(NotExistingBucketName))
            {
                NotExistingBucketName += FString::FromInt(FMath::RandRange(0, 100));
            }
            try
            {
                AddExpectedError(TEXT("Put Bucket Encryption: NoSuchBucket : The specified bucket does not exist"),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::PutBucketEncryption(NotExistingBucketName);
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "The specified bucket does not exist");
            }
        });

        AfterEach([this]()
        {
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            //Delete BucketName
            Aws::Client::ClientConfiguration Config;
            Config.region = RegionAwsString;

            Aws::S3::S3Client S3Client(Config);

            Aws::S3::Model::DeleteBucketRequest DeleteRequest;
            DeleteRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome =
                S3Client.DeleteBucket(DeleteRequest);

            if (!DeleteBucketOutcome.IsSuccess())
            {
                auto Err = DeleteBucketOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });
    });

    Describe("ListObjects()", [this]()
    {
        BeforeEach([this]()
        {
            Region = "us-west-2";
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            const Aws::S3::Model::BucketLocationConstraint& RegionBucketLocationConstraint =
                Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(RegionAwsString);

            BucketName = "ambit-listobjects-test";

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            ObjectName = "PutTest.txt";
            ObjectContent = "This is the test file for S3UEClient.";
            std::string ObjectNameStdString = std::string(TCHAR_TO_UTF8(*ObjectName));
            Aws::String ObjectNameAwsString(ObjectNameStdString.c_str(), ObjectNameStdString.size());

            Aws::S3::S3Client S3Client;

            // Create a test bucket
            Aws::S3::Model::CreateBucketRequest CreateRequest;
            CreateRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig;
            BucketConfig.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest.SetCreateBucketConfiguration(BucketConfig);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome =
                S3Client.CreateBucket(CreateRequest);

            if (!CreateOutcome.IsSuccess())
            {
                auto Err = CreateOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating list-objects-test fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            // Put an object into the bucket
            Aws::S3::Model::PutObjectRequest PutObjectRequest;
            PutObjectRequest.SetBucket(BucketNameAwsString);
            PutObjectRequest.SetKey(ObjectNameAwsString);
            std::string ObjectContentString = std::string(TCHAR_TO_UTF8(*ObjectContent));

            const std::shared_ptr<Aws::IOStream> Input_Data =
                Aws::MakeShared<Aws::StringStream>("");
            *Input_Data << ObjectContentString.c_str();

            PutObjectRequest.SetBody(Input_Data);

            Aws::S3::Model::PutObjectOutcome PutObjectOutcome = S3Client.PutObject(PutObjectRequest);

            if (!PutObjectOutcome.IsSuccess())
            {
                auto Err = PutObjectOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Putting object into ambit-listobjects-test bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });

        It("should contain objects after uploading them with right region and bucket name.", [this]()
        {
            TestTrue(
                TEXT("ListObjects"),
                S3UEClient::ListObjects(Region, BucketName).Contains(ObjectName));
        });

        It("should catch exception about NetworkError when using wrong region", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("ListObjects:  : Encountered network error when sending http request"),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::ListObjects("us-east-1", BucketName);
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "Encountered network error when sending http request");
            }
        });

        It("should catch exception about NoSuchBucket when using wrong bucket name", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("ListObjects: NoSuchBucket : The specified bucket does not exist"),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::ListObjects(Region, "ambit-output-test");
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "The specified bucket does not exist");
            }
        });

        Describe("With empty region or bucket name", [this]()
        {
            It("should catch invalid argument exception when region is empty", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT("The bucket name or region is empty. Please check them again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::ListObjects("", BucketName);
                }
                catch (const std::invalid_argument& Ia)
                {
                    FString Err = Ia.what();
                    TestEqual(
                        TEXT("The invalid argument exception should be equal"),
                        Err,
                        "The bucket name or region is empty. Please check them again."
                    );
                }
            });

            It("should catch invalid argument exception when bucket name is empty", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT("The bucket name or region is empty. Please check them again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::ListObjects(Region, "");
                }
                catch (const std::invalid_argument& Ia)
                {
                    FString Err = Ia.what();
                    TestEqual(
                        TEXT("The invalid argument exception should be equal"),
                        Err,
                        "The bucket name or region is empty. Please check them again."
                    );
                }
            });
        });

        AfterEach([this]()
        {
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            std::string ObjectNameStdString = std::string(TCHAR_TO_UTF8(*ObjectName));
            Aws::String ObjectNameAwsString(ObjectNameStdString.c_str(), ObjectNameStdString.size());

            Aws::Client::ClientConfiguration Config;
            Config.region = RegionAwsString;

            Aws::S3::S3Client S3Client(Config);
            // Delete Object
            Aws::S3::Model::DeleteObjectRequest DeleteObjectRequest;

            DeleteObjectRequest.WithKey(ObjectNameAwsString)
                               .WithBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteObjectOutcome DeleteObjectOutcome =
                S3Client.DeleteObject(DeleteObjectRequest);

            if (!DeleteObjectOutcome.IsSuccess())
            {
                auto Err = DeleteObjectOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting the object in ambit-listobjects-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            //Delete Bucket
            Aws::S3::Model::DeleteBucketRequest DeleteRequest;
            DeleteRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome =
                S3Client.DeleteBucket(DeleteRequest);

            if (!DeleteBucketOutcome.IsSuccess())
            {
                auto Err = DeleteBucketOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting ambit-listobjects-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });
    });

    Describe("GetObjectAsString()", [this]()
    {
        BeforeEach([this]()
        {
            Region = "us-west-2";
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            const Aws::S3::Model::BucketLocationConstraint& RegionBucketLocationConstraint =
                Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(RegionAwsString);

            BucketName = "ambit-getobject-test";

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            ObjectName = "PutTest.txt";
            ObjectContent = "This is the test file for S3UEClient.";
            std::string ObjectNameStdString = std::string(TCHAR_TO_UTF8(*ObjectName));
            Aws::String ObjectNameAwsString(ObjectNameStdString.c_str(), ObjectNameStdString.size());

            Aws::S3::S3Client S3Client;

            // Create a test bucket
            Aws::S3::Model::CreateBucketRequest CreateRequest;
            CreateRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig;
            BucketConfig.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest.SetCreateBucketConfiguration(BucketConfig);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome =
                S3Client.CreateBucket(CreateRequest);

            if (!CreateOutcome.IsSuccess())
            {
                auto Err = CreateOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating ambit-getobject-test fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            // Put an object into the bucket
            Aws::S3::Model::PutObjectRequest PutObjectRequest;
            PutObjectRequest.SetBucket(BucketNameAwsString);
            PutObjectRequest.SetKey(ObjectNameAwsString);
            std::string ObjectContentString = std::string(TCHAR_TO_UTF8(*ObjectContent));

            const std::shared_ptr<Aws::IOStream> Input_Data =
                Aws::MakeShared<Aws::StringStream>("");
            *Input_Data << ObjectContentString.c_str();

            PutObjectRequest.SetBody(Input_Data);

            Aws::S3::Model::PutObjectOutcome PutObjectOutcome = S3Client.PutObject(PutObjectRequest);

            if (!PutObjectOutcome.IsSuccess())
            {
                auto Err = PutObjectOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Putting object into ambit-getobject-test bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });

        It("should return the text content with right Region, BucketName and ObjectName.", [this]()
        {
            TestEqual(
                TEXT("GetObjectAsString"),
                S3UEClient::GetObjectAsString(Region, BucketName, ObjectName),
                "This is the test file for S3UEClient.");
        });

        It("should return the text content with right Region, BucketName and ObjectName from the encrypted bucket.", [this]()
        {
            S3UEClient::PutBucketEncryption(BucketName);
            TestEqual(
                TEXT("GetObjectAsString from encrypted bucket"),
                S3UEClient::GetObjectAsString(Region, BucketName, ObjectName),
                "This is the test file for S3UEClient.");
        });

        It("should catch exception about NetworkError when using wrong region", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("GetObjectAsString:  : Encountered network error when sending http request"),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::GetObjectAsString("us-east-1", BucketName, ObjectName);
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "Encountered network error when sending http request");
            }
        });

        It("should catch exception about NoSuchBucket when using wrong bucket name", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("GetObjectAsString: NoSuchBucket : The specified bucket does not exist"),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::GetObjectAsString(Region, "ambit-output-test", ObjectName);
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "The specified bucket does not exist");
            }
        });

        It("should catch exception about NoSuchKey when using wrong object name", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("GetObjectAsString: NoSuchKey : The specified key does not exist."),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::GetObjectAsString(Region, BucketName, "WrongObject.txt");
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "The specified key does not exist.");
            }
        });

        Describe("When region, bucket name or object name is empty", [this]()
        {
            It("should catch invalid argument exception when region is empty", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT("The region, bucket name or object name is empty. Please check them again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::GetObjectAsString("", BucketName, ObjectName);
                }
                catch (const std::invalid_argument& Ia)
                {
                    FString Err = Ia.what();
                    TestEqual(
                        TEXT("The invalid argument exception should be equal"),
                        Err,
                        "The region, bucket name or object name is empty. Please check them again."
                    );
                }
            });

            It("should catch invalid argument exception when bucket name is empty", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT("The region, bucket name or object name is empty. Please check them again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::GetObjectAsString(Region, "", ObjectName);
                }
                catch (const std::invalid_argument& Ia)
                {
                    FString Err = Ia.what();
                    TestEqual(
                        TEXT("The invalid argument exception should be equal"),
                        Err,
                        "The region, bucket name or object name is empty. Please check them again."
                    );
                }
            });

            It("should catch invalid argument exception when object name is empty", [this]()
            {
                try
                {
                    AddExpectedError(
                        TEXT("The region, bucket name or object name is empty. Please check them again."),
                        EAutomationExpectedErrorFlags::Exact, 1);
                    S3UEClient::GetObjectAsString(Region, BucketName, "");
                }
                catch (const std::invalid_argument& Ia)
                {
                    FString Err = Ia.what();
                    TestEqual(
                        TEXT("The invalid argument exception should be equal"),
                        Err,
                        "The region, bucket name or object name is empty. Please check them again."
                    );
                }
            });
        });

        AfterEach([this]()
        {
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            std::string ObjectNameStdString = std::string(TCHAR_TO_UTF8(*ObjectName));
            Aws::String ObjectNameAwsString(ObjectNameStdString.c_str(), ObjectNameStdString.size());

            Aws::Client::ClientConfiguration Config;
            Config.region = RegionAwsString;

            Aws::S3::S3Client S3Client(Config);
            // Delete Object
            Aws::S3::Model::DeleteObjectRequest DeleteObjectRequest;

            DeleteObjectRequest.WithKey(ObjectNameAwsString)
                               .WithBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteObjectOutcome DeleteObjectOutcome =
                S3Client.DeleteObject(DeleteObjectRequest);

            if (!DeleteObjectOutcome.IsSuccess())
            {
                auto Err = DeleteObjectOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting the object in ambit-getobject-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            //Delete Bucket
            Aws::S3::Model::DeleteBucketRequest DeleteRequest;
            DeleteRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome =
                S3Client.DeleteBucket(DeleteRequest);

            if (!DeleteBucketOutcome.IsSuccess())
            {
                auto Err = DeleteBucketOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting ambit-getobject-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });
    });

    Describe("PutObject()", [this]()
    {
        BeforeEach([this]()
        {
            Region = "us-west-2";
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            const Aws::S3::Model::BucketLocationConstraint& RegionBucketLocationConstraint =
                Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(RegionAwsString);

            BucketName = "ambit-putobject-test";

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            ObjectName = "PutTest.txt";
            ObjectContent = "This is the test file for S3UEClient.";
            std::string ObjectNameStdString = std::string(TCHAR_TO_UTF8(*ObjectName));
            Aws::String ObjectNameAwsString(ObjectNameStdString.c_str(), ObjectNameStdString.size());

            Aws::S3::S3Client S3Client;

            // Create a test bucket
            Aws::S3::Model::CreateBucketRequest CreateRequest;
            CreateRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig;
            BucketConfig.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest.SetCreateBucketConfiguration(BucketConfig);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome =
                S3Client.CreateBucket(CreateRequest);

            if (!CreateOutcome.IsSuccess())
            {
                auto Err = CreateOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating ambit-putobject-test fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });

        It("should return true which represents putting object into the bucket successfully.", [this]()
        {
            TestEqual(
                TEXT(
                    "PutObject"),
                S3UEClient::PutObject(Region, BucketName, ObjectName, ObjectContent), true);
        });

        It("should catch exception about NetworkError when using wrong region", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("PutObject:  : Encountered network error when sending http request"),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::PutObject("us-east-1", BucketName, ObjectName, ObjectContent);
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "Encountered network error when sending http request");
            }
        });

        It("should catch exception about NoSuchBucket when using wrong bucket name", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("PutObject: NoSuchBucket : The specified bucket does not exist"),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::PutObject(Region, "ambit-output-test", ObjectName, ObjectContent);
            }
            catch (const std::runtime_error& Re)
            {
                FString Err = Re.what();
                TestEqual(
                    TEXT("The catched exception should be equal to expected"),
                    Err,
                    "The specified bucket does not exist");
            }
        });

        It("should catch invalid argument exception when region is empty", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("The region, bucket name or object name is empty. Please check them again."),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::PutObject("", BucketName, ObjectName, ObjectContent);
            }
            catch (const std::invalid_argument& Ia)
            {
                FString Err = Ia.what();
                TestEqual(
                    TEXT("The invalid argument exception should be equal"),
                    Err,
                    "The region, bucket name or object name is empty. Please check them again."
                );
            }
        });

        It("should catch invalid argument exception when bucket name is empty", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("The region, bucket name or object name is empty. Please check them again."),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::PutObject(Region, "", ObjectName, ObjectContent);
            }
            catch (const std::invalid_argument& Ia)
            {
                FString Err = Ia.what();
                TestEqual(
                    TEXT("The invalid argument exception should be equal"),
                    Err,
                    "The region, bucket name or object name is empty. Please check them again."
                );
            }
        });

        It("should catch invalid argument exception when object name is empty", [this]()
        {
            try
            {
                AddExpectedError(
                    TEXT("The region, bucket name or object name is empty. Please check them again."),
                    EAutomationExpectedErrorFlags::Exact, 1);
                S3UEClient::PutObject(Region, BucketName, "", ObjectContent);
            }
            catch (const std::invalid_argument& Ia)
            {
                FString Err = Ia.what();
                TestEqual(
                    TEXT("The invalid argument exception should be equal"),
                    Err,
                    "The region, bucket name or object name is empty. Please check them again."
                );
            }
        });

        AfterEach([this]()
        {
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            std::string ObjectNameStdString = std::string(TCHAR_TO_UTF8(*ObjectName));
            Aws::String ObjectNameAwsString(ObjectNameStdString.c_str(), ObjectNameStdString.size());

            Aws::Client::ClientConfiguration Config;
            Config.region = RegionAwsString;

            Aws::S3::S3Client S3Client(Config);

            //Delete Object
            Aws::S3::Model::DeleteObjectRequest DeleteObjectRequest;

            DeleteObjectRequest.WithKey(ObjectNameAwsString)
                               .WithBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteObjectOutcome DeleteObjectOutcome =
                S3Client.DeleteObject(DeleteObjectRequest);

            if (!DeleteObjectOutcome.IsSuccess())
            {
                auto Err = DeleteObjectOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting the object in ambit-putobject-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            //Delete Bucket
            Aws::S3::Model::DeleteBucketRequest DeleteRequest;
            DeleteRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome =
                S3Client.DeleteBucket(DeleteRequest);

            if (!DeleteBucketOutcome.IsSuccess())
            {
                auto Err = DeleteBucketOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting ambit-putobject-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });
    });

    Describe("PutLocalObject()", [this]()
    {
        BeforeEach([this]()
        {
            Region = "us-west-2";
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            const Aws::S3::Model::BucketLocationConstraint& RegionBucketLocationConstraint =
                Aws::S3::Model::BucketLocationConstraintMapper::GetBucketLocationConstraintForName(RegionAwsString);

            BucketName = "ambit-putlocalobject-test";

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            FString FileContent = "Test PutLocalObject";
            ObjectName = "PutLocalObjectTest.txt";
            FilePath = FPaths::Combine(*FPaths::ProjectIntermediateDir(), *ObjectName);
            FFileHelper::SaveStringToFile(FileContent, *FilePath,
                                          FFileHelper::EEncodingOptions::AutoDetect,
                                          &IFileManager::Get(),
                                          FILEWRITE_None);

            Aws::S3::S3Client S3Client;

            // Create a test bucket
            Aws::S3::Model::CreateBucketRequest CreateRequest;
            CreateRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::CreateBucketConfiguration BucketConfig;
            BucketConfig.SetLocationConstraint(RegionBucketLocationConstraint);

            CreateRequest.SetCreateBucketConfiguration(BucketConfig);

            Aws::S3::Model::CreateBucketOutcome CreateOutcome =
                S3Client.CreateBucket(CreateRequest);

            if (!CreateOutcome.IsSuccess())
            {
                auto Err = CreateOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Creating ambit-putlocalobject-test fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });

        It("should return true which represents putting local object into the bucket successfully.", [this]()
        {
            TestEqual(
                TEXT(
                    "PutLocalObject"),
                S3UEClient::PutLocalObject(Region, BucketName, ObjectName, FilePath), true);
        });

        AfterEach([this]()
        {
            std::string RegionStdString = std::string(TCHAR_TO_UTF8(*Region));
            Aws::String RegionAwsString(RegionStdString.c_str(), RegionStdString.size());

            std::string BucketNameStdString = std::string(TCHAR_TO_UTF8(*BucketName));
            Aws::String BucketNameAwsString(BucketNameStdString.c_str(), BucketNameStdString.size());

            std::string ObjectNameStdString = std::string(TCHAR_TO_UTF8(*ObjectName));
            Aws::String ObjectNameAwsString(ObjectNameStdString.c_str(), ObjectNameStdString.size());

            Aws::Client::ClientConfiguration Config;
            Config.region = RegionAwsString;

            Aws::S3::S3Client S3Client(Config);

            //Delete Local File
            FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*FilePath);

            //Delete Object
            Aws::S3::Model::DeleteObjectRequest DeleteObjectRequest;

            DeleteObjectRequest.WithKey(ObjectNameAwsString)
                               .WithBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteObjectOutcome DeleteObjectOutcome =
                S3Client.DeleteObject(DeleteObjectRequest);

            if (!DeleteObjectOutcome.IsSuccess())
            {
                auto Err = DeleteObjectOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting the object in ambit-putobject-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }

            //Delete Bucket
            Aws::S3::Model::DeleteBucketRequest DeleteRequest;
            DeleteRequest.SetBucket(BucketNameAwsString);

            Aws::S3::Model::DeleteBucketOutcome DeleteBucketOutcome =
                S3Client.DeleteBucket(DeleteRequest);

            if (!DeleteBucketOutcome.IsSuccess())
            {
                auto Err = DeleteBucketOutcome.GetError();
                UE_LOG(LogTemp, Error, TEXT("Deleting ambit-putobject-bucket fails: %s, %s"),
                       *FString(Err.GetExceptionName().c_str()),
                       *FString(Err.GetMessage().c_str()));
            }
        });
    });
}
