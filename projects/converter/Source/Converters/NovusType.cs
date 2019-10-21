using System;
using System.IO;

abstract class NovusType
{
    protected abstract int GetNovusType();
    protected abstract int GetVersion();

    protected void SerializeNovusHeader(BinaryWriter binaryWriter)
    {
        binaryWriter.Write(GetNovusType());
        binaryWriter.Write(GetVersion());
    }
}