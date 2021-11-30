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

#include "AmbitMode.h"
#include "AssetThumbnail.h"
#include "CoreMinimal.h"
#include "Editor.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/BaseToolkit.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class IDetailsView;
class SAmbitWidget;
class SErrorText;
struct FPropertyAndParent;

class FAmbitModeToolkit : public FModeToolkit
{
public:
    /** FModeToolkit interface */
    void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;
    /** End FModeToolkit interface */

    /** IToolkit interface */
    FName GetToolkitFName() const override;
    FText GetBaseToolkitName() const override;
    FAmbitMode* GetEditorMode() const override;
    TSharedPtr<class SWidget> GetInlineContent() const override;
    /** End IToolkit interface */

private:
    /** Geometry tools widget */
    TSharedPtr<SAmbitWidget> Widget;
};

/**
* Slate widgets for the Ambit Mode
*/
class SAmbitWidget : public SCompoundWidget
{
public:
SLATE_BEGIN_ARGS(SAmbitWidget)
        {
        }

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, TSharedRef<FAmbitModeToolkit> InParentToolkit);

    void RefreshDetailPanel();

protected:

    class FAmbitMode* GetEditorMode() const;

    FText GetErrorText() const;

    bool GetAmbitIsEnabled() const;

    bool GetIsPropertyVisible(const FPropertyAndParent& PropertyAndParent) const;

    TSharedPtr<SErrorText> Error;

    TSharedPtr<IDetailsView> DetailsPanel;
};
