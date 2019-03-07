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

#include "SHA1.h"
#include <cstring>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "BigNumber.h"

SHA1Hasher::SHA1Hasher()
{
    SHA1_Init(&_state);
    memset(_data, 0, SHA_DIGEST_LENGTH * sizeof(uint8_t));
}
SHA1Hasher::~SHA1Hasher()
{
    SHA1_Init(&_state);
}

void SHA1Hasher::Init()
{
    SHA1_Init(&_state);
}
void SHA1Hasher::Finish(void)
{
    SHA1_Final(_data, &_state);
}

void SHA1Hasher::UpdateHash(const uint8_t* data, size_t size)
{
    SHA1_Update(&_state, data, size);
}
void SHA1Hasher::UpdateHash(const std::string& string)
{
    UpdateHash((uint8_t const*)string.c_str(), string.length());
}
void SHA1Hasher::UpdateHashForBn(size_t size, BigNumber* bigNumber0, ...)
{
    va_list v;
    BigNumber* tempBn;

    va_start(v, bigNumber0);
    tempBn = bigNumber0;

    for (size_t i = 0; i < size; i++)
    {
        UpdateHash(tempBn->BN2BinArray().get(), tempBn->GetBytes());
        tempBn = va_arg(v, BigNumber*);
    }
    va_end(v);
}

std::string GetSHA1FromHexStr(std::string const& HexString)
{
    unsigned char data[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)HexString.c_str(), HexString.length(), (unsigned char*)&data);

    // Assume the string has an even number of characters
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');

    auto first = std::begin(data);
    auto last = std::end(data);

    while (first != last)
    {
        ss << std::setw(2) << static_cast<int>(*first++);
    }

    return ss.str();
}