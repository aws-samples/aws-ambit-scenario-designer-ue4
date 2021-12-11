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

#include "TimeOfDayDropdownMenu.h"

#include "Constant.h"

#include "AmbitMode.h"
#include "AmbitObject.h"

TArray<FString> FTimeOfDayDropdownMenu::Values = {
    TimeOfDay::KCustom,
    TimeOfDay::KMorning,
    TimeOfDay::KMidDay,
    TimeOfDay::KEvening,
    TimeOfDay::KNight
};

FString FTimeOfDayDropdownMenu::GetMenuItem()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    if (AmbitMode != nullptr)
    {
        return AmbitMode->UISettings->PresetTimeOfDay;
    }

    return FString("---");
}

void FTimeOfDayDropdownMenu::OnChangeEvent(TSharedRef<IPropertyHandle> PropertyHandle, FString NewValue)
{
    ensure(PropertyHandle->SetValue(NewValue) == FPropertyAccess::Success);

    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    if (AmbitMode == nullptr)
    {
        // TODO: throw errors
        return;
    }

    if (NewValue == TimeOfDay::KMorning)
    {
        AmbitMode->UISettings->TimeOfDay = 6.0f;
    }
    else if (NewValue == TimeOfDay::KMidDay)
    {
        AmbitMode->UISettings->TimeOfDay = 12.0f;
    }
    else if (NewValue == TimeOfDay::KEvening)
    {
        AmbitMode->UISettings->TimeOfDay = 18.0f;
    }
    else if (NewValue == TimeOfDay::KNight)
    {
        AmbitMode->UISettings->TimeOfDay = 0.0f;
    }
}

TSharedRef<SWidget> FTimeOfDayDropdownMenu::GetMenu(TSharedRef<IPropertyHandle> PropertyHandle)
{
    FMenuBuilder MenuBuilder(true, nullptr);

    for (int32 i = 0; i < Values.Num(); i++)
    {
        MenuBuilder.AddMenuEntry(FText::FromString(Values[i]), FText::GetEmpty(), FSlateIcon(),
                                 FExecuteAction::CreateStatic(&OnChangeEvent, PropertyHandle, Values[i]));
    }

    return MenuBuilder.MakeWidget();
}
