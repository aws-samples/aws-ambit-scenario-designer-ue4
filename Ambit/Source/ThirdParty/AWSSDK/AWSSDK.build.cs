/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

using System;
using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class AWSSDK : ModuleRules
{
    private List<string> LibraryNames = new List<string>()
    {
        "aws-c-auth",
        "aws-c-cal",
        "aws-c-common",
        "aws-c-compression",
        "aws-c-event-stream",
        "aws-c-http",
        "aws-c-io",
        "aws-c-mqtt",
        "aws-c-s3",
        "aws-checksums",
        "aws-cpp-sdk-core",
        "aws-crt-cpp",
        "aws-cpp-sdk-s3",
        "aws-cpp-sdk-firehose",
        "aws-cpp-sdk-kinesis",
        "aws-cpp-sdk-sts",
        "aws-cpp-sdk-cognito-identity",
        "aws-cpp-sdk-identity-management",
    };

    public AWSSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        //The ModuleType.External setting tells the engine not to look for (or compile) source code.
        Type = ModuleType.External;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        // The AWS SDK relies on some identifiers being undefined for
        // correct behavior (not the same as being defined with value
        // 0). The unreal build tool treats warnings as errors, so
        // rather than disabling that behavior (strict is good), we
        // can just disable the warnings for undefined identifiers.
        bEnableUndefinedIdentifierWarnings = false;

        // Dynamically linking to the SDK requires us to define the
        // USE_IMPORT_EXPORT symbol for all build targets using the
        // SDK. Source: https://github.com/aws/aws-sdk-cpp/blob/main/Docs/SDK_usage_guide.md#build-defines
        PublicDefinitions.Add("USE_IMPORT_EXPORT");
        PublicDefinitions.Add("AWS_CRT_CPP_USE_IMPORT_EXPORT");


        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.Add("USE_WINDOWS_DLL_SEMANTICS");
        }
        PublicIncludePaths.Add(ModuleDirectory);
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));

        LibraryNames.ForEach(AddLibrary);
    }

    private void AddLibrary(string LibraryName)
    {
        string Platform = Target.Platform.ToString();
        string LibraryPath = Path.Combine(ModuleDirectory, Platform);

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Add the library with symbols required by the linker (.lib for dynamic libraries on windows).
            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, LibraryName + ".lib"));
            // Stage the library along with the target, so it can be loaded at runtime.
            RuntimeDependencies.Add("$(BinaryOutputDir)/" + LibraryName + ".dll",
                Path.Combine(LibraryPath, LibraryName + ".dll"));
        }
        else
        {
            throw new PlatformNotSupportedException(
                "Platform " + Platform + " is not supported by the CoreSDK Module.");
        }
    }
}
