#pragma once
#include "../Networking/ByteBuffer.h"
#include <iostream>
#include <fstream>

class FileReader
{
public:
    FileReader(std::string path, std::string fileName) : _path(path), _fileName(fileName), _length(0) { }

    bool Open()
    {
        _fileStream.open(_path);
        if (!_fileStream)
            return false;

        // Find length of file
        _fileStream.seekg(0, _fileStream.end);
        _length = size_t(_fileStream.tellg());

        // Restore read pointer back to the start of the file
        _fileStream.seekg(0, _fileStream.beg);

        return true;
    }

    void Read(Common::ByteBuffer& buffer, size_t length)
    {
        // Soft check to ensure we don't try to read from empty file
        if (_length == 0)
            return;

        _fileStream.read((char*)buffer.data(), length);
    }

    std::string Path() { return _path; }
    std::string FileName() { return _fileName; }
    size_t Length() { return _length; }
private:
    std::ifstream _fileStream;
    std::string _path;
    std::string _fileName;
    size_t _length;
};