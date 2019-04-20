#include "DBCReader.h"

DBCReader* DBCReader::_reader;

/*
	1 = Can't open file
	2 = Invalid format
	3 = Invalid data / string size read
*/

int DBCReader::Load(Common::ByteBuffer& buffer)
{
    u32 header = 0;
    buffer.Read<u32>(header);

    // Check for WDBC header
    if (header != 'CBDW')
        return 2;

    try
    {
        buffer.Read<u32>(_rowCount);
        buffer.Read<u32>(_fieldCount);
        buffer.Read<u32>(_rowSize);
        buffer.Read<u32>(_stringSize);
    }
    catch (std::exception)
    {
        return 2;
    }

    u32 dataSize = _rowSize * _rowCount + _stringSize;
    _data = new unsigned char[dataSize];
    std::memset(_data, 0, dataSize);
    _stringTable = _data + _rowSize * _rowCount;

    if (!_data || !_stringTable)
        return 3;

    buffer.Read(_data, dataSize);
    return 0;
}

DBCReader::DBCRow DBCReader::GetRow(u32 id)
{
    return DBCRow(*this, _data + id * _rowSize);
}