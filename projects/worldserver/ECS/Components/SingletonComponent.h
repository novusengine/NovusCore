#pragma once
#include <NovusTypes.h>

class NovusConnection;
class WorldServerHandler;
struct SingletonComponent 
{
	f32 deltaTime;
	f32 lifeTime;
	NovusConnection* connection;
	WorldServerHandler* worldServerHandler;
};