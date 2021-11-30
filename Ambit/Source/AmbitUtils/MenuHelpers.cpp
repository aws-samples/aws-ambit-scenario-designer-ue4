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

#include "MenuHelpers.h"
#include "AmbitUtilsModule.h"
#include "Misc/MessageDialog.h"

#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

void FMenuHelpers::DisplayMessagePopup(FString Message, FString Level)
{
    FText MessageText = FText::FromString(Message);
    FText TitleText = FText::FromString(Level);
    FMessageDialog::Open(EAppMsgType::Ok, MessageText, &TitleText);
}

void FMenuHelpers::LogErrorAndPopup(FString Message)
{
    UE_LOG(LogAmbitUtils, Error, TEXT("%s"), *Message);
    DisplayMessagePopup(Message, "Error");
}

void FMenuHelpers::CreateNotification(FText MessageText, const FSlateBrush* Image, 
    float FadeInDuration, float FadeOutDuration, float ExpireDuration, bool bFireAndForget)
{
    FNotificationInfo Info(MessageText);
    Info.bFireAndForget = bFireAndForget;
    Info.FadeInDuration = FadeInDuration;
    Info.FadeOutDuration = FadeOutDuration;
    Info.ExpireDuration = ExpireDuration;

    if (Image)
    {
        Info.Image = Image;
    }
    FSlateNotificationManager::Get().AddNotification(Info);
}