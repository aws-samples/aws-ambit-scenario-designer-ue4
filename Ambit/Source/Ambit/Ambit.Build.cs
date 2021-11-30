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

using UnrealBuildTool;
using System;
using System.IO;

public class Ambit : ModuleRules
{
    public Ambit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { });

        PrivateIncludePaths.AddRange(new string[] { });

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "LevelEditor",
                "EditorStyle",
                "UnrealEd",
                "InputCore",
                "PropertyEditor",
                "Json",
                "JsonUtilities",
                "DesktopPlatform",
                "IntroTutorials",
                "AmbitUtils",
                "AWSSDK",
                "AWSUE4Module",
                "PlacementMode", // used for adding ambit spawner types to placement category
                "PhysXVehicles", // used for Ambit vehicle path generator to support vehicle spawn
                "Projects", // to use IPluginManager
                "HoudiniEngineEditor",
                "HoudiniEngineRuntime",
                "GLTFExporter"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(new string[] { });

        // The AWS SDK relies on some identifiers being undefined for
        // correct behavior (not the same as being defined with value
        // 0). The unreal build tool treats warnings as errors, so
        // rather than disabling that behavior (strict is good), we
        // can just disable the warnings for undefined identifiers.
        bEnableUndefinedIdentifierWarnings = false;
    }
}
