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
#include "SpawnerBase.h"

#include "SpawnOnPath.generated.h"

struct FSpawnOnPathConfig;
/**
 * This class provides random generation of obstacles along a spline.
 */
UCLASS()
class AMBIT_API ASpawnOnPath : public ASpawnerBase
{
    GENERATED_BODY()
public:
    // Sets default values for this actor's properties
    ASpawnOnPath();

    UPROPERTY(VisibleAnywhere, Category = "Ambit Spawner")
    class USplineComponent* Spline;

    /**
    If no surfaces are specified in Match By, when true,
    Ambit Spawner will snap obstacles to surfaces below.
    */
    UPROPERTY(EditAnywhere, Category = "Ambit Spawner")
    bool bSnapToSurfaceBelow = false;

    /**
    Rotation will follow the spline. Overrides degrees of rotation.
    */
    UPROPERTY(EditAnywhere, Category = "Ambit Spawner",
        meta = (DisplayAfter = "DensityMax"))
    bool bFollowSplineRotation = false; // Put follow spline checkbox below density fields

    /**
     * See https://docs.unrealengine.com/4.26/en-US/API/Runtime/Engine/GameFramework/AActor/PostEditChangeProperty/
     */
    void PostEditChangeProperty(
        FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * @inheritDoc
     */
    TSharedPtr<FSpawnOnPathConfig> GetConfiguration() const;

    /**
     * @inheritDoc
     */
    void Configure(const TSharedPtr<FSpawnOnPathConfig>& Config);

private:
    // Spawns actors as specified by this object's parameters.
    TMap<FString, TArray<FTransform>> GenerateActors() override;
};
