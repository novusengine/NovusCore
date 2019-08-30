#pragma once
#include "HMAC.h"
#include "ArcFour.h"
#include "BigNumber.h"
#include <cstring>

#define STREAMENCRYPTION_KEY_SIZE 16
#define STREAMENCRYPTION_HASH_SIZE 20
#define STREAMENCRYPTION_ARC4_DROP_SIZE 1024
class StreamEncryption
{
public:
    StreamEncryption() : _serverEncryptor(STREAMENCRYPTION_HASH_SIZE), _clientDecryptor(STREAMENCRYPTION_HASH_SIZE) {}
    ~StreamEncryption()
    {
        if (_hmac)
        {
            delete _hmac;
        }
    }

    void Setup(BigNumber* key)
    {
        // Generate Encryptor / Decryptor hashes by providing a constant key for both and the SRP6-a session key for the client.
        {
            _hmac = new HMACSHA1(STREAMENCRYPTION_KEY_SIZE, const_cast<u8*>(_serverKey));
            _serverEncryptor.Setup(_hmac->CalculateHash(key));

            _hmac = new HMACSHA1(STREAMENCRYPTION_KEY_SIZE, const_cast<u8*>(_clientKey));
            _clientDecryptor.Setup(_hmac->CalculateHash(key));
        }

        // World of Warcraft makes use of ARC4-Drop1024, thus we need to remove the first 1024 bytes
        {
            u8 dropBytes[STREAMENCRYPTION_ARC4_DROP_SIZE];

            std::memset(dropBytes, 0, STREAMENCRYPTION_ARC4_DROP_SIZE);
            _serverEncryptor.UpdateEncryption(STREAMENCRYPTION_ARC4_DROP_SIZE, dropBytes);

            std::memset(dropBytes, 0, STREAMENCRYPTION_ARC4_DROP_SIZE);
            _clientDecryptor.UpdateEncryption(STREAMENCRYPTION_ARC4_DROP_SIZE, dropBytes);
        }

        _setup = true;
    }

    void Encrypt(u8* buffer, size_t size)
    {
        if (_setup)
        {
            _serverEncryptor.UpdateEncryption(size, buffer);
        }
    }
    void Decrypt(u8* buffer, size_t size)
    {
        if (_setup)
        {
            _clientDecryptor.UpdateEncryption(size, buffer);
        }
    }

private:
    bool _setup = false;
    HMACSHA1* _hmac = nullptr;
    ArcFour _serverEncryptor, _clientDecryptor;

    const u8 _serverKey[STREAMENCRYPTION_KEY_SIZE] = {0xCC, 0x98, 0xAE, 0x04, 0xE8, 0x97, 0xEA, 0xCA, 0x12, 0xDD, 0xC0, 0x93, 0x42, 0x91, 0x53, 0x57};
    const u8 _clientKey[STREAMENCRYPTION_KEY_SIZE] = {0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE, 0x2F, 0x43, 0x67, 0xCE};
};