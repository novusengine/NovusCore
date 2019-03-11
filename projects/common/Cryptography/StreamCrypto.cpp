/*
* Copyright (C) 2008-2018 TrinityCore <https://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "StreamCrypto.h"

#include "HMAC.h"
#include "BigNumber.h"

StreamCrypto::StreamCrypto() : _cDecrypt(20), _sEncrypt(20), _valid(false) { }

void StreamCrypto::SetupClient(BigNumber* key)
{
    u8 cEncryptionKey[16] = { 0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE, 0x2F, 0x43, 0x67, 0xCE };
    HMACH cEncryptionHMAC(16, (u8*)cEncryptionKey);
    u8* eHash = cEncryptionHMAC.CalculateHash(key);

    u8 sDecryptionKey[16] = { 0xCC, 0x98, 0xAE, 0x04, 0xE8, 0x97, 0xEA, 0xCA, 0x12, 0xDD, 0xC0, 0x93, 0x42, 0x91, 0x53, 0x57 };
    HMACH sDecryptHMAC(16, (u8*)sDecryptionKey);
    u8* dHash = sDecryptHMAC.CalculateHash(key);

    _sEncrypt.Setup(eHash);
    _cDecrypt.Setup(dHash);

    // Drop first 1024 bytes, as WoW uses ARC4-drop1024.
    u8 dropBuffer[1024];

    memset(dropBuffer, 0, 1024);
    _sEncrypt.UpdateEncryption(1024, dropBuffer);

    memset(dropBuffer, 0, 1024);
    _cDecrypt.UpdateEncryption(1024, dropBuffer);

    _valid = true;
}
void StreamCrypto::SetupServer(BigNumber* key)
{
    u8 sEncryptionKey[16] = { 0xCC, 0x98, 0xAE, 0x04, 0xE8, 0x97, 0xEA, 0xCA, 0x12, 0xDD, 0xC0, 0x93, 0x42, 0x91, 0x53, 0x57 };
    HMACH sEncryptionHMAC(16, (u8*)sEncryptionKey);
    u8* eHash = sEncryptionHMAC.CalculateHash(key);

    u8 cDecryptionKey[16] = { 0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE, 0x2F, 0x43, 0x67, 0xCE };
    HMACH cDecryptHMAC(16, (u8*)cDecryptionKey);
    u8* dHash = cDecryptHMAC.CalculateHash(key);

    _sEncrypt.Setup(eHash);
    _cDecrypt.Setup(dHash);

    // Drop first 1024 bytes, as WoW uses ARC4-drop1024.
    u8 dropBuffer[1024];

    memset(dropBuffer, 0, 1024);
    _sEncrypt.UpdateEncryption(1024, dropBuffer);

    memset(dropBuffer, 0, 1024);
    _cDecrypt.UpdateEncryption(1024, dropBuffer);

    _valid = true;
}

void StreamCrypto::Decrypt(u8* data, size_t size)
{
    if (!_valid)
        return;

    _cDecrypt.UpdateEncryption(size, data);
}

void StreamCrypto::Encrypt(u8* data, size_t size)
{
    if (!_valid)
        return;

    _sEncrypt.UpdateEncryption(size, data);
}
