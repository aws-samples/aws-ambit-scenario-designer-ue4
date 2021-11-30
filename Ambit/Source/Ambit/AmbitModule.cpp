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

#include "AmbitModule.h"
#include "Mode/AmbitMode.h"
#include "Mode/AmbitWidget.h"

#include "Actors/Spawners/SpawnOnSurface.h"
#include "Actors/Spawners/SpawnOnPath.h"
#include "Actors/Spawners/SpawnInVolume.h"
#include "Actors/Spawners/SpawnVehiclePath.h"
#include "Actors/Spawners/SpawnWithHoudini.h"
#include "Actors/EditorDetails/SpawnerDetails.h"

#include "LevelEditor.h"

#include "Styling/SlateStyleRegistry.h"
#include "SlateOptMacros.h"
#include "PlacementMode/Public/IPlacementModeModule.h"

#include <AmbitUtils/MenuHelpers.h>

#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(LogAmbit)

// anon namespace for utility methods
namespace
{
    FString GetAmbitPluginDir()
    {
        FString EnginePluginDir = FPaths::EnginePluginsDir() / TEXT("Editor/Ambit");
        if (FPaths::DirectoryExists(EnginePluginDir))
        {
            return EnginePluginDir;
        }

        FString ProjectPluginDir = FPaths::ProjectPluginsDir() / TEXT("Editor/Ambit");
        if (FPaths::DirectoryExists(ProjectPluginDir))
        {
            return ProjectPluginDir;
        }

        TSharedPtr<IPlugin> AmbitPlugin = IPluginManager::Get().FindPlugin(TEXT("Ambit"));
        if (AmbitPlugin.IsValid() && FPaths::DirectoryExists(AmbitPlugin->GetBaseDir()))
        {
            return AmbitPlugin->GetBaseDir();
        }

        UE_LOG(LogAmbit, Warning, TEXT("Could not find the Ambit plugin's directory"));
        return "";
    }
}

#define LOCTEXT_NAMESPACE "FAmbitModule"

TSharedPtr<FSlateStyleSet> FAmbitModule::StyleSet = nullptr;
TSharedPtr<class ISlateStyle> FAmbitModule::GetStyleSet() { return StyleSet; }
FAmbitModule* FAmbitModule::FAmbitModuleInstance = nullptr;

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FAmbitModule::Initialize()
{
    // Const icon sizes
    const FVector2D Icon16x16(16.0f, 16.0f);
    const FVector2D Icon20x20(20.0f, 20.0f);
    const FVector2D Icon32x32(32.0f, 32.0f);
    const FVector2D Icon40x40(40.0f, 40.0f);
    const FVector2D Icon64x64(64.0f, 64.0f);

    // Only register once
    if (StyleSet.IsValid())
    {
        return;
    }

    StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
    StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
    StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

    static FString IconsDir = GetAmbitPluginDir() / TEXT("Content/Slate/Icons/");

    // Mode Icon Settings
    StyleSet->Set("LevelEditor.Ambit", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon40x40));
    StyleSet->Set("LevelEditor.Ambit.Small", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon20x20));
    StyleSet->Set("LevelEditor.Ambit.Selected", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon40x40));
    StyleSet->Set("LevelEditor.Ambit.Selected.Small", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon20x20));

    // Actor Icon Settings
    StyleSet->Set("ClassIcon.SpawnOnSurface", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon16x16));
    StyleSet->Set("ClassThumbnail.SpawnOnSurface", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon64x64));
    // TODO: Get different icons for Spline, Bounding and Path Generator
    StyleSet->Set("ClassIcon.SpawnInVolume", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon16x16));
    StyleSet->Set("ClassThumbnail.SpawnInVolume", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon64x64));
    StyleSet->Set("ClassIcon.SpawnOnPath", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon16x16));
    StyleSet->Set("ClassThumbnail.SpawnOnPath", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon64x64));
    StyleSet->Set("ClassIcon.SpawnVehiclePath", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon16x16));
    StyleSet->Set("ClassThumbnail.SpawnVehiclePath", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon64x64));
    StyleSet->Set("ClassIcon.SpawnWithHoudini", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon16x16));
    StyleSet->Set("ClassThumbnail.SpawnWithHoudini", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon64x64));

    // Notification Icon Settings
    StyleSet->Set("Notification.Ambit", new FSlateImageBrush(IconsDir + "Ambit_Icon_1024.png", Icon32x32));

    FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FName FAmbitModule::GetStyleSetName()
{
    static FName StyleName(TEXT("Ambit Mode"));
    return StyleName;
}

