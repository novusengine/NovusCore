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

#ifdef _WIN32
#include <Windows.h>
#endif

//The name of the console window.
#define WINDOWNAME "Data Extractor"

i32 main()
{
	/* Set up console window title */
#ifdef _WIN32  //Windows
	SetConsoleTitle(WINDOWNAME);
#endif

	MPQHandler mpqHandler;
	if (mpqHandler.Load())
	{
		std::string sqlOutput = "";
		DBCLoader::LoadMap(mpqHandler, sqlOutput);

		std::ofstream output(std::filesystem::current_path().string() + "/dbcdata.sql", std::ofstream::out);
		output << sqlOutput;
		output.close();
	}
	else
	{
		NC_LOG_ERROR("Failed to load any MPQs");
	}

	std::getchar();
	return 0;
}
