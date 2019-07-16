/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once

#include <NovusTypes.h>
#include <Utils/DebugHandler.h>
#include <filesystem>
#include <vector>
#include "MPQArchive.h"
#include "MPQFile.h"

const std::vector<std::string> MPQArchives = {"patch.MPQ", "patch-2.MPQ", "patch-3.MPQ", "patch-enUS.MPQ", "patch-enUS-2.MPQ", "patch-enUS-3.MPQ", "patch-enGB.MPQ", "patch-enGB-2.MPQ", "patch-enGB-3.MPQ"};
class MPQHandler
{
public:
    MPQHandler() : Archives() {}

    bool IsMPQWanted(std::string fileName)
    {
        for (i32 i = 0; i < static_cast<i32>(MPQArchives.size()); i++)
        {
            if (fileName == MPQArchives[i])
                return true;
        }

        return false;
    }
    i32 GetMPQIndex(std::string fileName)
    {
        for (i32 i = 0; i < static_cast<i32>(MPQArchives.size()); i++)
        {
            if (fileName == MPQArchives[i])
                return i;
        }

        return -1;
    }

    bool Load()
    {
        u32 counter = 0;
        std::filesystem::path basePath = std::filesystem::current_path();

        std::vector<MPQArchive> tempList;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(basePath))
        {
            if (entry.is_regular_file())
            {
                std::filesystem::path file = std::filesystem::path(entry.path());
                std::string fileName = file.filename().string();
                if (IsMPQWanted(fileName))
                {
                    try
                    {
                        std::string mpqPath = file.lexically_relative(basePath).string();

                        MPQArchive archive(mpqPath, fileName);
                        if (archive.Open())
                        {
                            counter++;
                            tempList.push_back(archive);
                        }
                    }
                    catch (std::exception e)
                    {
                        NC_LOG_ERROR(e.what());
                    }
                }
            }
        }

        Archives.resize(counter);
        for (MPQArchive archive : tempList)
        {
            i32 mpqIndex = GetMPQIndex(archive.name);
            if (mpqIndex == -1)
                continue;

            Archives[mpqIndex] = archive;
        }

        return counter != 0;
    }
    bool GetFile(std::string name, MPQFile& output)
    {
        bool fileFound = false;

        for (MPQArchive archive : Archives)
        {
            u32 fileNumber = 0;
            if (!archive.GetFileNumber(name, fileNumber))
                continue;

            i64 fileSize = 0;
            if (!archive.GetFileSize_Unpacked(fileNumber, fileSize) || fileSize <= 1)
                continue;

            MPQFile file(name);
            file.Buffer.Resize(fileSize);

            if (!archive.ReadFile(fileNumber, file.Buffer.data(), fileSize))
                continue;

            output = file;
            fileFound = true;
        }

        return fileFound;
    }

    void CloseAll()
    {
        for (MPQArchive archive : Archives)
        {
            archive.Close();
        }
    }

    std::vector<MPQArchive> Archives;
};