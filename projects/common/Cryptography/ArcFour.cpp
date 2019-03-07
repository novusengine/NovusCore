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

#include "ArcFour.h"

ArcFour::ArcFour(size_t size) : _CIPHER_CONTEXT(EVP_CIPHER_CTX_new())
{
    EVP_CIPHER_CTX_init(_CIPHER_CONTEXT);
    EVP_EncryptInit_ex(_CIPHER_CONTEXT, EVP_rc4(), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_set_key_length(_CIPHER_CONTEXT, size);
}

ArcFour::ArcFour(uint8_t* seed, size_t size) : _CIPHER_CONTEXT(EVP_CIPHER_CTX_new())
{
    EVP_CIPHER_CTX_init(_CIPHER_CONTEXT);

    EVP_EncryptInit_ex(_CIPHER_CONTEXT, EVP_rc4(), nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_set_key_length(_CIPHER_CONTEXT, size);
    EVP_EncryptInit_ex(_CIPHER_CONTEXT, nullptr, nullptr, seed, nullptr);
}

ArcFour::~ArcFour()
{
    EVP_CIPHER_CTX_free(_CIPHER_CONTEXT);
}

void ArcFour::Setup(uint8_t* seed)
{
    EVP_EncryptInit_ex(_CIPHER_CONTEXT, nullptr, nullptr, seed, nullptr);
}

void ArcFour::UpdateEncryption(size_t size, uint8_t* data)
{
    int out = 0;
    EVP_EncryptUpdate(_CIPHER_CONTEXT, data, &out, data, size);
    EVP_EncryptFinal_ex(_CIPHER_CONTEXT, data, &out);
}
