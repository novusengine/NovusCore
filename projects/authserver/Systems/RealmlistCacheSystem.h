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
#pragma once
#include <NovusTypes.h>
#include <Database/DatabaseConnector.h>
#include <Dependencies\robin-hood-hashing\robin_hood.h>
#include "../Connections/AuthConnection.h"

namespace RealmlistCacheSystem
{
void Update()
{
    std::shared_ptr<DatabaseConnector> connector;
    if (!DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, connector))
        return;

    amy::result_set realmServerListResultData;
    if (!connector->Query("SELECT id, name, address, type, flags, timezone, population FROM realms;", realmServerListResultData))
        return;

    AuthConnection::realmServerListMutex.lock();
    robin_hood::unordered_map<u8, RealmServerData>& realmServerData = AuthConnection::realmServerList;
    realmServerData.clear();

    for (auto row : realmServerListResultData)
    {
        RealmServerData realmData;
        realmData.realmName = row[1].GetString();
        realmData.realmAddress = row[2].GetString();
        realmData.type = row[3].GetU8();
        realmData.flags = row[4].GetU8();
        realmData.timeZone = row[5].GetU8();
        realmData.population = row[6].GetF32();

        realmServerData[row[0].GetU8()] = realmData;
    }
    AuthConnection::realmServerListMutex.unlock();
}
} // namespace RealmlistCacheSystem