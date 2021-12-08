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

#include "Constant.h"

#include "ExportPlatforms.generated.h"

/**
 * Export Platform struct
 */
USTRUCT()
struct AMBIT_API FExportPlatforms
{
    GENERATED_BODY()

    FExportPlatforms()
        : bLinux(false)
          , bWindows(false)
    {
    }

private:
    UPROPERTY(EditAnywhere, Category = ExportPlatform, meta = (DisplayName = "Linux"))
    bool bLinux;
    UPROPERTY(EditAnywhere, Category = ExportPlatform, meta = (DisplayName = "Windows"))
    bool bWindows;

public:
    /**
     * Generates a TMap(Key: Weather Types String, Value: Weather Types bool)
     */
    TMap<FString, bool> GetSelectionMap() const;

    /**
     * Returns a list of the weather types that are currently selected.
     */
    TArray<FString> GetSelectedPlatforms() const;

    /**
     * Getter and Setter for export platforms
     */
    void SetLinux(bool SelectedLinux);

    bool GetLinux() const;

    void SetWindows(bool SelectedWindows);

    bool GetWindows() const;
};
