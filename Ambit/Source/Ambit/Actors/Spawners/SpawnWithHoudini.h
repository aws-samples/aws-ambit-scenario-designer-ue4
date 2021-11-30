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

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HoudiniEngineEditor/Public/HoudiniPublicAPIAssetWrapper.h"
#include "Misc/AutomationTest.h"

#include "AmbitSpawner.h"
#include "Ambit/Actors/SpawnerConfigs/SpawnWithHoudiniConfig.h"
#include "Ambit/Utils/MatchBy.h"

#include "SpawnWithHoudini.generated.h"

class AHoudiniAssetActor;
class USpawnedObjectConfig;

UENUM()
enum EGenerationType
{
    NotGenerated,
    FromEditor,
    FromRuntime
};

/**
 * A helper structure meant to keep Houdini Asset handling centralized.
 */
USTRUCT(BlueprintType)
struct AMBIT_API FHoudiniLoadableAsset
{
    GENERATED_BODY()

public:

    /**
     * A specific Houdini Digital Asset that we wish to load to the screen.
     */
    UPROPERTY(EditAnywhere, meta =
        (DisplayName = "Houdini Asset"),
        Category = "Ambit Spawner")
    UHoudiniAsset* HDAToLoad;

    /**
     * A list of the parameters that we will randomize. If it is not in this list,
     * it will not be randomized.
     */
    UPROPERTY(EditAnywhere, meta =
        (DisplayName = "Parameters to randomize"),
        Category = "Ambit Spawner")
    TSet<FName> ParamsToRandom;

    /**
     * A list of loaded Houdini assets that are on screen. This does not hold a strong
     * reference to the actor so should be checked for validity before using, using ->IsValidLowLevel(),
     * as it may have been deleted/GC'd at some point.
     */
    UPROPERTY()
    TArray<UHoudiniPublicAPIAssetWrapper*> SpawnedActors;
};

/**
 * This class handles the spawning of Houdini Digital Assets.
 */
UCLASS()
class AMBIT_API ASpawnWithHoudini : public AActor, public IAmbitSpawner
{
    GENERATED_BODY()
public:
    ASpawnWithHoudini();
    ~ASpawnWithHoudini();

    class UBillboardComponent* IconComponent;

    /**
     * Change this value to generate different random arrangements.
     */
    UPROPERTY(EditAnywhere,
        Category = "Ambit Spawner")
    int32 RandomSeed = 0;

    /**
     * Select to Match By AND/OR
     */
    UPROPERTY(EditAnywhere,
        Category = "Ambit Spawner")
    TEnumAsByte<EMatchBy> MatchBy = EMatchBy::NameAndTags;

    /**
     * The search string used to find actors representing surfaces to spawn onto.
     */
    UPROPERTY(EditAnywhere,
        Category = "Ambit Spawner")
    FString SurfaceNamePattern = "";

    /**
     *The tag name used to find actors representing surfaces to spawn onto.
     */
    UPROPERTY(EditAnywhere,
        Category = "Ambit Spawner")
    TArray<FName> SurfaceTags;

    /**
     * The maximum random value floats can be adjusted to.
     */
     float FloatMax = 100.f;

    /**
     * The maximum random value ints can be adjusted to.
     */
    int32 IntMax = 100;

    /**
     * The minimum number of items to spawn per square meter on average.
     */
    UPROPERTY(EditAnywhere, meta =
        (DisplayName = "Items Per Meter (Min)"),
        Category = "Ambit Spawner")
    float DensityMin = 0.05f;

    /**
     * The maximum number of items to spawn per square meter on average.
     */
    UPROPERTY(EditAnywhere, meta = (DisplayName =
        "Items Per Meter (Max)"),
        Category = "Ambit Spawner")
    float DensityMax = 0.2f;

    /**
     * The HDA that will be spawned randomly.
     */
    UPROPERTY(EditAnywhere, meta =
        (DisplayName = "Houdini Asset", ShowOnlyInnerProperties, AdvancedDisplay),
        Category = "Ambit Spawner")
    TArray<FHoudiniLoadableAsset> HoudiniAssetDetails;

    /**
     * For internal testing only. Returns when PreInstantiation has been completed on the HDAs
     * triggered by Houdini Plugin's response.
     */
    FDoneDelegate PreInstantiationDone;

    /**
     * For internal testing only. Returns when PostProcessing has been completed on the HDAs
     * triggered by Houdini Plugin's response.
     */
    FDoneDelegate PostProcessingDone;

    /**
     * @inheritDoc
     */
    bool HasActorsToSpawn() const override;

    /**
     * Generates actor obstacles from the HDA specified. This will also call
     * each actor for randomization (using RandomizeActor) once Houdini has
     * prepared the asset for cooking. 
     */
    void GenerateObstacles();

    /**
     * Destroys the actors that have been created and clears out references.
     *
     *@return Returns a boolean if the operation was successful (true) or if the obstacles could not be cleared (false).
     */
    bool ClearObstacles();

    /**
     * For the incoming Actor, populate the ParamsToRandom
     * that matches the Actor's HDA.
     *
     *@param SpawnedActor A reference to a Houdini Public API Asset Wrapper that has been
     * properly instantiated and is at least in the PreInstantiation stage from Houdini's
     * creation workflow.
     */
    void PopulateParameters(UHoudiniPublicAPIAssetWrapper* SpawnedActor);

