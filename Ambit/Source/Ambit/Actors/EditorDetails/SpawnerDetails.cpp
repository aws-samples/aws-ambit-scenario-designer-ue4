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

#include "SpawnerDetails.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "EngineUtils.h"
#include "PropertyCustomizationHelpers.h"
#include "SlateOptMacros.h"

#include "Ambit/Actors/Spawners/SpawnWithHoudini.h"

TSharedRef<IDetailCustomization> FSpawnerDetails::MakeInstance()
{
    return MakeShareable(new FSpawnerDetails);
}

void FSpawnerDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    // Get all components which are being customized.
    TArray<TWeakObjectPtr<UObject>> ObjectsCustomized;
    DetailBuilder.GetObjectsBeingCustomized(ObjectsCustomized);

    // Extract the Houdini Asset Component to detail
    for (auto WeakObject : ObjectsCustomized)
    {
        if (WeakObject.IsValid())
        {
            TWeakObjectPtr<ASpawnWithHoudini> Spawner = Cast<ASpawnWithHoudini>(WeakObject);
            if (Spawner.IsValid())
            {
                AddSpawnerButtonForObject(Spawner, DetailBuilder);
            }
        }
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FSpawnerDetails::AddSpawnerButtonForObject(TWeakObjectPtr<ASpawnWithHoudini> Spawner,
                                                IDetailLayoutBuilder& DetailBuilder) const
{
    const FName KCategoryName = "Ambit Spawner";
    const FString KSpawnerGroup = "SpawnerActions";
    const FString KGenerate = "Generate Obstacles";
    const FString KClear = "Clear Obstacles";
    const FString KHintTextGenerateObstacles = "Generate Random Obstacle Locations";
    const FString KHintTextClearObstacles = "Remove All Obstacles Generated";

    const int KPaddingLeft = 1;
    const int KPaddingRemainingSides = 0;
    const int KMarginHorizontal = 4;
    const int KMarginVertical = 0;

    auto OnGenerateClicked = [Spawner]()
    {
        Spawner->GenerateObstacles();

        return FReply::Handled();
    };

    auto OnClearClicked = [Spawner]()
    {
        Spawner->ClearObstacles();

        return FReply::Handled();
    };

    // Obstacle Spawner Buttons
    IDetailCategoryBuilder& SpawnerSettingsCategory = DetailBuilder.
            EditCategory(KCategoryName);

    SpawnerSettingsCategory.AddCustomRow(
        FText::FromString(KSpawnerGroup))
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
          .HAlign(HAlign_Center)
          .AutoWidth()
          .Padding(KPaddingLeft, KPaddingRemainingSides, KPaddingRemainingSides, KPaddingRemainingSides)
        [
            SNew(SButton)
            .ContentPadding(FMargin(KMarginHorizontal, KMarginVertical))
        .Text(FText::FromString(KGenerate))
        .OnClicked_Lambda(OnGenerateClicked)
        .ToolTipText(FText::FromString(KHintTextGenerateObstacles))
        ]
        + SHorizontalBox::Slot()
          .HAlign(HAlign_Center)
          .AutoWidth()
          .Padding(KPaddingLeft, KPaddingRemainingSides, KPaddingRemainingSides, KPaddingRemainingSides)
        [
            SNew(SButton)
            .ContentPadding(FMargin(KMarginHorizontal, KMarginVertical))
        .Text(FText::FromString(KClear))
        .OnClicked_Lambda(OnClearClicked)
        .ToolTipText(FText::FromString(KHintTextClearObstacles))
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
