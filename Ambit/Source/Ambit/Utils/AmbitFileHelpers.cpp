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

#include "AmbitFileHelpers.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Misc/FileHelper.h"

namespace AmbitFileHelpers
{
    FString LoadSingleFileFromPopup(const FString& AllowedFileTypes)
    {
        IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
        if (DesktopPlatform == nullptr)
        {
            return "";
        }

        // This opens a dialog box for the user to select the file they want to import
        TArray<FString> OpenFilenames;
        const bool bSuccessfullyOpenedFile = DesktopPlatform->OpenFileDialog(
            FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
            NSLOCTEXT("UnrealEd", "ImportFile", "Import Ambit File").ToString(),
            "", // default path - empty string defaults to the project folder
            TEXT(""), // default file -- empty string does not pre-select a file
            *AllowedFileTypes, // dereference the FString to a const TCHAR*
            EFileDialogFlags::None, // allow for multiple files to be selected? No.
            OpenFilenames); // where to store the file name strings once done

        if (!bSuccessfullyOpenedFile)
        {
            return "";
        }

        FString InString;
        FFileHelper::LoadFileToString(InString, ToCStr(OpenFilenames[0]));

        return InString;
    }

    FString GetPathForFileFromPopup(const FString& FileExtension,
                                    const FString& DefaultPath, const FString& Filename)
    {
        IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
        if (DesktopPlatform == nullptr)
        {
            return "";
        }

        TArray<FString> OpenFilenames;
        const bool bSuccessfullyOpenedFile = DesktopPlatform->SaveFileDialog(
            FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
            NSLOCTEXT("UnrealEd", "ExportFile", "Export Ambit File").ToString(),
            DefaultPath, // Path to open the dialog.
            Filename, // Default filename to use
            FileExtension,
            EFileDialogFlags::None,
            OpenFilenames
        );

        if (!bSuccessfullyOpenedFile)
        {
            return "";
        }

        return OpenFilenames[0];
    }

    FString GetPathForDirectoryFromPopup(const FString& DefaultPath)
    {
        IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
        if (DesktopPlatform == nullptr)
        {
            return "";
        }

        FString OutFolder;
        const bool bSuccessfullyOpenedFile = DesktopPlatform->OpenDirectoryDialog(
            FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
            NSLOCTEXT("UnrealEd", "ChooseLocation", "Select Location for Ambit Files").ToString(),
            DefaultPath,
            OutFolder
        );

        if (!bSuccessfullyOpenedFile)
        {
            return "";
        }

        return OutFolder;
    }


    void WriteFile(const FString& FilePath, const FString& OutString)
    {
        FFileHelper::SaveStringToFile(OutString, *FilePath,
                                      FFileHelper::EEncodingOptions::AutoDetect,
                                      &IFileManager::Get(),
                                      FILEWRITE_None);
    }

    FString CompressFile(const FString& SourceDir, const FString& TargetDir, const FString& FileName,
                         const FString& TargetPlatform)
    {
        int32 ReturnCode = 0;

        FString Executable;
        FString Arguments;
        FString DestinationFile;
        FString Destination;
        FString ErrorMessage;

        if (TargetPlatform == "LinuxNoEditor")
        {
            DestinationFile = FileName + ".tar.gz";
            Destination = FPaths::Combine(*TargetDir, *DestinationFile);
            Executable = "tar";
            Arguments = "-czf " + Destination + " " + FPaths::Combine(*SourceDir, TEXT("*"));
        }
        else
        {
            DestinationFile = FileName + ".zip";
            Destination = FPaths::Combine(*TargetDir, *DestinationFile);

            FString Platform = UGameplayStatics::GetPlatformName();
            if (Platform == "Windows")
            {
                Executable = FPaths::Combine(*FPlatformMisc::GetEnvironmentVariable(TEXT("ProgramW6432")),
                                             TEXT("7-Zip"), TEXT("7z.exe"));
                if (FPaths::FileExists(Executable))
                {
                    Arguments = "a " + Destination + " " + FPaths::Combine(*SourceDir, TEXT("*")) + " -tzip -mmt -mx5";
                }
                else
                {
                    Executable = "powershell";
                    Arguments = "-command \"& { Compress-Archieve -Update -Path " + FPaths::Combine(
                                *SourceDir, TEXT("*"))
                            + " -CompressionLevel Fastest -DestinationPath " + Destination + " }\"";
                }
            }
            else
            {
                Executable = "zip";
                Arguments = "-u " + Destination + " " + FPaths::Combine(*SourceDir, TEXT("*"));
            }
        }

        FProcHandle ProcHandle = FPlatformProcess::CreateProc(*Executable, *Arguments, false, false, false, nullptr, 0,
                                                              nullptr, nullptr);
        if (!ProcHandle.IsValid())
        {
            ErrorMessage = "Failed to create process with command " + Executable + " " + Arguments + ".";
            throw std::runtime_error(
                "Compress file " + std::string(TCHAR_TO_UTF8(*DestinationFile)) + " Failed. " + std::string(
                    TCHAR_TO_UTF8(*ErrorMessage)));
        }
        FPlatformProcess::WaitForProc(ProcHandle);
        FPlatformProcess::GetProcReturnCode(ProcHandle, &ReturnCode);

        if (ReturnCode != 0)
        {
            ErrorMessage = "Command " + Executable + " " + Arguments + " failed with error code " +
                    FString::FromInt(ReturnCode) + ".";
            throw std::runtime_error(
                "Compress file " + std::string(TCHAR_TO_UTF8(*DestinationFile)) + " Failed. " + std::string(
                    TCHAR_TO_UTF8(*ErrorMessage)));
        }
        UE_LOG(LogAmbit, Display, TEXT("Compress file %s for %s Successfully!"), *DestinationFile, *TargetPlatform);

        return DestinationFile;
    }
} // namespace AmbitFileHelpers
