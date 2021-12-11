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

#include "AmbitWidget.h"

#include "EditorModeManager.h"
#include "EditorTutorial.h"
#include "IDetailsView.h"
#include "IIntroTutorials.h"
#include "PropertyEditorModule.h"
#include "SlateOptMacros.h"

#include "AmbitDetailCustomization.h"
#include "AmbitMode.h"
#include "AmbitObject.h"

#define LOCTEXT_NAMESPACE "FAmbitModule"

void FAmbitModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
    FAmbitMode* AmbitMode = GetEditorMode();

    Widget = SNew(SAmbitWidget, SharedThis(this));

    FModeToolkit::Init(InitToolkitHost);
}

FName FAmbitModeToolkit::GetToolkitFName() const
{
    return FName("AmbitGenerator");
}

FText FAmbitModeToolkit::GetBaseToolkitName() const
{
    return LOCTEXT("Toolkitname", "Ambit Scenario Generator");
}

FAmbitMode* FAmbitModeToolkit::GetEditorMode() const
{
    return static_cast<FAmbitMode*>(GLevelEditorModeTools().GetActiveMode(FAmbitMode::EM_AmbitModeId));
}

TSharedPtr<SWidget> FAmbitModeToolkit::GetInlineContent() const
{
    return Widget;
}

//////////////////////////////////////////////////////////////////////////

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAmbitWidget::Construct(const FArguments& InArgs, TSharedRef<FAmbitModeToolkit> InParentToolkit)
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(
        "PropertyEditor");
    PropertyEditorModule.RegisterCustomClassLayout("AmbitObject",
                                                   FOnGetDetailCustomizationInstance::CreateStatic(
                                                       &FAmbitDetailCustomization::MakeInstance));
    PropertyEditorModule.NotifyCustomizationModuleChanged();

    const FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea);

    DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsPanel->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateSP(this, &SAmbitWidget::GetIsPropertyVisible));

    FAmbitMode* AmbitMode = GetEditorMode();

    const IIntroTutorials& IntroTutorials = FModuleManager::LoadModuleChecked<IIntroTutorials>(TEXT("IntroTutorials"));

    const FMargin StandardLeftPadding(6.f, 3.f, 3.f, 3.f);
    const FMargin StandardRightPadding(3.f, 3.f, 6.f, 3.f);

    // @formatter:off
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .Padding(StandardLeftPadding)
            .HAlign(HAlign_Left)

            + SHorizontalBox::Slot()
            .Padding(StandardRightPadding)
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
                // Tutorial link
                IntroTutorials.CreateTutorialsWidget(TEXT("AmbitMode"))
            ]
        ]
        + SVerticalBox::Slot()
        .Padding(0)
        [
            SNew(SVerticalBox)
            .IsEnabled(this, &SAmbitWidget::GetAmbitIsEnabled)
            + SVerticalBox::Slot()
            .Padding(0)
            [
                DetailsPanel.ToSharedRef()

            ]
        ]
    ];

    // @formatter:on

    RefreshDetailPanel();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FAmbitMode* SAmbitWidget::GetEditorMode() const
{
    return static_cast<FAmbitMode*>(GLevelEditorModeTools().GetActiveMode(FAmbitMode::EM_AmbitModeId));
}

FText SAmbitWidget::GetErrorText() const
{
    FAmbitMode* AmbitMode = GetEditorMode();

    return FText::GetEmpty();
}

bool SAmbitWidget::GetAmbitIsEnabled() const
{
    FAmbitMode* AmbitMode = GetEditorMode();
    return AmbitMode != nullptr;
}

bool SAmbitWidget::GetIsPropertyVisible(const FPropertyAndParent& PropertyAndParent) const
{
    return true;
}

void SAmbitWidget::RefreshDetailPanel() const
{
    FAmbitMode* AmbitMode = GetEditorMode();
    if (AmbitMode != nullptr)
    {
        // Refresh details panel
        DetailsPanel->SetObject(AmbitMode->UISettings, true);
    }
}
#undef LOCTEXT_NAMESPACE
