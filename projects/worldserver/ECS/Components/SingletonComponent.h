#pragma once
#include <NovusTypes.h> 
#include <unordered_map>

class NovusConnection;
class WorldServerHandler;
struct SingletonComponent 
{
	f32 deltaTime;
	f32 lifeTime;
	f32 lifeTimeInMS;
	NovusConnection* connection;
	WorldServerHandler* worldServerHandler;
    std::unordered_map<u32, u32> accountToEntityMap;
};