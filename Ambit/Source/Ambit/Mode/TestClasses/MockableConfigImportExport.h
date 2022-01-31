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

#include "Ambit/Mode/ConfigImportExport.h"
#include "Ambit/Mode/TestClasses/MockableGltfExport.h"

#include "MockableConfigImportExport.generated.h"

/**
 * Mock class for ConfigImportExport.h
 *
 * Currently the Lambda implementation is being used for unit tests. This needs to be updated in the future under a new
 * story.
 */
UCLASS()
class UConfigImportExportMock : public UObject, public IConfigImportExport
{
    GENERATED_BODY()
public:
    FReply OnImportSdf() override
    {
        return bHandled ? FReply::Handled() : FReply::Unhandled();
    }

    FReply OnExportSdf() override
    {
        return bHandled ? FReply::Handled() : FReply::Unhandled();
    }

    bool ProcessSdfForExport(const TMap<FString, TSharedPtr<FJsonObject>>& AmbitSpawnerArray, bool bToS3) override
    {
        return bHandled;
    }

    FReply OnImportBsc() override
    {
        return bHandled ? FReply::Handled() : FReply::Unhandled();
    }

    FReply OnGeneratePermutations() override
    {
        return bHandled ? FReply::Handled() : FReply::Unhandled();
    }

    FReply OnReadFromS3Bucket() override
    {
        return bHandled ? FReply::Handled() : FReply::Unhandled();
    }

    FReply OnExportMap() override
    {
        return bHandled ? FReply::Handled() : FReply::Unhandled();
    }

    FReply OnExportGltf() override
    {
        return bHandled ? FReply::Handled() : FReply::Unhandled();
    }

    void SetOutputs(bool Handled)
    {
        bHandled = Handled;
    }

private:
    bool bHandled;
};
