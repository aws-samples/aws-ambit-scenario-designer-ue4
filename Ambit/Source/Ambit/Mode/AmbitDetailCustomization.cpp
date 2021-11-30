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

#include "AmbitDetailCustomization.h"

#include "AmbitMode.h"
#include "AmbitObject.h"

#include "AWSRegionDropdownMenu.h"
#include "GltfFileTypeDropdownMenu.h"
#include "PropertyEditing.h"
#include "SlateOptMacros.h"
#include "TimeOfDayDropdownMenu.h"
#include "WeatherDropdownMenu.h"
#include "WeatherTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SWrapBox.h"

#define LOCTEXT_NAMESPACE "AmbitDetail_Customization"

// Note: Please do not auto-format this entire file.
// Only auto-format new code you are adding.
// If you are adding Slate code, you may need to manually format it.

// INFO: Replace with inline static in header file once available in Unreal.
static UConfigImportExport* ConfigExporter;

TSharedRef<IDetailCustomization> FAmbitDetailCustomization::MakeInstance()
{
    return MakeShareable(new FAmbitDetailCustomization);
}

void FAmbitDetailCustomization::PendingDelete()
{
    ConfigExporter = nullptr;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FAmbitDetailCustomization::CustomizeDetails(
    IDetailLayoutBuilder& DetailBuilder)
{
    TSet<UClass*> Classes;

    TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
    DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

    for (auto WeakObject : ObjectsBeingCustomized)
    {
        if (UObject* Instance = WeakObject.Get())
        {
            Classes.Add(Instance->GetClass());
        }
    }

    ConfigExporter = NewObject<UConfigImportExport>();

    // The padding setting for Check boxes
    const FMargin KStandardPadding(6.f, 3.f);

    // Build Scenario Settings Menu
    // The order that you add properties here is the order they actually appear
    IDetailCategoryBuilder& ScenarioSettingsCategory = DetailBuilder.
        EditCategory("Scenario Settings");

    // Format Scenario Name and Import Button
    const FString KHintTextScenarioName = "Import Scenario Definition File or Type Name for New Scenario";
    TSharedRef<IPropertyHandle> PropertyHandle_ScenarioName = DetailBuilder.
        GetProperty(GET_MEMBER_NAME_CHECKED(UAmbitObject, ScenarioName));
    ScenarioSettingsCategory.AddProperty(PropertyHandle_ScenarioName)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_ScenarioName->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(200.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(SEditableTextBox)
                .Text_Static(&GetPropertyValueText, PropertyHandle_ScenarioName)
                .OnTextCommitted_Static(&SetPropertyValueString,
                                        PropertyHandle_ScenarioName)
                .HintText(FText::FromString(KHintTextScenarioName))
                .ToolTipText(FText::FromString(KHintTextScenarioName))
            ]
            + SHorizontalBox::Slot()
              .AutoWidth()
              .Padding(1, 0, 0, 0)
            [
                SNew(SButton)
                .ContentPadding(FMargin(4, 0))
                .Text(NSLOCTEXT("UnrealEd", "GenericOpenDialog", "..."))
                .OnClicked_UObject(ConfigExporter, &UConfigImportExport::OnImportSdf)
            ]
        ];

    // Handle PresetTimeOfDay Dropdown Menu
    TSharedRef<IPropertyHandle> PropertyHandle_PresetTimeOfDay = DetailBuilder.
        GetProperty(GET_MEMBER_NAME_CHECKED(UAmbitObject, PresetTimeOfDay));
    ScenarioSettingsCategory.AddProperty(PropertyHandle_PresetTimeOfDay)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_PresetTimeOfDay->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(150.0f)
        .MaxDesiredWidth(150.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .FillWidth(1.1f)
            [
                SNew(SComboButton)
                .OnGetMenuContent_Static(&FTimeOfDayDropdownMenu::GetMenu,
                    PropertyHandle_PresetTimeOfDay)
                .ContentPadding(2)
                .ButtonContent()
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text_Static(&FTimeOfDayDropdownMenu::GetCurrentValueAsText,
                        PropertyHandle_PresetTimeOfDay)
                    .ToolTipText(PropertyHandle_PresetTimeOfDay->GetToolTipText())
                ]
            ]
        ];

    // Time of day slider
    TSharedRef<IPropertyHandle> PropertyHandle_TimeOfDay = DetailBuilder
        .GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, TimeOfDay));
    ScenarioSettingsCategory.AddProperty(PropertyHandle_TimeOfDay)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_TimeOfDay->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(150.0f)
        .MaxDesiredWidth(150.0f)
        [
            PropertyHandle_TimeOfDay->CreatePropertyValueWidget()
        ];

    // Add Weather Dropdown
    TSharedRef<IPropertyHandle> PropertyHandle_PresetWeather = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, PresetWeather));
    ScenarioSettingsCategory.AddProperty(PropertyHandle_PresetWeather)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_PresetWeather->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(150.0f)
        .MaxDesiredWidth(150.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .FillWidth(1.1f)
            [
                SNew(SComboButton)
                .OnGetMenuContent_Static(&FWeatherDropdownMenu::GetMenu,
                    PropertyHandle_PresetWeather)
                .ContentPadding(2)
                .ButtonContent()
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text_Static(&FWeatherDropdownMenu::GetCurrentValueAsText,
                        PropertyHandle_PresetWeather)
                    .ToolTipText(PropertyHandle_PresetWeather->GetToolTipText())
                ]
            ]
        ];

    // Format Weather Parameters
    TSharedRef<IPropertyHandle> PropertyHandle_WeatherParams = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, WeatherParameters));
    ScenarioSettingsCategory.AddProperty(PropertyHandle_WeatherParams);

    // Format Pedestrian Density
    TSharedRef<IPropertyHandle> PropertyHandle_PedestrianDensity = DetailBuilder
        .GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, PedestrianDensity));
    ScenarioSettingsCategory.AddProperty(PropertyHandle_PedestrianDensity)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_PedestrianDensity->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(150.0f)
        .MaxDesiredWidth(150.0f)
        [
            PropertyHandle_PedestrianDensity->CreatePropertyValueWidget()
        ];

    // Format Vehicle Density
    TSharedRef<IPropertyHandle> PropertyHandle_VehicleDensity = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, VehicleDensity));
    ScenarioSettingsCategory.AddProperty(PropertyHandle_VehicleDensity)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_VehicleDensity->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(150.0f)
        .MaxDesiredWidth(150.0f)
        [
            PropertyHandle_VehicleDensity->CreatePropertyValueWidget()
        ];

    // Format Export Button
    const FString KHintTextExportButton = "Export the Scenario Definition File";
    ScenarioSettingsCategory.AddCustomRow(FText::FromString("Export Scenario"))
        .ValueContent()
        .MinDesiredWidth(200.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(1, 0, 0, 0)
            [
                SNew(SButton)
                .ContentPadding(FMargin(4, 0))
                .Text(FText::FromString("Export Scenario"))
                .OnClicked_UObject(ConfigExporter, &UConfigImportExport::OnExportSdf)
                .ToolTipText(FText::FromString(KHintTextExportButton))
            ]
        ];
    
    // Build AWS Settings Menu
    IDetailCategoryBuilder& AWSSettingsCategory = DetailBuilder.
        EditCategory("AWS Settings");

    // AWS Region
    TSharedRef<IPropertyHandle> PropertyHandle_Region = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, AwsRegion));

    AWSSettingsCategory.AddProperty(PropertyHandle_Region)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_Region->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(150.0f)
        .MaxDesiredWidth(150.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .FillWidth(1.1f)
            [
                SNew(SComboButton)
                .OnGetMenuContent_Static(&FAWSRegionDropdownMenu::GetMenu,
                    PropertyHandle_Region)
                .ContentPadding(2)
                .ButtonContent()
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text_Static(&FAWSRegionDropdownMenu::GetCurrentValueAsText,
                        PropertyHandle_Region)
                    .ToolTipText(PropertyHandle_Region->GetToolTipText())
                ]
            ]
        ];

    // S3 Bucket Name
    TSharedRef<IPropertyHandle> PropertyHandle_BucketName = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, S3BucketName));
    AWSSettingsCategory.AddProperty(PropertyHandle_BucketName);

    // Build Permutations Settings Menu
    IDetailCategoryBuilder& PermutationsSettingsCategory = DetailBuilder.
        EditCategory("Permutation Settings");

    // Configuration Name
    const FString KHintTextConfigurationName =
        "Import Bulk Scenario Configuration File or Type Name for New Bulk Configuration";

    // Format Permutation name and import json file
    TSharedRef<IPropertyHandle> PropertyHandle_ConfigName = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, ConfigurationName));
    PermutationsSettingsCategory.AddProperty(PropertyHandle_ConfigName)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_ConfigName->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(200.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(SEditableTextBox)
                .Text_Static(&GetPropertyValueText, PropertyHandle_ConfigName)
                .OnTextCommitted_Static(&SetPropertyValueString,
                    PropertyHandle_ConfigName)
                .HintText(FText::FromString(KHintTextConfigurationName))
                .ToolTipText(FText::FromString(KHintTextConfigurationName))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(1, 0, 0, 0)
            [
                SNew(SButton)
                .ContentPadding(FMargin(4, 0))
                .Text(NSLOCTEXT("UnrealEd", "GenericOpenDialog", "..."))
                .OnClicked_UObject(ConfigExporter, &UConfigImportExport::OnReadFromS3Bucket)
            ]
         ];
    
    // Handle Batch name
    TSharedRef<IPropertyHandle> PropertyHandle_BatchName = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, BatchName));
    PermutationsSettingsCategory.AddProperty(PropertyHandle_BatchName);

    // Handle Time of Day types
    TSharedRef<IPropertyHandle> PropertyHandle_TimeOfDayTypes = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, TimeOfDayTypes));

    PermutationsSettingsCategory.AddProperty(PropertyHandle_TimeOfDayTypes)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_TimeOfDayTypes->CreatePropertyNameWidget()
        ]
        .ValueContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .Padding(KStandardPadding)
            [
                SNew(SWrapBox)
                .UseAllottedWidth(true)
                .InnerSlotPadding({ 6, 5 })
                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Morning)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Morning)
                        .ToolTipText(FText::FromString("Select Morning for Time of Day type"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Morning", "Morning"))
                        ]
                    ]
                ]

                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Noon)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Noon)
                        .ToolTipText(FText::FromString("Select Noon for Time of Day type"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Noon", "Noon"))
                        ]
                    ]
                ]

                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Evening)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Evening)
                        .ToolTipText(FText::FromString("Select Evening for Time of Day type"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Evening", "Evening"))
                        ]
                    ]
                ]

                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Night)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Night)
                        .ToolTipText(FText::FromString("Select Night for Time of Day type"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Night", "Night"))
                        ]
                    ]
                ]
            ]
        ];

    // Handle weather types
    TSharedRef<IPropertyHandle> PropertyHandle_WeatherTypes = DetailBuilder.
        GetProperty(GET_MEMBER_NAME_CHECKED(UAmbitObject, WeatherTypes));

    PermutationsSettingsCategory.AddProperty(PropertyHandle_WeatherTypes)
                .CustomWidget()
                .NameContent()
        [
            PropertyHandle_WeatherTypes->CreatePropertyNameWidget()
        ]
        .ValueContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .Padding(KStandardPadding)
            [
                SNew(SWrapBox)
                .UseAllottedWidth(true)
                .InnerSlotPadding({ 6, 5 })
                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Sunny)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Sunny)
                        .ToolTipText(FText::FromString("Select Sunny weather type"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Sunny", "Sunny"))
                        ]
                    ]
                ]

                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Rainy)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Rainy)
                        .ToolTipText(FText::FromString("Select Rainy weather type"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Rainy", "Rainy"))
                        ]
                    ]
                ]

                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Foggy)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Foggy)
                        .ToolTipText(FText::FromString("Select Foggy weather type"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Foggy", "Foggy"))
                        ]
                    ]
                ]
            ]
        ];

    // Handle Bulk Pedestrian Density
    TSharedRef<IPropertyHandle> PropertyHandle_BulkPedDensity = DetailBuilder.
    GetProperty(
        GET_MEMBER_NAME_CHECKED(UAmbitObject, BulkPedestrianTraffic));

    PermutationsSettingsCategory.AddProperty(PropertyHandle_BulkPedDensity)
                                .CustomWidget()
                                .NameContent()
        [
            PropertyHandle_BulkPedDensity->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(125.0f * 2.0f)
        .MaxDesiredWidth(125.0f * 2.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.2f)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("DensityMin", "DensityMin"))
                    .ToolTipText(LOCTEXT("BulkPedestrianMin", "The minimum percentage of pedestrian density in the range of [0.0, 1.0]"))
                ]
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .FillWidth(0.3f)
                .Padding(2, 0, 0, 2)
                [
                    SNew(SNumericEntryBox<float>)
                    .AllowSpin(true)
                    .MinValue(0.0f)
                    .MaxValue(1.0f)
                    .MaxSliderValue(1.0f)
                    .MinDesiredValueWidth(50.0f)
                    .Delta(0.1f)
                    .Value_Static(&GetBulkPedestrianDensityMin)
                    .OnValueChanged_Static(&SetBulkPedestrianDensityMin)
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.2f)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("DensityMax", "DensityMax"))
                    .ToolTipText(LOCTEXT("BulkPedestrianMax", "The maximum percentage of pedestrian density in the range of [0.0, 1.0]"))
                ]
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .FillWidth(0.3f)
                .Padding(2, 0, 0, 2)
                [
                    SNew(SNumericEntryBox<float>)
                    .AllowSpin(true)
                    .MinValue(0.0f)
                    .MaxValue(1.0f)
                    .MaxSliderValue(1.0f)
                    .MinDesiredValueWidth(50.0f)
                    .Delta(0.1f)
                    .Value_Static(&GetBulkPedestrianDensityMax)
                    .OnValueChanged_Static(&SetBulkPedestrianDensityMax)
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.2f)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("Increment", "Increment"))
                ]
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .FillWidth(0.3f)
                .Padding(2, 0, 0, 2)
                [
                    SNew(SEditableTextBox)
                    .IsReadOnly(true)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("0.1", "0.1"))
                ]
            ]
        ];

    // Handle Bulk Vehicle Density
    TSharedRef<IPropertyHandle> PropertyHandle_VehDensity = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, BulkVehicleTraffic));

    PermutationsSettingsCategory.AddProperty(PropertyHandle_VehDensity)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle_VehDensity->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(125.0f * 2.0f)
        .MaxDesiredWidth(125.0f * 2.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.2f)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("DensityMin", "DensityMin"))
                    .ToolTipText(LOCTEXT("BulkVehicleMin", "The minimum percentage of vehicle density in the range of [0.0, 1.0]"))
                ]
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .FillWidth(0.3f)
                .Padding(2, 0, 0, 2)
                [
                    SNew(SNumericEntryBox<float>)
                    .AllowSpin(true)
                    .MinValue(0.0f)
                    .MaxValue(1.0f)
                    .MaxSliderValue(1.0f)
                    .MinDesiredValueWidth(50.0f)
                    .Delta(0.1f)
                    .Value_Static(&GetBulkTrafficDensityMin)
                    .OnValueChanged_Static(&SetBulkTrafficDensityMin)
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.2f)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("DensityMax", "DensityMax"))
                    .ToolTipText(LOCTEXT("BulkVehicleMax", "The maximum percentage of vehicle density in the range of [0.0, 1.0]"))
                ]
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .FillWidth(0.3f)
                .Padding(2, 0, 0, 2)
                [
                    SNew(SNumericEntryBox<float>)
                    .AllowSpin(true)
                    .MinValue(0.0f)
                    .MaxValue(1.0f)
                    .MaxSliderValue(1.0f)
                    .MinDesiredValueWidth(50.0f)
                    .Delta(0.1f)
                    .Value_Static(&GetBulkTrafficDensityMax)
                    .OnValueChanged_Static(&SetBulkTrafficDensityMax)
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .FillWidth(0.2f)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("Increment", "Increment"))
                ]
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .FillWidth(0.3f)
                .Padding(2, 0, 0, 2)
                [
                    SNew(SEditableTextBox)
                    .IsReadOnly(true)
                    .Font(DetailBuilder.GetDetailFont())
                    .Text(LOCTEXT("0.1", "0.1"))
                ]
            ]
        ];

    // The number of permutation, read-only
    TSharedRef<IPropertyHandle> PropertyHandle_NumberOfPermutations =
        DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, NumberOfPermutations));
    PermutationsSettingsCategory.AddProperty(PropertyHandle_NumberOfPermutations);

    // Format Generate Button
    const FString KHintTextGeneratePermutations = 
        "Select Location to Export Bulk Scenario Configuration and Generate Permutations of Scenario Definition Files";
    PermutationsSettingsCategory.AddCustomRow(
                                    FText::FromString("Generate Permutations"))
                                .ValueContent()
                                .MinDesiredWidth(200.0f)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(1, 0, 0, 0)
        [
            SNew(SButton)
            .ContentPadding(FMargin(4, 0))
            .Text(FText::FromString("Generate Permutations"))
            .OnClicked_UObject(ConfigExporter, &UConfigImportExport::OnGeneratePermutations)
            .ToolTipText(FText::FromString(KHintTextGeneratePermutations))
        ]
    ];
    
    // Map Export Menu
    IDetailCategoryBuilder& MapExportSettingsCategory = DetailBuilder.
        EditCategory("Map Export Settings");

    // Handle export platforms
    TSharedRef<IPropertyHandle> PropertyHandle_ExportPlatforms = DetailBuilder.
        GetProperty(
            GET_MEMBER_NAME_CHECKED(UAmbitObject, ExportPlatforms));

    MapExportSettingsCategory.AddProperty(PropertyHandle_ExportPlatforms)
                .CustomWidget()
                .NameContent()
        [
            PropertyHandle_ExportPlatforms->CreatePropertyNameWidget()
        ]
        .ValueContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .Padding(KStandardPadding)
            [
                SNew(SWrapBox)
                .UseAllottedWidth(true)
                .InnerSlotPadding({ 6, 5 })
                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Linux)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Linux)
                        .ToolTipText(FText::FromString("Select standalone Linux export platform"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Linux", "Linux"))
                        ]
                    ]
                ]

                + SWrapBox::Slot()
                [
                    SNew(SBox)
                    .MinDesiredWidth(91)
                    [
                        SNew(SCheckBox)
                        .IsChecked_Static(&GetCheckState_Windows)
                        .OnCheckStateChanged_Static(&OnCheckStateChanged_Windows)
                        .ToolTipText(FText::FromString("Select standalone Windows export platform"))
                        [
                            SNew(STextBlock)
                            .Text(LOCTEXT("Windows", "Windows"))
                        ]
                    ]
                ]
            ]
        ];

    // Format Export Button
    const FString KHintTextExportMap = "Select target platform to export map";
    MapExportSettingsCategory.AddCustomRow(
        FText::FromString("Export Map"))
        .ValueContent()
        .MinDesiredWidth(200.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(1, 0, 0, 0)
            [
                SNew(SButton)
                .ContentPadding(FMargin(4, 0))
                .Text(FText::FromString("Export Map"))
                .OnClicked_UObject(ConfigExporter, &UConfigImportExport::OnExportMap)
                .ToolTipText(FText::FromString(KHintTextExportMap))
                .IsEnabled_Static(&GetExportButtonEnabled)
            ]
        ];

    // glTF File Type Selection
    TSharedRef<IPropertyHandle> PropertyHandle_GltfExportType = DetailBuilder.
        GetProperty(GET_MEMBER_NAME_CHECKED(UAmbitObject, GltfType));

    MapExportSettingsCategory.AddProperty(PropertyHandle_GltfExportType)
            .CustomWidget()
            .NameContent()
            [
                PropertyHandle_GltfExportType->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .MinDesiredWidth(150.0f)
            .MaxDesiredWidth(150.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
                .FillWidth(1.1f)
                [
                    SNew(SComboButton)
                    .OnGetMenuContent_Static(&FGltfFileTypeDropdownMenu::GetMenu,
                        PropertyHandle_GltfExportType)
                    .ContentPadding(2)
                    .ButtonContent()
                    [
                        SNew(STextBlock)
                        .Font(DetailBuilder.GetDetailFont())
                        .Text_Static(&FGltfFileTypeDropdownMenu::GetCurrentValueAsText,
                            PropertyHandle_GltfExportType)
                        .ToolTipText(PropertyHandle_GltfExportType->GetToolTipText())
                    ]
                ]
            ];

    // glTF Export Button
    const FString GltfExportButtonText = "Export glTF";
    MapExportSettingsCategory.AddCustomRow(
        FText::FromString(GltfExportButtonText))
        .ValueContent()
        .MinDesiredWidth(200.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(1, 0, 0, 0)
            [
                SNew(SButton)
                .ContentPadding(FMargin(4, 0))
                .Text(FText::FromString(GltfExportButtonText))
                .OnClicked_UObject(ConfigExporter, &UConfigImportExport::OnExportGltf)
                .IsEnabled_Static(&GetExportButtonEnabled)
            ]
        ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FText FAmbitDetailCustomization::GetPropertyValueText(
    TSharedRef<IPropertyHandle> PropertyHandle)
{
    FString Value;
    if (PropertyHandle->GetValueAsFormattedString(Value) ==
        FPropertyAccess::Success)
    {
        return FText::FromString(Value);
    }

    return FText();
}

void FAmbitDetailCustomization::SetPropertyValueString(
    const FText& NewValue, ETextCommit::Type CommitInfo,
    TSharedRef<IPropertyHandle> PropertyHandle)
{
    const FString Name = NewValue.ToString();
    ensure(PropertyHandle->SetValue(Name) == FPropertyAccess::Success);
}

void FAmbitDetailCustomization::SetBulkPedestrianDensityMin(float NewMin)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->BulkPedestrianTraffic.Min = NewMin;

    UpdateNumberOfPermutations();
}

TOptional<float> FAmbitDetailCustomization::GetBulkPedestrianDensityMin()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return TOptional<float>(AmbitMode->UISettings->BulkPedestrianTraffic.Min);
}

