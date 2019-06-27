/*
    MIT License

    Copyright (c) 2018-2019 NovusCore

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>
#include <vector>

#include "../../NovusEnums.h"

struct PositionUpdateData
{
    u16 opcode;
    u32 movementFlags;
    u16 movementFlagsExtra;
    u32 gameTime;
    Vector3 position;
    f32 orientation;
    u32 fallTime;
};

struct ChatUpdateData
{
    u8 chatType;
    i32 language;
    u64 sender;
    std::string message;
    bool handled;
};

struct PlayerUpdateDataComponent
{
    PlayerUpdateDataComponent() { }

    std::vector<u64> visibleGuids;
    std::vector<PositionUpdateData> positionUpdateData;
    std::vector<ChatUpdateData> chatUpdateData;
};
