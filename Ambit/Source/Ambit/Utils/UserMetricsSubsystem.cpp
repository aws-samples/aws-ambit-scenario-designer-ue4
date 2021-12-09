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

#include "UserMetricsSubsystem.h"

#include "Interfaces/IPluginManager.h"

#include <utility>
#include <AWSUE4Module/Public/FirehoseUE4Client.h>

#include "Ambit/Mode/Constant.h"
#include "AmbitUtils/JsonHelpers.h"

void UUserMetricsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    FirehoseClient = MakeUnique<FirehoseUE4Client>(UserMetrics::KFirehoseRegion, UserMetrics::KCognitoAccountID,
                                                   UserMetrics::KCognitoIdentityPoolID);

    //Send Metrics every 30 seconds
    SendEventsTickerHandle = FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &UUserMetricsSubsystem::SendEvents), 30);

    EventNumber = 0;

    //Generate UUID for each running instance
    UUID = FGuid::NewGuid().ToString();

    IPluginManager& Manager = IPluginManager::Get();
    EngineVersion = Manager.FindPlugin("Ambit")->GetDescriptor().EngineVersion;
    PluginVersion = Manager.FindPlugin("Ambit")->GetDescriptor().VersionName;

    this->Track(UserMetrics::AmbitGeneral::KAmbitStartEngineEvent, UserMetrics::AmbitGeneral::KAmbitGeneralNameSpace);
}

void UUserMetricsSubsystem::Deinitialize()
{
    if (FirehoseClient.IsValid())
    {
        FirehoseClient.Release();
    }

    if (SendEventsTickerHandle.IsValid())
    {
        FTicker::GetCoreTicker().RemoveTicker(SendEventsTickerHandle);
    }
}

void UUserMetricsSubsystem::Track(const FString& EventName, const FString& NameSpace,
                                  TSharedPtr<FJsonObject> Data)
{
    const TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);

    Json->SetStringField(UserMetrics::KEventInstanceIDKey, UUID);
    Json->SetStringField(UserMetrics::KEventDateTimeKey, FDateTime::UtcNow().ToString(TEXT("%Y-%m-%d %H:%M:%S")));
    Json->SetNumberField(UserMetrics::KEventNumberKey, EventNumber);

    Json->SetStringField(UserMetrics::KEventNameKey, EventName);
    Json->SetStringField(UserMetrics::KEventNameSpaceKey, NameSpace);
    Json->SetStringField(UserMetrics::KEventContextDataKey, FJsonHelpers::SerializeJsonCondense(std::move(Data)));

    Json->SetStringField(UserMetrics::KEventEngineVersionKey, EngineVersion);
    Json->SetStringField(UserMetrics::KEventPluginVersionKey, PluginVersion);

    Records.Enqueue(Json);

    EventNumber++;
}

bool UUserMetricsSubsystem::SendEvents(float DeltaTime)
{
    if (RecordUserMetrics)
    {
        TArray<FString> RecordsData;

        for (int i = 0; i < UserMetrics::KMaxFirehoseRecordPerBatch; i++)
        {
            TSharedPtr<FJsonObject> Json;
            if (Records.Dequeue(Json))
            {
                //Add newline to the end so that records can be identified by analytic tools
                RecordsData.Add(FJsonHelpers::SerializeJsonCondense(Json) + "\n");
            }
            else
            {
                break;
            }
        }

        if (RecordsData.Num() > 0 && FirehoseClient.IsValid())
        {
            FirehoseClient->SendEvents(UserMetrics::KFirehoseStreamName, RecordsData);
        }
    }
    return true;
}
