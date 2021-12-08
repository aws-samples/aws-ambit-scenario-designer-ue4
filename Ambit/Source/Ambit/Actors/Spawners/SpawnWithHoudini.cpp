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

#include "SpawnWithHoudini.h"

#include "HoudiniPublicAPI.h"
#include "HoudiniPublicAPIBlueprintLib.h"
#include "Components/BillboardComponent.h"
#include "UObject/ConstructorHelpers.h"

#include <stdexcept>

#include "Ambit/AmbitModule.h"
#include "Ambit/Actors/SpawnedObjectConfigs/SpawnedObjectConfig.h"
#include "Ambit/Mode/Constant.h"
#include "Ambit/Utils/AmbitWorldHelpers.h"

#include <AmbitUtils/MenuHelpers.h>

ASpawnWithHoudini::ASpawnWithHoudini()
{
    IconComponent = CreateDefaultSubobject<UBillboardComponent>("Icon");
    const auto& IconAsset = ConstructorHelpers::FObjectFinder<UTexture2D>(AmbitIcon::KPath);
    IconComponent->Sprite = IconAsset.Object;
    IconComponent->SetRelativeScale3D(FVector(0.5f));
    IconComponent->SetupAttachment(RootComponent);

    SetRootComponent(IconComponent);

    FHoudiniLoadableAsset StarterAsset;
    StarterAsset.HDAToLoad = nullptr;
    HoudiniAssetDetails.Add(StarterAsset);

    HoudiniApi = UHoudiniPublicAPIBlueprintLib::GetAPI();
}

ASpawnWithHoudini::~ASpawnWithHoudini()
{
    UWorld* World = GetWorld();

    // If we generated from the editor, we don't destroy when leaving play.
    // If we generated from the runtime, we let the EndPlay function handle
    // this clearing as to not disrupt other lifecycles.
    if (CurrentGeneration == NotGenerated
        || CurrentGeneration == FromEditor
        && (World != nullptr && (World->IsPlayInEditor() || World->IsPlayInPreview())))
    {
        ResetObstacleSpawner();
    }
}

bool ASpawnWithHoudini::HasActorsToSpawn() const
{
    const TArray<FHoudiniLoadableAsset> FilteredActorsToSpawn =
            HoudiniAssetDetails.FilterByPredicate([](const FHoudiniLoadableAsset& AssetDetails)
            {
                return AssetDetails.HDAToLoad != nullptr;
            });
    return FilteredActorsToSpawn.Num() > 0;
}

void ASpawnWithHoudini::GenerateObstacles()
{
    Random.Initialize(RandomSeed);
    const bool bDidClear = ClearObstacles();
    if (!bDidClear)
    {
        return;
    }

    const TArray<AActor*> SurfaceActors = AmbitWorldHelpers::GetActorsByMatchBy(
        MatchBy, SurfaceNamePattern, SurfaceTags);

    UE_LOG(LogAmbit, Display, TEXT("Matching surface actors: %i"),
           SurfaceActors.Num());

    TArray<FTransform> LocationsToSpawn =
            AmbitWorldHelpers::GenerateRandomLocationsFromActors(
                SurfaceActors, RandomSeed, DensityMin, DensityMax);

    UWorld* World = GetWorld();

    if (CurrentGeneration == NotGenerated)
    {
        if (World != nullptr && (World->IsPlayInEditor() || World->IsPlayInPreview()))
        {
            CurrentGeneration = FromRuntime;
        }
        else
        {
            CurrentGeneration = FromEditor;
        }
    }

    HoudiniApi->CreateSession();
    for (const FTransform& Transform : LocationsToSpawn)
    {
        const int32 Index = Random.RandRange(0, HoudiniAssetDetails.Num() - 1);
        UHoudiniAsset* IndividualHDA = HoudiniAssetDetails[Index].HDAToLoad;
        // Check whether the HDA Asset has been selected
        if (IndividualHDA != nullptr)
        {
            FString BakePath = GetBakePathRelative();
            UHoudiniPublicAPIAssetWrapper* SpawnedActor = HoudiniApi->InstantiateAsset(
                IndividualHDA, Transform, World, nullptr
                , false, false, BakePath);
            if (SpawnedActor != nullptr)
            {
                SpawnedActor->GetOnPreInstantiationDelegate().AddUniqueDynamic(
                    this, &ASpawnWithHoudini::AssetPreInstantiation_DelegateHandler);
                SpawnedActor->GetOnPostProcessingDelegate().AddUniqueDynamic(
                    this, &ASpawnWithHoudini::PostProcessing_DelegateHandler);
                SpawnedActor->GetOnPostBakeDelegate().AddUniqueDynamic(
                    this, &ASpawnWithHoudini::PostBake_DelegateHandler);

                HoudiniAssetDetails[Index].SpawnedActors.Add(SpawnedActor);
            }
        }
    }
}

