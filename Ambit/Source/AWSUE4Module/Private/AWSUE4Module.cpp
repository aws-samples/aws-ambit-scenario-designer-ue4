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

#include "AWSUE4Module.h"

#include "Core.h"

#include <aws/core/Aws.h>

DEFINE_LOG_CATEGORY(LogAWSUE4Module)
#define LOCTEXT_NAMESPACE "FAWSUE4Module"

void* MemoryManagerWrapper::AllocateMemory(std::size_t blockSize, std::size_t /*alignment*/,
                                           const char* /*allocationTag*/)
{
    return operator new(blockSize);
}

void MemoryManagerWrapper::FreeMemory(void* memoryPtr)
{
    return operator delete(memoryPtr);
}

FAWSUE4Module::FAWSUE4Module()
    : SdkOptions(new Aws::SDKOptions)
{
}

FAWSUE4Module::~FAWSUE4Module()
{
    delete static_cast<Aws::SDKOptions*>(SdkOptions);
    SdkOptions = nullptr;
}

void FAWSUE4Module::StartupModule()
{
    Aws::SDKOptions* AwsSDKOptions = static_cast<Aws::SDKOptions*>(SdkOptions);
    AwsSDKOptions->memoryManagementOptions.memoryManager = &MemoryManager;
    InitAPI(*AwsSDKOptions);
    ApiInitialized = true;
}

void FAWSUE4Module::ShutdownModule()
{
    if (!ApiInitialized)
    {
        return;
    }

    ApiInitialized = false;
    ShutdownAPI(*static_cast<Aws::SDKOptions*>(SdkOptions));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAWSUE4Module, AWSUE4Module)
