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

#include "HMAC.h"
#include "BigNumber.h"

#include <cstring>
#include <assert.h>

HMAC_CTX* HMAC_CTX_INIT()
{
    HMAC_CTX* context = new HMAC_CTX();
    HMAC_CTX_init(context);
    return context;
}

void HMAC_CONTEXT_CLEANUP(HMAC_CTX* context)
{
    HMAC_CTX_cleanup(context);
    delete context;
}

HMACH::HMACH(size_t size, u8* seed)
{
    _HMAC_CONTEXT = HMAC_CTX_INIT();
    HMAC_Init_ex(_HMAC_CONTEXT, seed, (int)size, EVP_sha1(), nullptr);
    memset(_data, 0, sizeof(_data));
}

HMACH::~HMACH()
{
    HMAC_CONTEXT_CLEANUP(_HMAC_CONTEXT);
}

void HMACH::UpdateHash(std::string const& string)
{
    HMAC_Update(_HMAC_CONTEXT, reinterpret_cast<u8 const*>(string.c_str()), string.length());
}

void HMACH::UpdateHash(u8 const* data, size_t size)
{
    HMAC_Update(_HMAC_CONTEXT, data, size);
}

void HMACH::Finish()
{
    u32 size = 0;
    HMAC_Final(_HMAC_CONTEXT, _data, &size);

    assert(size == SHA_DIGEST_LENGTH);
}

u8* HMACH::CalculateHash(BigNumber* bigNumber)
{
    HMAC_Update(_HMAC_CONTEXT, bigNumber->BN2BinArray().get(), bigNumber->GetBytes());
    
    Finish();
    return _data;
}
