# How to use configuration files

## ReSharper settings file
The `Ambit_Resharper.DotSettings` file can be used with [JetBrains Resharper](https://www.jetbrains.com/resharper/) to help developers write better C++ code for the AWS Ambit plugin. This file helps us standardize our code style and ensure uniformity throughout the code base.

### Installation
Import the settings file in Visual Studio by going to **Extensions > ReSharper > Manage Options > Import and Export > Import from File**

### Usage
> **DO NOT** use the ReSharper formatter on any Engine files. This is to be used **only** on AWS Ambit plugin files.

1. In the editor, select the code you want to format. If no code is selected, the whole file will be formatted. You can also select multiple files from the Solution Explorer to format multiple files at a time.
2. Go to **Extensions > ReSharper > Edit > Cleanup Code**
3. In the **ReSharper - Code Cleanup** window, select the **Ambit Quick** configuration.
4. Hit **Run**
