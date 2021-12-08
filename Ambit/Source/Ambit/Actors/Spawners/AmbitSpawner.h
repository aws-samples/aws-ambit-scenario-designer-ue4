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
#include "UObject/Interface.h"

#include <AmbitUtils/ConfigJsonSerializer.h>

#include "AmbitSpawner.generated.h"

UINTERFACE()
class AMBIT_API UAmbitSpawner : public UInterface
{
    GENERATED_BODY()
};

class AMBIT_API IAmbitSpawner
{
    GENERATED_BODY()

public:
    DECLARE_DELEGATE_TwoParams(FOnSpawnedObjectsConfigCompleted, TScriptInterface<IConfigJsonSerializer>&, bool);

    /**
     * Determines if there are actors or objects that the spawner is able to spawn.
     *
     * @return Returns a boolean if there is at least 1 actor to spawn. 
     */
    virtual bool HasActorsToSpawn() const = 0;

    /**
     * Retrieves the configuration for the objects that the spawner creates.
     * Once completed, should call OnSpawnedObjectConfigCompleted with a pointer
     * to the IConfigJsonSerializer implementation containing the configuration
     * for the spawned objects, and a boolean to indicate if the operation was successful.
     */
    virtual void GenerateSpawnedObjectConfiguration() = 0;

    /**
     * Retrieves the configuration for the objects that the spawner creates.
     * Once completed, should call OnSpawnedObjectConfigCompleted with a pointer
     * to the IConfigJsonSerializer implementation containing the configuration
     * for the spawned objects, and a boolean to indicate if the operation was successful.
     *
     * @param Seed the seed to set the spawner to, if applicable.
     */
    virtual void GenerateSpawnedObjectConfiguration(int Seed) = 0;

    /**
     * Retrieves the delegate indicating that the spawned object configuration
     * has completed.
     *
     * @return A delegate that can be bound.
     */
    virtual FOnSpawnedObjectsConfigCompleted& GetOnSpawnedObjectConfigCompletedDelegate()
    {
        return OnSpawnedObjectConfigCompleted;
    };

protected:
    FOnSpawnedObjectsConfigCompleted OnSpawnedObjectConfigCompleted;
};
