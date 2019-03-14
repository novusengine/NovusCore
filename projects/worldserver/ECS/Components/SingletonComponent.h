#pragma once
#include <NovusTypes.h>

class NovusConnection;
class WorldServerHandler;
struct SingletonComponent 
{
	f32 deltaTime;
	NovusConnection* connection;
	WorldServerHandler* worldServerHandler;
};