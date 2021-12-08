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

#include "AmbitMode.h"

#include "EditorModeManager.h"
#include "EditorViewportClient.h"
#include "Toolkits/ToolkitManager.h"

#include "AmbitObject.h"
#include "AmbitWeather.h"
#include "AmbitWidget.h"
#include "Ambit/Utils/UserMetricsSubsystem.h"

const FEditorModeID FAmbitMode::EM_AmbitModeId = TEXT("EM_AmbitMode");

FAmbitMode* FAmbitMode::GetEditorMode()
{
    return static_cast<FAmbitMode*>(GLevelEditorModeTools().GetActiveMode(EM_AmbitModeId));
}

FAmbitMode::FAmbitMode()
{
    UISettings = NewObject<UAmbitObject>(GetTransientPackage(), TEXT("AmbitSettings"), RF_Transactional);
    UISettings->SetParent(this);
}

FAmbitMode::~FAmbitMode() = default;

void FAmbitMode::AddReferencedObjects(FReferenceCollector& Collector)
{
    // Call parent implementation
    FEdMode::AddReferencedObjects(Collector);

    Collector.AddReferencedObject(UISettings);
}

void FAmbitMode::Enter()
{
    FEdMode::Enter();

    if (!Toolkit.IsValid() && UsesToolkits())
    {
        // this is what instantiates the UI for Ambit's custom mode
        Toolkit = MakeShareable(new FAmbitModeToolkit);
        Toolkit->Init(Owner->GetToolkitHost());
    }

    GEngine->GetEngineSubsystem<UUserMetricsSubsystem>()->Track(UserMetrics::AmbitMode::KAmbitModeOpenEvent,
                                                                UserMetrics::AmbitMode::KAmbitModeNameSpace);
}

void FAmbitMode::Exit()
{
    if (Toolkit.IsValid())
    {
        FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
        Toolkit.Reset();
    }

    // Call base Exit method to ensure proper cleanup
    FEdMode::Exit();

    GEngine->GetEngineSubsystem<UUserMetricsSubsystem>()->Track(UserMetrics::AmbitMode::KAmbitModeCloseEvent,
                                                                UserMetrics::AmbitMode::KAmbitModeNameSpace);
}

void FAmbitMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
    FEdMode::Tick(ViewportClient, DeltaTime);

    for (TObjectIterator<AAmbitWeather> It; It; ++It)
    {
        AAmbitWeather* Weather = Cast<AAmbitWeather>(*It);
        if (Weather != nullptr)
        {
            Weather->ApplyTimeOfDay(UISettings->TimeOfDay);
            Weather->ApplyWeather(UISettings->WeatherParameters);
            if (UISettings->TimeOfDay != TimeOfDay::KDefaultMorningSolarTime &&
                UISettings->TimeOfDay != TimeOfDay::KDefaultMidDaySolarTime &&
                UISettings->TimeOfDay != TimeOfDay::KDefaultEveningSolarTime &&
                UISettings->TimeOfDay != TimeOfDay::KDefaultNightSolarTime)
            {
                UISettings->PresetTimeOfDay = TimeOfDay::KCustom;
            }
            UISettings->PresetWeather = AmbitWeatherParameters::GetWeatherType(UISettings->WeatherParameters);
        }
    }
}

void FAmbitMode::Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI)
{
    FEdMode::Render(View, Viewport, PDI);
}

bool FAmbitMode::UsesToolkits() const
{
    return true;
}
