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

class FString;

namespace AmbitFileHelpers
{
    /**
     * Pops up a dialog and allows the user to select a single file of the allowed types.
     * Then parses and returns a string of the file contents.
     *
     *@param AllowedFileTypes a string containing the allowed file types. Empty if all.
     *
     *@return The file contents parsed as an FString. Empty string if error.
     */
    FString LoadSingleFileFromPopup(const FString& AllowedFileTypes);

    /**
     * Pops up a dialog and allows the user to specify a new file to save.
     *
     *@param FileExtension valid file extensions to overwrite/save.
     *@param DefaultPath (Optional) Path to default the popup to.
     *@param Filename (Optional) default file name to use
     *
     *@return returns the full path of the file to save. Empty string if none.
     */
    FString GetPathForFileFromPopup(const FString& FileExtension, const FString& DefaultPath = "",
                                    const FString& Filename = "");

    /**
     * Pops up a window and returns the path to a directory selected by th user.
     *
     *
     */
    FString GetPathForDirectoryFromPopup(const FString& DefaultPath = "");

    /**
     * Creates a file at the Path and Name provided if it doesn't exist and then writes
     * the String to that file. If the file does exist, it will re-write it.
     *
     * @param Path The path to write the file
     * @param OutFile the File Nam     * @Param OutString the String to write to the file
     *
     */
    void WriteFile(const FString& FilePath, const FString& OutString);

    FString CompressFile(const FString& SourceDirectory, const FString& TargetDirectory, const FString& FileName,
                         const FString& TargetPlatform);
} // namespace AmbitFileHelpers
