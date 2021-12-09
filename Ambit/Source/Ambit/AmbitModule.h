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
#include "Styling/ISlateStyle.h"
#include "Styling/SlateStyle.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAmbit, Log, All);

class FToolBarBuilder;

class FAmbitModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    void StartupModule() override;

    void ShutdownModule() override;

    // adding mode
    static void Initialize();

    static TSharedPtr<class ISlateStyle> GetStyleSet();

    static FName GetStyleSetName();

    /** Returns singleton instance of the FAmbitModule */
    static FAmbitModule& Get();

    const FSlateBrush* GetAmbitNotificationBrush() const
    {
        return AmbitNotificationBrush;
    }

    /**
    * Calls a function to create and display a notification 
    * with the Ambit logo as its icon
    *
    * @param MessageText The message to display to the user
    * @param FadeInDuration The fade in duration for this element
    * @param FadeOutDuration The fade out duration for this element
    * @param ExpireDuration The duration before a fadeout for this element
    * @param bFireAndForget When true the notification will automatically time out after the expire duration.
    */
    static void CreateAmbitNotification(const FText& MessageText, float FadeInDuration = 0.0f,
                                        float FadeOutDuration = 2.0f, float ExpireDuration = 5.0f,
                                        bool bFireAndForget = true);

private:
    // adding mode
    static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

    static TSharedPtr<class FSlateStyleSet> StyleSet;
    static FAmbitModule* FAmbitModuleInstance;
    const FSlateBrush* AmbitNotificationBrush = nullptr;

    template <typename ClassType>
    void RegisterPlaceableItem(const FName& UniqueHandle);
};
