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

#include "ExportPlatforms.h"
#include "Constant.h"

TMap<FString, bool> FExportPlatforms::GetSelectionMap() const
{
    TMap<FString, bool> StringToBool;
    StringToBool.Add(ExportPlatform::KLinux, this->bLinux);
    StringToBool.Add(ExportPlatform::KWindows, this->bWindows);
    return StringToBool;
}

TArray<FString> FExportPlatforms::GetSelectedPlatforms() const
{
    TMap<FString, bool> ExportPlatformsMap = this->GetSelectionMap();
    TArray<FString> SelectedPlatforms;
    for (auto& Pair : ExportPlatformsMap)
    {
        if (Pair.Value)
        {
            SelectedPlatforms.Add(*Pair.Key);
        }
    }
    return SelectedPlatforms;
}

void FExportPlatforms::SetLinux(bool SelectedLinux)
{
    this->bLinux = SelectedLinux;
}

bool FExportPlatforms::GetLinux() const
{
    return this->bLinux;
}

void FExportPlatforms::SetWindows(bool SelectedWindows)
{
    this->bWindows = SelectedWindows;
}

bool FExportPlatforms::GetWindows() const
{
    return this->bWindows;
}
