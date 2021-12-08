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

#include "AWSUEStringUtils.h"

#include <string>

Aws::String AWSUEStringUtils::FStringToAwsString(const FString& UnrealString)
{
    const std::string StdString = std::string(TCHAR_TO_UTF8(*UnrealString));
    Aws::String AwsString(StdString.c_str(), StdString.size());
    return AwsString;
}

FString AWSUEStringUtils::AwsStringToFString(const Aws::String& AwsString)
{
    const std::string StdString(AwsString.c_str(), AwsString.size());
    return FString(StdString.c_str());
}