FAmbitModule& FAmbitModule::Get()
{
    check(FAmbitModule::FAmbitModuleInstance);
    return *FAmbitModuleInstance;
}

void FAmbitModule::CreateAmbitNotification(FText MessageText, float FadeInDuration,
                                           float FadeOutDuration, float ExpireDuration, bool bFireAndForget)
{
    FMenuHelpers::CreateNotification(MessageText, Get().GetAmbitNotificationBrush(),
                                     FadeInDuration, FadeOutDuration, ExpireDuration, bFireAndForget);
}

void FAmbitModule::StartupModule()
{
    FAmbitModuleInstance = this;
    //Initialize StyleSet
    Initialize();

    AmbitNotificationBrush = FSlateIcon(GetStyleSetName(), "Notification.Ambit").GetIcon();

    //register mode
    FEditorModeRegistry::Get().RegisterMode<FAmbitMode>(
        FAmbitMode::EM_AmbitModeId,
        LOCTEXT("AmbitMode", "AWS Ambit Scenario Mode"),
        FSlateIcon(GetStyleSetName(), "LevelEditor.Ambit", "LevelEditor.Ambit.Small"),
        true
    );

    //register place actors category
    IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
    //Register Ambit Spawner Types
    int32 Priority = 43; // Position of category tab in Place Actors panel
    FPlacementCategoryInfo AwsAmbit(LOCTEXT("AwsAmbit",
                                                     "AWS Ambit"), "AwsAmbit",
                                             TEXT("AwsAmbit"), Priority);
    PlacementModeModule.RegisterPlacementCategory(AwsAmbit);

    //register actors to category
    RegisterPlaceableItem<ASpawnOnSurface>(AwsAmbit.UniqueHandle);
    RegisterPlaceableItem<ASpawnInVolume>(AwsAmbit.UniqueHandle);
    RegisterPlaceableItem<ASpawnOnPath>(AwsAmbit.UniqueHandle);
    RegisterPlaceableItem<ASpawnVehiclePath>(AwsAmbit.UniqueHandle);
    RegisterPlaceableItem<ASpawnWithHoudini>(AwsAmbit.UniqueHandle);

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    // Register details presenter for our component type and runtime settings.
    PropertyModule.RegisterCustomClassLayout(ASpawnWithHoudini::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FSpawnerDetails::MakeInstance));

    PropertyModule.NotifyCustomizationModuleChanged();
}

template <typename ClassType>
void FAmbitModule::RegisterPlaceableItem(const FName& UniqueHandle)
{
    IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
    PlacementModeModule.RegisterPlaceableItem(UniqueHandle,
                                              MakeShareable(
                                                  new FPlaceableItem(nullptr, FAssetData(ClassType::StaticClass()))));
}

void FAmbitModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module. For modules that support dynamic reloading,
    // we call this function before unloading the module.
    FEditorModeRegistry::Get().UnregisterMode(FAmbitMode::EM_AmbitModeId);
    FAmbitModuleInstance = nullptr;

    if (IPlacementModeModule::IsAvailable())
    {
        IPlacementModeModule::Get().UnregisterPlacementCategory("AwsAmbit");
    }

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomPropertyTypeLayout(ASpawnWithHoudini::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAmbitModule, Ambit)
