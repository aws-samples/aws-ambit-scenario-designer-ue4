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

#include <utility>

#include "Ambit/Mode/GltfExport.h"

#include "MockableGltfExport.generated.h"

/**
 * Mock class for GltfExport.h
 * To be used only during testing.
 */
UCLASS()
class UGltfExportMock : public UObject, public IGltfExport
{
    GENERATED_BODY()
public:
    GltfExportReturnCode Export(UWorld* World, const FString& Filename) const override
    {
        return ReturnCode;
    }

    void SetOutput(GltfExportReturnCode Code)
    {
        ReturnCode = Code;
    }
private:
    GltfExportReturnCode ReturnCode;
};
