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

#include "FirehoseUE4Client.h"

#include "AWSUEStringUtils.h"

#include <aws/cognito-identity/CognitoIdentityClient.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/firehose/FirehoseClient.h>
#include <aws/firehose/model/PutRecordBatchRequest.h>
#include <aws/identity-management/auth/CognitoCachingCredentialsProvider.h>

static const char GAllocation_Tag[] = "FirehoseUE4Client";

FirehoseUE4Client::FirehoseUE4Client(const FString& Region)
{
    const Aws::String AWSRegion = AWSUEStringUtils::FStringToAwsString(Region);

    Aws::Client::ClientConfiguration Config;
    Config.region = AWSRegion;

    Client = Aws::MakeUnique<Aws::Firehose::FirehoseClient>(GAllocation_Tag, Config);
}

FirehoseUE4Client::FirehoseUE4Client(const FString& Region, const FString& AccountId, const FString& IdentityPoolId)
{
    const Aws::String AWSRegion = AWSUEStringUtils::FStringToAwsString(Region);
    const Aws::String CognitoAccountId = AWSUEStringUtils::FStringToAwsString(AccountId);
    const Aws::String CognitoIdentityPoolId = AWSUEStringUtils::FStringToAwsString(IdentityPoolId);

    Aws::Client::ClientConfiguration Config;
    Config.region = AWSRegion;

    auto IdentityClient = Aws::MakeShared<Aws::CognitoIdentity::CognitoIdentityClient>(
        GAllocation_Tag, Aws::MakeShared<Aws::Auth::AnonymousAWSCredentialsProvider>(GAllocation_Tag), Config);
    auto CredentialsProvider = Aws::MakeShared<Aws::Auth::CognitoCachingAnonymousCredentialsProvider>(
        GAllocation_Tag, CognitoAccountId, CognitoIdentityPoolId, IdentityClient);

    Client = Aws::MakeUnique<Aws::Firehose::FirehoseClient>(GAllocation_Tag, CredentialsProvider, Config);
}

void FirehoseUE4Client::SendEvents(const FString& StreamName, const TArray<FString>& Data) const
{
    const Aws::String DeliveryStreamName = AWSUEStringUtils::FStringToAwsString(StreamName);

    Aws::Firehose::Model::PutRecordBatchRequest Request;
    Request.SetDeliveryStreamName(DeliveryStreamName);
    Aws::Vector<Aws::Firehose::Model::Record> Records;

    for (const FString& Element : Data)
    {
        Aws::Firehose::Model::Record Record;

        Aws::Utils::ByteBuffer bytes(reinterpret_cast<unsigned char*>(TCHAR_TO_UTF8(*Element)), Element.Len());

        Record.SetData(bytes);
        Records.emplace_back(Record);
    }

    Request.SetRecords(Records);

    Client->PutRecordBatchAsync(Request, [](const Aws::Firehose::FirehoseClient* /*unused*/,
                                            const Aws::Firehose::Model::PutRecordBatchRequest& /*unused*/,
                                            const Aws::Firehose::Model::PutRecordBatchOutcome& Outcome,
                                            const std::shared_ptr<const Aws::Client::AsyncCallerContext>& /*unused*/)
    {
        if (!Outcome.IsSuccess())
        {
            const auto Err = Outcome.GetError();
            UE_LOG(LogAWSUE4Module, Error, TEXT("Firehose SendEvents: %s : %s"),
                   *FString(Err.GetExceptionName().c_str()), *FString(Err.GetMessage().c_str()));
        }
    });
}