bool ASpawnWithHoudini::ClearObstacles()
{
    UWorld* World = GetWorld();

    const bool bIsRuntime = World != nullptr && (World->IsPlayInEditor() || World->IsPlayInPreview());

    if (CurrentGeneration == FromEditor && bIsRuntime)
    {
        FMenuHelpers::DisplayMessagePopup("Content generated in the editor cannot be modified in runtime.", "Error");
        return false;
    }

    for (auto& LoadedSet : HoudiniAssetDetails)
    {
        TArray<UHoudiniPublicAPIAssetWrapper*> SpawnedActors = LoadedSet.SpawnedActors;

        for (auto* Actor : SpawnedActors)
        {
            if (Actor->IsValidLowLevel() && !Actor->IsPendingKill())
            {
                Actor->DeleteInstantiatedAsset();
            }
        }

        LoadedSet.SpawnedActors.Empty();
    }

    CurrentGeneration = NotGenerated;
    ActorBakeCount = 0;
    CachedActorCount = 0;
    return true;
}

void ASpawnWithHoudini::PopulateParameters(UHoudiniPublicAPIAssetWrapper* SpawnedActor)
{
    if (SpawnedActor->IsValidLowLevel())
    {
        const int32 FoundAsset = HoudiniAssetDetails.IndexOfByPredicate([SpawnedActor](const FHoudiniLoadableAsset&
        asset)
            {
                return asset.SpawnedActors.Contains(SpawnedActor);
            });

        if (FoundAsset != INDEX_NONE && HoudiniAssetDetails[FoundAsset].ParamsToRandom.Num() == 0)
        {
            TMap<FName, FHoudiniParameterTuple> ActorExistingParameterMap;

            const bool bActiveParameters = SpawnedActor->GetParameterTuples(ActorExistingParameterMap);
            if (bActiveParameters)
            {
                for (const auto& ParameterMap : ActorExistingParameterMap)
                {
                    FName ParameterName = ParameterMap.Key;
                    if (ParameterName.GetPlainNameString().Contains("seed"))
                    {
                        HoudiniAssetDetails[FoundAsset].ParamsToRandom.Add(ParameterName);
                    }
                }
            }
        }
    }
}

