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
#include "Subsystems/EngineSubsystem.h"

#include <AWSUE4Module/Public/FirehoseUE4Client.h>

#include "Dom/JsonObject.h"


#include "UserMetricsSubsystem.generated.h"

/**
 * UserMetrics Subsystem which extends the engine subsystem meaning this will be a singleton instance with lifetime in both editor and runtime
 */
UCLASS()
class AMBIT_API UUserMetricsSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    void Initialize(FSubsystemCollectionBase& Collection) override;
    void Deinitialize() override;

    /**
     * The actual metrics tracking function which can be used any place where it worth tracking user events
     *
     *@param EventName event name for this user event.
     *@param NameSpace namespace which this event belongs to (like AmbitSpawner/AmbitMode etc).
     *@param CustomData any custom data we'd like to store along with the event. This is stored in json format.
     */
    void Track(FString EventName, FString NameSpace, TSharedPtr<FJsonObject> CustomData = MakeShareable(new FJsonObject));

private:
    bool SendEvents(float deltatime);

    //AWS Firehose client used to send metrics
    TUniquePtr<FirehoseUE4Client> FirehoseClient;

    //Records buffer which will be flushed periodically
    TQueue<TSharedPtr<FJsonObject>> Records;

    //EventNumber indicating the sequence of user actions
    int32 EventNumber;

    //Unique instance Id generated for each engine usage instance
    FString UUID;

    FString PluginVersion;

    FString EngineVersion;

    FDelegateHandle SendEventsTickerHandle;

    bool RecordUserMetrics = true;
};
