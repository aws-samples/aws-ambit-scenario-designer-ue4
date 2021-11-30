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

#include "PropertyHandle.h"

/**
 * This templated class is to represent the base of implementing a dropdown menu
 * T will be the variable type of the menu items and M is the class that
 * will be implemented by each menu.
 *
 * Serves as an easy way for one to extend this class to develop a menu and know what is required
 */
template <typename T, typename M>
class TFDropdownMenuBase
{
public:

    /**
     * Array of Values for the menu to swap between
     **/
    static TArray<T> Values;

    /**
    * Virtual method to return the variable that is to be modified by this menu
    */
    static T GetMenuItem()
    {
        return M::GetMenuItem();
    }

    /**
     * Returns the localized text form of what the default menu option
     */
    static FText GetDefaultValueAsText()
    {
        return M::GetDefaultValueAsText();
    }

    /**
     * Returns the localized text form of what the current selected menu option
     */
    static FText GetCurrentValueAsText(TSharedRef<IPropertyHandle> PropertyHandle)
    {
        return M::GetCurrentValueAsText(PropertyHandle);
    }

    /**
     * Defines the logic of what happens when the value of the menu changes
     */
    static void OnChangeEvent(TSharedRef<IPropertyHandle> PropertyHandle, T NewValue)
    {
        M::OnChangeEvent(PropertyHandle, NewValue);
    }

    /**
     * Builds the widget which includes all the options for the menu
     */
    static TSharedRef<SWidget> GetMenu(TSharedRef<IPropertyHandle> PropertyHandle)
    {
        return M::GetMenu(PropertyHandle);
    }
};
