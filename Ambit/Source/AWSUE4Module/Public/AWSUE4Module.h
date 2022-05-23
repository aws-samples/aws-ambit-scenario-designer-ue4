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

#include <aws/core/utils/memory/MemorySystemInterface.h>

DECLARE_LOG_CATEGORY_EXTERN(LogAWSUE4Module, Log, All);

/**
 * Memory manager wrapper to provide the AWS Sdk to use Unreal's memory management
 * (FMemory through overloaded new/delete operators by default) for dynamic allocations
 */
class MemoryManagerWrapper : public Aws::Utils::Memory::MemorySystemInterface
{
public:
    /**
     * Allocate the memory with the block size by using Unreal Engine override new
     */
    void* AllocateMemory(std::size_t blockSize, std::size_t alignment, const char* allocationTag = nullptr) override;

    /**
     * Free the memory pointed to by pointer by using Unreal Engine override delete
     */
    void FreeMemory(void* memoryPtr) override;

    /**
     * Must override due to the pure virtual method in the parent interface
     */
    void Begin() override
    {
    };

    /**
     * Must override due to the pure virtual method in the parent interface
     */
    void End() override
    {
    };
};

/**
 * Manages the runtime resources required by the AWS UE4 Module. In particular, this includes
 * loading and unloading of the AWS libraries during module startup and shutdown.
 *
 * @see IModuleInterface for details
 */
class AWSUE4MODULE_API FAWSUE4Module : public IModuleInterface
{
public:
    /**
     * Constructs a FAWSUE4Module instance.
     */
    FAWSUE4Module();

    /**
     * Destructs a FAWSUE4Module instance.
     */
    virtual ~FAWSUE4Module() override;

    /**
     * Called right after the module DLL has been loaded and the module object has been created
     * Load dependent modules here, and they will be guaranteed to be available during ShutdownModule. ie:
     *
     * FModuleManager::Get().LoadModuleChecked(TEXT("HTTP"));
     *
     * @see IModuleInterface for details
     */
    void StartupModule() override;

    /**
     * Called before the module is unloaded, right before the module object is destroyed.
     * During normal shutdown, this is called in reverse order that modules finish StartupModule().
     * This means that, as long as a module references dependent modules in it's StartupModule(), it
     * can safely reference those dependencies in ShutdownModule() as well.
     *
     * @see IModuleInterface for details
     */
    void ShutdownModule() override;

private:
    /**
     * Reference to Aws::SDKOptions. This is needed to make sure to pass the same
     * instance to Aws::InitAPI and Aws::ShutdownAPI as dictated by the
     * [AWS docs](). Using type `void *` to encapsulate implementation details of
     * this module (i.e. including the AWS SDK headers in this module's interface).
     */
    void* SdkOptions{nullptr};

    /**
     *  A boolean variable to demonstrate whether the api has been initialized.
     */
    bool ApiInitialized{false};

    MemoryManagerWrapper MemoryManager;
};