    /**
     * For the incoming Actor, randomize the parameters of its asset as long
     * as the parameter is within the ParamsToRandom for the corresponding HDA
     * and will be within the range of [0, IntMax/FloatMax] if Int/Float respectively.
     * Currently, does not randomize String parameters and takes whatever the current value is instead.
     *
     *@param SpawnedActor A reference to a Houdini Public API Asset Wrapper that has been
     * properly instantiated and is contained within one of the HoudiniAssetDetail's classes.
     * This parameter will be modified by the end of the function to have a changed set of internal
     * parameters, and will be set to be recooked by the end of the call.
     */
    void RandomizeActor(UHoudiniPublicAPIAssetWrapper* SpawnedActor);

    /**
     * Get the array of spawned Houdini Asset Actors. Mainly used for tests. These objects are not guaranteed
     * to exist, and should be verified like any other TWeakObjectPtr on usage. 
     *
     *@return A TArray of a softly-linked referenced to all of the actors spawned by this system.
     */
    TArray<UHoudiniPublicAPIAssetWrapper*> GetSpawnedActors() const;

    /**
     * Resets the instance of the object back to its initial form for the settings,
     * and parameters. 
     */
    void ResetObstacleSpawner();

    /**
     * @inheritDoc
     */
    TSharedPtr<FSpawnWithHoudiniConfig> GetConfiguration() const;

    /**
     * @inheritDoc
     */
    void Configure(const TSharedPtr<FSpawnWithHoudiniConfig> Config);

    /**
     * @inheritDoc
     * Starts the process for the Houdini Spawner that will load and bake the objects.
     */
    void GenerateSpawnedObjectConfiguration() override;

    /**
     * @inheritDoc
     * Starts the process for the Houdini Spawner that will load and bake the objects.
     */
    void GenerateSpawnedObjectConfiguration(int32 Seed) override;

    /**
     * This method will call several of the class methods to help create the object and set up the HDA for usage.
     * It signals PreInstantiationDone when completed.
     *
     *@param SpawnedActor A reference to a Houdini Public API Asset Wrapper that has been
     * properly instantiated and is at least in the PreInstantiation stage from Houdini's
     * creation workflow.
     */
    UFUNCTION(CallInEditor, Category = "Ambit Spawner Delegate Handler")
    void AssetPreInstantiation_DelegateHandler(UHoudiniPublicAPIAssetWrapper* SpawnedActor);

    /**
     * This method indicates when a particular HDA has finished processing, and is rendered to screen in the event
     * we need to do some post-handling for it.
     * It signals PostProcessingDone when completed.
     *
     *@param SpawnedActor A reference to a Houdini Public API Asset Wrapper that has been
     * properly instantiated and is at least in the PostProcessing stage from Houdini's
     * creation workflow.
     */
    UFUNCTION(CallInEditor, Category = "Ambit Spawner Delegate Handler")
    void PostProcessing_DelegateHandler(UHoudiniPublicAPIAssetWrapper* SpawnedActor);

    /**
     * This method indicates when a particular HDA has finished baking the object to disk. 
     * It signals PostProcessingDone when completed.
     *
     *@param SpawnedActor A reference to a Houdini Public API Asset Wrapper that has been
     * properly instantiated and is at least in the PostBake stage from Houdini's
     * creation workflow.
     *
     *@param bSuccess Boolean to determine if the baking was successful. 
     */
    UFUNCTION(CallInEditor, Category = "Ambit Spawner Delegate Handler")
    void PostBake_DelegateHandler(UHoudiniPublicAPIAssetWrapper* SpawnedActor, bool bSuccess);

    /**
     * Quick access to return the number of actors that are currently spawned. 
     */
    int GetActorCount();

protected:
    /**
     * Called when the game starts or when spawned
     */
    void BeginPlay() override;

    /**
     * Called when the game end or is de-spawned
     */
    void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /**
     * A stored random number generator for usages to ensure consistent randomness across functions.
     */
    FRandomStream Random;

    /**
     * A stored reference to the HoudiniPublicAPI.
     */
    UPROPERTY()
    UHoudiniPublicAPI* HoudiniApi;

    /**
     * Determines what type of spawning generation the spawner currently is in. 
     */
    UPROPERTY()
    TEnumAsByte<EGenerationType> CurrentGeneration;

    /**
     * The current number of actors that have been baked.
     */
    int ActorBakeCount;

    /**
     * A cached instance of the actor count.
     */
    int CachedActorCount;

    /**
     * Determines if we are trying to export to SDF, which requires we bake assets to disk. 
     */
    bool IsExportSdf() const { return OnSpawnedObjectConfigCompleted.IsBound(); };

    /**
     * A content-specified path (based on the Game's Content folder) to where the baked objects will be placed.
     */
    FString GetBakePathRelative() const { return "/Game/Ambit/" + this->GetName(); };

    /**
     * An absolute path on disk to the Content directory to where the baked objects will be placed.
     */
    FString GetBakePathFull() const { return FPaths::ProjectContentDir() + "/Ambit/" + this->GetName(); };

    /**
     * Handles the SDF export by finding all of the baked objects, packaging them for the SDF,
     * deleting them from disk, and then finally exporting out that configuration to the delegate.
     * Once completed, will send the delegate through the bound OnCompleted Delegate, and inform if the
     * process was successful or not.
     *
     * This method assumes that all appropriate objects have been baked to disk and are currently loaded in the world. 
     */
    void CreateSpawnedObjectConfiguration();
};
