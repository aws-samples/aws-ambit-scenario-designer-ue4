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

#pragma once

#include "DropdownMenuBase.h"

/**
 * This class is a base class to extend when you are building a dropdown menu that
 * will choose between FStrings. This class is intended be extended and for the user
 * to implement GetMenuItem and OnChangeEvent for their individual use case.
 */
template <typename M>
class TFStringDropdownMenuBase : public TFDropdownMenuBase<FString, M>
{
public:
    /**
     * Changes and FString into FText and returns
     */
    static FText GetDefaultValueAsText()
    {
        return FText::FromString(M::GetMenuItem());
    }

    /**
     * Gets the current value (FString type) and converts to FText
     */
    static FText GetCurrentValueAsText(TSharedRef<IPropertyHandle> PropertyHandle)
    {
        FString CurrentValue = "";
        const FPropertyAccess::Result Result = PropertyHandle->GetValue(CurrentValue);
        check(Result == FPropertyAccess::Success);

        return FText::FromString(CurrentValue);
    }
};
