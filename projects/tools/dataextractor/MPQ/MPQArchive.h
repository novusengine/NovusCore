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
#include <libmpq/mpq.h>
#include <filesystem>
#include <vector>
#include <cstring>

class MPQArchive
{
public:
    MPQArchive() {}
    MPQArchive(std::string path, std::string archiveName) : name(archiveName), _path(path) {}

    bool Open()
    {
        i32 result = libmpq__archive_open(&_archive, _path.c_str(), 0);
        if (result)
        {
            switch (result)
            {
            case LIBMPQ_ERROR_OPEN:
                NC_LOG_ERROR("Archive %s: Does not exist", _path.c_str());
                break;
            case LIBMPQ_ERROR_SEEK:
                NC_LOG_ERROR("Archive %s: Failed seek", _path.c_str());
                break;
            case LIBMPQ_ERROR_READ:
                NC_LOG_ERROR("Archive %s: Failed read", _path.c_str());
                break;
            case LIBMPQ_ERROR_MALLOC:
                NC_LOG_ERROR("Archive %s: Insufficient memory", _path.c_str());
                break;
            case LIBMPQ_ERROR_FORMAT:
                NC_LOG_ERROR("Archive %s: Incorrect file format", _path.c_str());
                break;
            default:
                NC_LOG_ERROR("Archive %s. Error code unhandled", _path.c_str());
                break;
            }

            return false;
        }

        NC_LOG_SUCCESS("Archive Found: %s", _path.c_str());
        return true;
    }
    void Close()
    {
        if (_archive)
        {
            libmpq__archive_close(_archive);
        }
    }

    bool GetFiles(std::vector<std::string>& output)
    {
        u32 listFileNumber;
        if (!GetListFile(listFileNumber))
        {
            return false;
        }

        i64 listSize = 0, transferred = 0;
        libmpq__file_size_unpacked(_archive, listFileNumber, &listSize);

        u8* buffer = new u8[listSize];
        libmpq__file_read(_archive, listFileNumber, buffer, listSize, &transferred);

        {
#ifdef _WIN32
            auto const& strtok_impl = strtok_s;
#else
            auto const& strtok_impl = strtok_r;
#endif

            char seperators[] = "\r\n";
            char* nextToken = nullptr;
            char* token = strtok_impl((char*)buffer, seperators, &nextToken);

            u32 counter = 0;
            while (token && (counter < listSize))
            {
                output.push_back(token);
                counter += static_cast<u32>(strlen(token)) + 2;
                token = strtok_impl(nullptr, seperators, &nextToken);
            }
        }

        return true;
    }
    bool GetListFile(u32& output)
    {
        return GetFileNumber("(listfile)", output);
    }
    bool GetFileNumber(std::string name, u32& output)
    {
        if (libmpq__file_number(_archive, name.c_str(), &output))
            return false;

        return true;
    }
    bool GetFileSize_Unpacked(u32 fileNumber, i64& output)
    {
        if (libmpq__file_size_unpacked(_archive, fileNumber, &output))
            return false;

        return true;
    }
    bool ReadFile(u32 fileNumber, u8* buffer, i64 size)
    {
        i64 transferred = 0;
        if (libmpq__file_read(_archive, fileNumber, buffer, size, &transferred))
            return false;

        return true;
    }

    std::string name;
private:
    mpq_archive_s* _archive = nullptr;
    std::string _path = "";
};