void FAmbitDetailCustomization::SetBulkPedestrianDensityMax(float NewMax)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->BulkPedestrianTraffic.Max = NewMax;

    UpdateNumberOfPermutations();
}

TOptional<float> FAmbitDetailCustomization::GetBulkPedestrianDensityMax()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return TOptional<float>(AmbitMode->UISettings->BulkPedestrianTraffic.Max);
}

void FAmbitDetailCustomization::SetBulkTrafficDensityMin(float NewMin)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->BulkVehicleTraffic.Min = NewMin;

    UpdateNumberOfPermutations();
}

TOptional<float> FAmbitDetailCustomization::GetBulkTrafficDensityMin()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return TOptional<float>(AmbitMode->UISettings->BulkVehicleTraffic.Min);
}

void FAmbitDetailCustomization::SetBulkTrafficDensityMax(float NewMax)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->BulkVehicleTraffic.Max = NewMax;

    UpdateNumberOfPermutations();
}

TOptional<float> FAmbitDetailCustomization::GetBulkTrafficDensityMax()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return TOptional<float>(AmbitMode->UISettings->BulkVehicleTraffic.Max);
}

FText FAmbitDetailCustomization::UpdateNumberOfPermutations()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    int32 PedestrianCounts = AmbitMode->UISettings->BulkPedestrianTraffic.GetVariantCount();
    int32 VehicleCounts = AmbitMode->UISettings->BulkVehicleTraffic.GetVariantCount();
    int32 LengthOfWeatherTypes = AmbitMode->UISettings->WeatherTypes.GetVariantCount();
    int32 NumOfWeatherTypes = FMath::Max(LengthOfWeatherTypes, 1);
    int32 LengthOfTimeOfDayTypes = AmbitMode->UISettings->TimeOfDayTypes.GetVariantCount();
    int32 NumOfTimeOfDayTypes = FMath::Max(LengthOfTimeOfDayTypes, 1);

    int32 NumberOfPermutation = PedestrianCounts * VehicleCounts * NumOfWeatherTypes * NumOfTimeOfDayTypes;
    AmbitMode->UISettings->NumberOfPermutations = NumberOfPermutation;
    return FText::AsNumber(NumberOfPermutation);
}