void ASpawnWithHoudini::RandomizeActor(UHoudiniPublicAPIAssetWrapper* SpawnedActor)
{
    if (SpawnedActor->IsValidLowLevel())
    {
        int32 AssetIndexInList = INDEX_NONE;
        const int32 ListNumber = HoudiniAssetDetails.IndexOfByPredicate([SpawnedActor, &AssetIndexInList](
        const FHoudiniLoadableAsset& asset)
            {
                AssetIndexInList = asset.SpawnedActors.IndexOfByKey(SpawnedActor);
                return AssetIndexInList != INDEX_NONE;
            });

        TMap<FName, FHoudiniParameterTuple> ActorExistingParameterMap;
        TMap<FName, FHoudiniParameterTuple> ActorNewParameterMap;

        // Since this task is parallelized, we need to ensure that this seed is the same per actor,
        // but different enough from all other actors.
        if (ListNumber != INDEX_NONE)
        {
            int32 AssetSeed = 100 * RandomSeed + 10 * ListNumber + AssetIndexInList;
            Random.Initialize(AssetSeed);
        }

        const bool bActiveParameters = SpawnedActor->GetParameterTuples(ActorExistingParameterMap);
        if (bActiveParameters)
        {
            for (const auto& ParameterTuple : ActorExistingParameterMap)
            {
                FHoudiniParameterTuple NewParameterValue;

                const FName ParameterName = ParameterTuple.Key;
                FHoudiniParameterTuple CurrentTuples = ParameterTuple.Value;

                // If the ParamsToRandom does not have the parameter, we skip it and take whatever it has.
                if (ListNumber != INDEX_NONE && HoudiniAssetDetails[ListNumber].ParamsToRandom.Contains(ParameterName))
                {
                    for (int i = 0; i < CurrentTuples.BoolValues.Num(); i++)
                    {
                        bool bRandomBool = Random.RandRange(0, 1) == 1;
                        NewParameterValue.BoolValues.Add(bRandomBool);
                    }

                    for (int i = 0; i < CurrentTuples.FloatValues.Num(); i++)
                    {
                        float RandomFloat = Random.FRandRange(0, FloatMax);
                        NewParameterValue.FloatValues.Add(RandomFloat);
                    }

                    for (int i = 0; i < CurrentTuples.Int32Values.Num(); i++)
                    {
                        int RandomInt = Random.RandRange(0, IntMax);
                        NewParameterValue.Int32Values.Add(RandomInt);
                    }

                    // Currently, we shouldn't randomize strings. Keep them the same values to keep param list happy.
                    for (int i = 0; i < CurrentTuples.StringValues.Num(); i++)
                    {
                        FString PreviousString = CurrentTuples.StringValues[i];
                        NewParameterValue.StringValues.Add(PreviousString);
                    }

                    for (int i = 0; i < CurrentTuples.FloatRampPoints.Num(); i++)
                    {
                        // For the sake of simplicity, this keeps the numbers sane.
                        const float InPosition = CurrentTuples.FloatRampPoints[i].Position;
                        const float InValue = Random.FRandRange(0, FloatMax);
                        const EHoudiniPublicAPIRampInterpolationType InterpolationType = static_cast<
                            EHoudiniPublicAPIRampInterpolationType>(Random.RandRange(1, 7));

                        FHoudiniPublicAPIFloatRampPoint RandomFloatRamp = FHoudiniPublicAPIFloatRampPoint(
                            InPosition, InValue, InterpolationType);
                        NewParameterValue.FloatRampPoints.Add(RandomFloatRamp);
                    }

                    for (int i = 0; i < CurrentTuples.ColorRampPoints.Num(); i++)
                    {
                        // Seeded variant of FLinearColor::MakeRandomColor
                        const uint8 Hue = static_cast<uint8>(Random.RandRange(0, 255));
                        const uint8 Saturation = static_cast<uint8>(Random.RandRange(0, 255));
                        const uint8 Brightness = static_cast<uint8>(Random.RandRange(0, 255));
                        const FLinearColor Color = FLinearColor::MakeFromHSV8(Hue, Saturation, Brightness);

                        // For the sake of simplicity, this keeps the numbers sane.
                        const float InPosition = CurrentTuples.ColorRampPoints[i].Position;
                        const EHoudiniPublicAPIRampInterpolationType InterpolationType = static_cast<
                            EHoudiniPublicAPIRampInterpolationType>(Random.RandRange(1, 7));

                        FHoudiniPublicAPIColorRampPoint RandomColorRamp = FHoudiniPublicAPIColorRampPoint(
                            InPosition, Color, InterpolationType);
                        NewParameterValue.ColorRampPoints.Add(RandomColorRamp);
                    }

                    ActorNewParameterMap.Add(ParameterName, NewParameterValue);
                }
                else
                {
                    ActorNewParameterMap.Add(ParameterName, CurrentTuples);
                }
            }

            const bool bResolved = SpawnedActor->SetParameterTuples(ActorNewParameterMap);
            if (bResolved)
            {
                SpawnedActor->Recook();
                ActorExistingParameterMap.Empty();
            }
            else
            {
                UE_LOG(LogAmbit, Warning,
                       TEXT("Houdini Asset could not be fully cooked for spawner %s and object %s. Invalid option set."
                       ),
                       *this->GetActorLabel(), *SpawnedActor->GetName());
            }
        }
    }
    else
    {
        UE_LOG(LogAmbit, Warning, TEXT("Houdini Asset is no longer available in %s. Skipping."),
               *this->GetActorLabel());
    }
}

