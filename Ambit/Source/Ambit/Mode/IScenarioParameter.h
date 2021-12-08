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

struct FScenarioDefinition;

struct AMBIT_API IScenarioParameter
{
    virtual ~IScenarioParameter() = default;

    /**
     * Returns the number of variants of this specific parameter.
     */
    virtual int32 GetVariantCount() = 0;

    /**
     * Applies a specific variant of this parameter to the provided scenario
     * definition instance. This is usually accomplished by updating one or more
     * properties on the scenario definition that related to this parameter.
     */
    virtual void ApplyVariant(int32 VariantIndex, FScenarioDefinition& Scenario) = 0;
};
