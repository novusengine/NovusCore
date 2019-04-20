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
#include <Utils/DebugHandler.h>
#include <filesystem>
#include <sstream>
#include "ADT.h"
#include "../MPQ/MPQHandler.h"

namespace MapLoader
{
void LoadMaps(MPQHandler& handler, std::vector<std::string> adtLocationOutput)
{
    NC_LOG_MESSAGE("Extracting ADTs...");
    std::filesystem::path basePath(std::filesystem::current_path().string() + "/NovusExtractor");
    std::filesystem::path mapPath(basePath.string() + "/maps");
    if (!std::filesystem::exists(mapPath))
    {
        std::filesystem::create_directory(mapPath);
    }

    MPQFile file;
    for (std::string adtName : adtLocationOutput)
    {
        bool createAdtDirectory = true;
        std::filesystem::path adtPath(basePath.string() + "/maps/" + adtName);
        if (std::filesystem::exists(adtPath))
        {
            createAdtDirectory = false;
        }

        for (u32 x = 0; x < 64; x++)
        {
            for (u32 y = 0; y < 64; y++)
            {
                // We could read the WDL file here to get the ADT list

                std::stringstream fileNameStream;
                std::stringstream filePathStream;
                fileNameStream << adtName << "_" << x << "_" << y;
                std::string fileName = fileNameStream.str();

                filePathStream << "world\\maps\\" << adtName << "\\" << fileName << ".adt";
                if (handler.GetFile(filePathStream.str(), file))
                {
                    if (createAdtDirectory)
                    {
                        std::filesystem::create_directory(adtPath);
                        createAdtDirectory = false;
                    }

                    ADT mapAdt(file, fileName + ".nmap", adtPath.string());
                    mapAdt.Convert();
                }
            }
        }
    }

    return;
}
} // namespace MapLoader