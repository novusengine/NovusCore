/*
# MIT License

# Copyright(c) 2018 NovusCore

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

#include <string>
#include <openssl/sha.h>

class BigNumber;

class SHA1Hasher
{
public:
    SHA1Hasher();
    ~SHA1Hasher();

    void UpdateHashForBn(size_t size, BigNumber* bigNumber0, ...);

    void UpdateHash(const uint8_t* data, size_t size);
    void UpdateHash(const std::string& str);

    void Init();
    void Finish();

    uint8_t* GetData(void) { return _data; }
    int GetLength(void) const { return SHA_DIGEST_LENGTH; }

private:
    SHA_CTX _state;
    uint8_t _data[SHA_DIGEST_LENGTH];
};

/// Returns SHA1 hash from hex string.
std::string GetSHA1FromHexStr(std::string const& content);