TArray<UHoudiniPublicAPIAssetWrapper*> ASpawnWithHoudini::GetSpawnedActors() const
{
    TArray<UHoudiniPublicAPIAssetWrapper*> SpawnedActors;
    for (const auto& LoadedSet : HoudiniAssetDetails)
    {
        TArray<UHoudiniPublicAPIAssetWrapper*> AssetActors = LoadedSet.SpawnedActors;

        SpawnedActors.Append(AssetActors);
    }

    return SpawnedActors;
}

void ASpawnWithHoudini::ResetObstacleSpawner()
{
    ClearObstacles();
    HoudiniAssetDetails.Empty();
    FHoudiniLoadableAsset StarterAsset;
    StarterAsset.HDAToLoad = nullptr;
    HoudiniAssetDetails.Add(StarterAsset);
    SurfaceNamePattern = "";
    SurfaceTags.Empty();
    MatchBy = NameAndTags;
    RandomSeed = 0;
}

void ASpawnWithHoudini::GenerateSpawnedObjectConfiguration()
{
    GenerateSpawnedObjectConfiguration(RandomSeed);
}

void ASpawnWithHoudini::GenerateSpawnedObjectConfiguration(int32 Seed)
{
    USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();

    const int32 OriginalSeed = RandomSeed;
    RandomSeed = Seed;

    GenerateObstacles();
    RandomSeed = OriginalSeed;
}

TSharedPtr<FSpawnWithHoudiniConfig> ASpawnWithHoudini::GetConfiguration() const
{
    TSharedPtr<FSpawnWithHoudiniConfig> Config =
            MakeShareable(new FSpawnWithHoudiniConfig);
    Config->SpawnerLocation = this->GetActorLocation();
    Config->SpawnerRotation = this->GetActorRotation();
    Config->MatchBy = MatchBy;
    Config->SurfaceNamePattern = SurfaceNamePattern;
    Config->SurfaceTags = SurfaceTags;
    Config->DensityMin = DensityMin;
    Config->DensityMax = DensityMax;
    Config->RandomSeed = RandomSeed;

    return Config;
}

void ASpawnWithHoudini::Configure(const TSharedPtr<FSpawnWithHoudiniConfig>& Config)
{
    MatchBy = Config->MatchBy;
    SurfaceNamePattern = Config->SurfaceNamePattern;
    SurfaceTags = Config->SurfaceTags;
    DensityMin = Config->DensityMin;
    DensityMax = Config->DensityMax;
    RandomSeed = Config->RandomSeed;
}

int ASpawnWithHoudini::GetActorCount()
{
    if (CachedActorCount == 0)
    {
        CachedActorCount = GetSpawnedActors().Num();
    }

    return CachedActorCount;
}

void ASpawnWithHoudini::BeginPlay()
{
    Super::BeginPlay();

    // Only spawn if there are no spawned objects from the editor.
    if (CurrentGeneration != FromEditor)
    {
        GenerateObstacles();
    }
}

