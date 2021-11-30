# AWS Ambit Scenario Designer for Unreal Engine 4

The AWS Ambit Scenario Designer for Unreal Engine 4 (Ambit) plugin for Unreal Engine adds a new Mode, export to test functionality, and a new component for prototype purposes.

## Directory Info

```
📂 Ambit/                      ~ Source code for the Ambit plug-in
📂 docs/                       ~ Source for Ambit documentation
📂 HoudiniDigitalAssets/       ~ Source for Houdini digital assets
📄 user-guide-buildspec.yml    ~ AWS CodeBuild configuration used to build the User Guide
```

## Developer Pre-Requisites

- Unreal Engine 4.27
- Visual Studio 2019
- Houdini 18.5.696 (or higher)

## Installation Instructions

To use the Ambit plugin, place this repository in one of the two locations mentioned below:

- In your Unreal Engine installation directory, under the `Engine > Plugins > Editor` folder. Choosing this method ensures the Ambit plugin is included in every Unreal Engine project you create. _Note: This is the less preferred option unless you also intend to build the engine from source._
- If you want to include this plugin only for specific projects, add it to the `Plugins > Editor` folder for that specific Unreal project. These folders will need to be created if it doesn't already exist.

## Dependencies

### AWS SDK Module

This repository contains the AWS SDK module called AWSUE4Module and pre-compiled aws-sdk-s3 libraries from [AWS SDK for C++ Version 1.9](https://github.com/aws/aws-sdk-cpp/tree/1.9.93).

### Houdini Engine

#### Installation

In order to use the Houdini integration aspects of this code, you will need to download and install the Houdini Plugin (Version 2) for Unreal Engine. This plugin is bundled together with the Houdini installer on Windows. Follow these steps to get the plugin working inside Unreal Engine:

1. Download and launch the Houdini installer. During the installation process, make sure the **Houdini Engine for Unreal** option is selected.
2. Copy the **HoudiniEngine** folder to the `Plugins > Runtime` folder of either your Unreal directory, or your Unreal project directory.
3. Launch Unreal.

For example, for UE 4.27 and Houdini 18.5.696, you will need to copy:

```
C:\Program Files\Side Effects Software\Houdini 18.5.696\engine\unreal\4.27-v2\HoudiniEngine
```

to

```
C:\Program Files\Epic Games\UE_4.27\Engine\Plugins\Runtime\HoudiniEngine
```

Additional information regaring the HoudiniEngine plugin is available at: <https://www.sidefx.com/docs/unreal/_getting_started.html>.

#### License

To obtain a Houdini license for Unreal Engine:

1. Visit [this page](https://www.sidefx.com/buy/#houdini-engine-unreal-unity) in your browser.
2. Create an account and login.
3. Add the **Houdini Engine for Unreal** license to your cart and checkout.

This license is free of cost. Once purchased, you should receive an email with next steps on setting up your Houdini Engine license.

#### Digital Assets

Further, you'll need to copy the file `<AmbitPluginRoot>/HoudiniDigitalAssets/AWSAmbit.hda` into the special user-specific Houdini OTL directory. On Windows you'll find this directory at `C:\Users\<username>\Documents\houdiniXX.Y\otls`. You will need to create the `otls` folder if it doesn't exist.

For example, for Houdini 18.5, the final file location should be:

```
C:\Users\<username>\Documents\houdini18.5\otls\AWSAmbit.hda
```

### glTF Exporter Plugin

The glTF Exporter Plugin needs to be installed to Unreal Engine in order to work with the Ambit plugin and to export any static meshes as glTF. There are two ways to install the plugin to your Unreal Engine version:

- Using your preferred internet browser.
- Using the Epic Games Launcher.

#### Using the browser

1. Visit [this link](https://www.unrealengine.com/marketplace/en-US/product/gltf-exporter) in your browser.
2. If you aren't already signed in, Click on the **Sign in to Buy** button to login to the marketplace using your Epic Games ID.
3. Once signed in, click on the **Install to Engine** button and select the engine version before proceeding.

#### Using the Epic Games Launcher

1. Open the **Epic Games Launcher**
2. Click on the **Marketplace** tab at the top of the launcher.
3. Search for "gltf exporter" and open the plugin made by **Epic Games**
4. Click on the **Install to Engine** button and select the engine version before proceeding.

## Build Instructions

Once the Ambit plugin is installed in the desired location, head over to your Unreal Engine project and right-click on the `*.uproject` file at the root of the project directory and select the **Generate Visual Studio project files** option. Select the appropriate engine version if required and proceed with generating the files.

Once completed, launch the `*.sln` file in Visual Studio. In the **Solution Explorer** window, right-click on your project and select **Set as Startup Project**. To build the solution, select `Build > Build Solution` from the Visual Studio menubar.

To launch the project, hit the **Local Windows Debugger** button, or press **F5**.

You can also launch the project by opening the `*.uplugin` file present at the root of your project directory.

## Package instructions

To create pre-built binaries of the plugin, follow these steps:

1. Please make sure dependencies of Ambit plugin are set up in the **Engine folder** before going to the next step.
2. Click **Edit** tab and open **Plugins** window.
3. Find the **AWS Ambit Scenario Designer** plugin.
4. Click the **Package** button and choose the folder you would like to save the plugin.
5. Wait for the packaging process to complete and use the **Output Log** to see logs.

You can use this pre-built plugin in both Unreal Engine C++ and Blueprint Projects by installing it in your Project Plugins folder or the Engine Plugins folder.

## Testing instructions

### Testing Ambit Spawners

To manually test the **Ambit Spawners**, open and run the map found at `/Ambit Content/Test/Maps/ProceduralPlacementTestMap` and use the existing **Spawn on Surface** actor.

### Testing Import/Export

To manually test import and export functionality, follow these steps:

1. First set some random parameters in the **Scenario Settings** section of the Ambit Mode.
2. Click the "Export Scenario" Button at the bottom of the **Scenario Settings** section.
3. Verify the file was created in the **Project Folder** of your project.
4. Reset all the values of your **Scenario Settings** section or re-load the project.
5. Click the '...' next to **Scenario Name** and import the file.

### Running Automated Tests

To enable the Unreal Engine editor to support automated tests, open the Unreal Engine project, enable the plugin called "Functional Testing Editor", and restart the editor.

Automated test files - called "specs" - end with a ".spec.cpp" file extension and are usually located next to the files that they test. These tests will be automatically recognized by the Unreal Engine test runner. To run the tests:

1. First open the **Test Automation** window (`Window > Test Automation`).
2. Select the **Automation** tab.
3. Tick the box next to the **Ambit** group of tests.
4. Click the **Start Tests** button.

For more info on working with automated tests in UE see ["Automation Spec Overview"](https://docs.unrealengine.com/en-US/Programming/Automation/AutomationSpec/index.html)

## User Guide

To learn how to use the Ambit plugin, you can take a look at the [User Guide](https://aws-samples.github.io/aws-ambit-scenario-designer-ue4/). This guide will help you learn how to use the various aspects of the Ambit plugin such as the AWS Ambit Scenario Mode, its weather and scenario generation options, along with the Ambit Spawner and Ambit Path Generation tools.

To contribute to the User Guide, follow the instructions listed [here](./docs/user-guide/README.md).

Meanwhile, you can use the In-Editor tutorials to get familiar with AWS Ambit Scenario Mode. Click the tutorial button at the top right corner of the Unreal Editor or the top right corner of the AWS Ambit Scenario Mode and select the **AWS Ambit** chapter to view all tutorials.

## Other Information

| File | Description |
| ----- | ----- |
| CONTRIBUTING.md | All the information you need to know if you would like to contribute to this project |
| LICENSE | Covers the license information for this project |
| NOTICE.md | Amazon.com, Inc.'s copyright for this project |
| THIRD_PARTY_LICENSES.md | Contains a list of assets obtained from third parties |