/** Time of Day Types check box helper functions*/
void FAmbitDetailCustomization::OnCheckStateChanged_Morning(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->TimeOfDayTypes.SetMorning(State == ECheckBoxState::Checked);
    UpdateNumberOfPermutations();
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Morning()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->TimeOfDayTypes.GetMorning() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FAmbitDetailCustomization::OnCheckStateChanged_Noon(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->TimeOfDayTypes.SetNoon(State == ECheckBoxState::Checked);
    UpdateNumberOfPermutations();
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Noon()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->TimeOfDayTypes.GetNoon() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FAmbitDetailCustomization::OnCheckStateChanged_Evening(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->TimeOfDayTypes.SetEvening(State == ECheckBoxState::Checked);
    UpdateNumberOfPermutations();
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Evening()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->TimeOfDayTypes.GetEvening() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FAmbitDetailCustomization::OnCheckStateChanged_Night(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->TimeOfDayTypes.SetNight(State == ECheckBoxState::Checked);
    UpdateNumberOfPermutations();
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Night()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->TimeOfDayTypes.GetNight() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

/** Weather Types check box helper functions*/
void FAmbitDetailCustomization::OnCheckStateChanged_Sunny(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->WeatherTypes.SetSunny(State == ECheckBoxState::Checked);
    UpdateNumberOfPermutations();
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Sunny()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->WeatherTypes.GetSunny() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FAmbitDetailCustomization::OnCheckStateChanged_Rainy(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->WeatherTypes.SetRainy(State == ECheckBoxState::Checked);
    UpdateNumberOfPermutations();
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Rainy()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->WeatherTypes.GetRainy() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FAmbitDetailCustomization::OnCheckStateChanged_Foggy(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->WeatherTypes.SetFoggy(State == ECheckBoxState::Checked);
    UpdateNumberOfPermutations();
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Foggy()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->WeatherTypes.GetFoggy() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FAmbitDetailCustomization::OnCheckStateChanged_Linux(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->ExportPlatforms.SetLinux(State == ECheckBoxState::Checked);
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Linux()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->ExportPlatforms.GetLinux() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FAmbitDetailCustomization::OnCheckStateChanged_Windows(ECheckBoxState State)
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    AmbitMode->UISettings->ExportPlatforms.SetWindows(State == ECheckBoxState::Checked);
}

ECheckBoxState FAmbitDetailCustomization::GetCheckState_Windows()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->ExportPlatforms.GetWindows() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool FAmbitDetailCustomization::GetExportButtonEnabled()
{
    FAmbitMode* AmbitMode = FAmbitMode::GetEditorMode();
    check(AmbitMode);

    return AmbitMode->UISettings->ExportPlatforms.GetWindows() || AmbitMode->UISettings->ExportPlatforms.GetLinux();
}

#undef LOCTEXT_NAMESPACE
