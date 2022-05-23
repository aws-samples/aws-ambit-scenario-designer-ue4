//   Copyright 2022 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
#include "IScenarioParameter.h"

#include <AmbitUtils/ConfigJsonSerializer.h>

#include "PedestrianTraffic.generated.h"

/**
 * Pedestrian Traffic struct
 */
USTRUCT()
struct AMBIT_API FPedestrianTraffic
#if CPP
        : public IScenarioParameter, public FConfigJsonSerializer
#endif
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = PedestrianTraffic,
        meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", DisplayName = "DensityMin"))
    float Min = 0.0f;
    UPROPERTY(EditAnywhere, Category = PedestrianTraffic,
        meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", DisplayName = "DensityMax"))
    float Max = 0.0f;
    UPROPERTY(VisibleAnywhere, Category = PedestrianTraffic, meta = (DisplayName = "Increment"))
    float Increment = 0.1f;

    /**
     * @see AmbitUtils/FConfigJsonSerializer.h#SerializeToJson()
     */
    TSharedPtr<FJsonObject> SerializeToJson() const override;

    /**
     * @see AmbitUtils/FConfigJsonSerializer.h#DeserializeFromJson(TSharedPtr<FJsonObject>)
     */
    void DeserializeFromJson(TSharedPtr<FJsonObject> JsonObject) override;

    /**
     * @see IScenarioParameter#GetVariantCount()
     */
    int32 GetVariantCount() override;

    /**
     * @see IScenarioParameter#ApplyVariant(int32 VariantIndex, FScenarioDefinition& Scenario)
     */
    void ApplyVariant(int32 VariantIndex, FScenarioDefinition& Scenario) override;
};
