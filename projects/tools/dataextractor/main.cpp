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

#include <NovusTypes.h>
#include <Utils/DebugHandler.h>
#include <fstream>
#include "MPQ/MPQHandler.h"
#include "DBC/DBCLoader.h"
#include "MAP/MAPLoader.h"

#ifdef _WIN32
#include <Windows.h>
#endif

//The name of the console window.
#define WINDOWNAME "Data Extractor"

i32 main()
{
    /* Set up console window title */
#ifdef _WIN32 //Windows
    SetConsoleTitle(WINDOWNAME);
#endif

    MPQHandler mpqHandler;
    if (mpqHandler.Load())
    {
        std::filesystem::path basePath = std::filesystem::current_path();
        std::filesystem::path outputPath(basePath.string() + "/NovusExtractor");
        if (!std::filesystem::exists(outputPath))
        {
            std::filesystem::create_directory(outputPath);
        }

        std::string sqlOutput = "";

        std::vector<std::string> adtLocations;
        if (DBCLoader::LoadMap(mpqHandler, sqlOutput, adtLocations))
        {
            MapLoader::LoadMaps(mpqHandler, adtLocations);
        }

        NC_LOG_MESSAGE("Building sql...");
        std::ofstream output(outputPath.string() + "/DBCImportData.sql", std::ofstream::out);
        output << sqlOutput;
        output.close();

        mpqHandler.CloseAll();
        NC_LOG_SUCCESS("Finished extracting all data");
    }
    else
    {
        NC_LOG_ERROR("Failed to load any MPQs");
    }

    system("pause");
    return 0;
}
