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
 * A collection of helpers functions that make building menus and popups easier.
 */
namespace FMenuHelpers
{
    /**
     * Pop up a window for the user to acknowledge
     *
     * @param Message The message to display to the user
     * @param Level the level of error usually Info, Warning, Error
     */
    AMBITUTILS_API void DisplayMessagePopup(const FString& Message, const FString& Level);

    /**
     * Log an Error and popup a window to the user
     *
     *@param Message the message to log and print
     */
    AMBITUTILS_API void LogErrorAndPopup(const FString& Message);

    /** 
    * Create and display a notification message to the user
    * 
    * @param MessageText The message to display to the user
    * @param Image The image for the Ambit logo notification icon
    * @param FadeInDuration The fade in duration for this element
    * @param FadeOutDuration The fade out duration for this element
    * @param ExpireDuration The duration before a fadeout for this element
    * @param bFireAndForget When true the notification will automatically time out after the expire duration.
    */
    AMBITUTILS_API void CreateNotification(const FText& MessageText, const FSlateBrush* Image,
                                           float FadeInDuration = 0.0f, float FadeOutDuration = 2.0f,
                                           float ExpireDuration = 2.0f, bool bFireAndForget = true);
};
