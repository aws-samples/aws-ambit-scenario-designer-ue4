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

#include "Ambit/Mode/ConfigImportExport.h"

#include "IDetailCustomization.h"
#include "PropertyHandle.h"
#include "Templates/SharedPointer.h"

class FAmbitMode;
class USpawnedObjectConfig;

class FAmbitDetailCustomization : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance();

    //IDetailCustomization interface
    void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
    void PendingDelete() override;
    //End of IDetailCustomization interface

    static FText UpdateNumberOfPermutations();

private:
    // General Methods
    static FText GetPropertyValueText(TSharedRef<IPropertyHandle> PropertyHandle);
    static void SetPropertyValueString(const FText& NewValue, ETextCommit::Type CommitInfo,
                                       TSharedRef<IPropertyHandle> PropertyHandle);

    // Permutation Options
    static void SetBulkPedestrianDensityMin(float);
    static TOptional<float> GetBulkPedestrianDensityMin();
    static void SetBulkPedestrianDensityMax(float);
    static TOptional<float> GetBulkPedestrianDensityMax();

    static void SetBulkTrafficDensityMin(float);
    static TOptional<float> GetBulkTrafficDensityMin();
    static void SetBulkTrafficDensityMax(float);
    static TOptional<float> GetBulkTrafficDensityMax();

    static void OnCheckStateChanged_Morning(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Morning();
    static void OnCheckStateChanged_Noon(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Noon();
    static void OnCheckStateChanged_Evening(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Evening();
    static void OnCheckStateChanged_Night(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Night();

    static void OnCheckStateChanged_Sunny(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Sunny();
    static void OnCheckStateChanged_Rainy(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Rainy();
    static void OnCheckStateChanged_Foggy(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Foggy();

    // Export Map
    static void OnCheckStateChanged_Linux(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Linux();
    static void OnCheckStateChanged_Windows(ECheckBoxState InState);
    static ECheckBoxState GetCheckState_Windows();
    static bool GetExportButtonEnabled();
};
