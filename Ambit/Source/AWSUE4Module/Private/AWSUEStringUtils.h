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

#include <aws/core/Region.h>

namespace AWSUEStringUtils
{
    /**
     * From Github link:
     * https://github.com/aws/aws-sdk-cpp/blob/master/aws-cpp-sdk-core/include/aws/core/Region.h
     */
    const TArray<FString> AwsRegionsAsFStrings = {
        Aws::Region::US_EAST_1,
        Aws::Region::US_EAST_2,
        Aws::Region::US_WEST_1,
        Aws::Region::US_WEST_2
    };

    /**
     * Converts FString(Unreal Engine type string) into Aws::String
     */
    Aws::String FStringToAwsString(const FString& UnrealString);

    /**
     * Converts Aws::String into FString
     */
    FString AwsStringToFString(const Aws::String& AwsString);
};