void ASpawnWithHoudini::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (CurrentGeneration != FromEditor)
    {
        ClearObstacles();
    }
}

void ASpawnWithHoudini::AssetPreInstantiation_DelegateHandler(UHoudiniPublicAPIAssetWrapper* SpawnedActor)
{
    PopulateParameters(SpawnedActor);
    RandomizeActor(SpawnedActor);
    PreInstantiationDone.ExecuteIfBound();

    SpawnedActor->GetOnPreInstantiationDelegate().RemoveDynamic(
        this, &ASpawnWithHoudini::AssetPreInstantiation_DelegateHandler);
}

void ASpawnWithHoudini::PostProcessing_DelegateHandler(UHoudiniPublicAPIAssetWrapper* SpawnedActor)
{
    if (IsExportSdf())
    {
        SpawnedActor->SetRemoveOutputAfterBake(true);
        SpawnedActor->BakeAllOutputs();
    }

    PostProcessingDone.ExecuteIfBound();
    SpawnedActor->GetOnPostProcessingDelegate().RemoveDynamic(this, &ASpawnWithHoudini::PostProcessing_DelegateHandler);
}

void ASpawnWithHoudini::PostBake_DelegateHandler(UHoudiniPublicAPIAssetWrapper* SpawnedActor, bool bSuccess)
{
    if (bSuccess)
    {
        ActorBakeCount++;

        if (ActorBakeCount == GetActorCount() && OnSpawnedObjectConfigCompleted.IsBound())
        {
            CreateSpawnedObjectConfiguration();
        }
    }
    else
    {
        USpawnedObjectConfig* EmptyConfig = NewObject<USpawnedObjectConfig>();

        auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(EmptyConfig);

        if (OnSpawnedObjectConfigCompleted.IsBound())
        {
            OnSpawnedObjectConfigCompleted.ExecuteIfBound(FinalConfig, false);
            OnSpawnedObjectConfigCompleted.Unbind();
        }
    }

    SpawnedActor->GetOnPostBakeDelegate().RemoveDynamic(this, &ASpawnWithHoudini::PostBake_DelegateHandler);
}

void ASpawnWithHoudini::CreateSpawnedObjectConfiguration()
{
    bool bSucceeded = false;
    USpawnedObjectConfig* Config = NewObject<USpawnedObjectConfig>();

    try
    {
        TArray<FString> Result;

        const FString PathSuffix = TEXT("uasset");
        IFileManager::Get().FindFiles(Result, *GetBakePathFull(), *PathSuffix);

        const EMatchBy MatchName = NameOrTags;
        const TArray<FName> TagsT;
        for (const FString& FoundBake : Result)
        {
            auto AssetRelativePath = FPaths::Combine(GetBakePathRelative(), FoundBake);
            auto AssetFullPath = FPaths::Combine(GetBakePathFull(), FoundBake);
            auto AssetNameOnly = FoundBake;
            AssetNameOnly.RemoveFromEnd(".uasset");
            const TArray<AActor*> SurfaceActors = AmbitWorldHelpers::GetActorsByMatchBy(
                MatchName, AssetNameOnly, TagsT, true);
            TArray<FTransform> Transforms;
            for (auto* Transform : SurfaceActors)
            {
                Transforms.Add(Transform->GetActorTransform());

                Transform->Destroy();
            }
            Config->SpawnedObjects.Add(AssetRelativePath, Transforms);

            // Upload folder to S3 bucket here, if needed.

            FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*AssetFullPath);
        }

        ClearObstacles();

        bSucceeded = true;
    }
    catch (const std::runtime_error& Re)
    {
        UE_LOG(LogAmbit, Error, TEXT("Failed to bake properly. Error: %i"),
               Re.what());
    }

    auto FinalConfig = TScriptInterface<IConfigJsonSerializer>(Config);

    OnSpawnedObjectConfigCompleted.ExecuteIfBound(FinalConfig, bSucceeded);
}
