/*
    MIT License

    Copyright (c) 2018-2019 NovusCore

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>
#include "Utils/ConcurrentQueue.h"
#include "Message.h"
#include <entt.hpp>
#include <taskflow/taskflow.hpp>

enum InputMessages
{
	MSG_IN_EXIT,
	MSG_IN_PING,
    MSG_IN_SET_CONNECTION,
    MSG_IN_FOWARD_PACKET
};

enum OutputMessages
{
	MSG_OUT_EXIT_CONFIRM,
	MSG_OUT_PRINT
};

namespace tf
{
	class Framework;
}

struct FrameworkRegistryPair
{
    entt::registry registry;
    tf::Framework framework;
    tf::Taskflow taskflow;
};

class NovusConnection;
class WorldServerHandler
{
public:
	WorldServerHandler(f32 targetTickRate);
	~WorldServerHandler();

	void Start();
	void Stop();

	void PassMessage(Message& message);
	bool TryGetMessage(Message& message);

	void SetNovusConnection(NovusConnection* novusConnection);

    template <typename... Args>
    void PrintMessage(std::string message, Args... args)
    {
        char str[256];
        i32 length = sprintf_s(str, message.c_str(), args...);
        assert(length > -1);

        Message printMessage;
        printMessage.code = MSG_OUT_PRINT;
        printMessage.message = new std::string(str);
        _outputQueue.enqueue(printMessage);
    }
private:
	void Run();
	bool Update();
	void UpdateSystems();

	void SetupUpdateFramework();

private:
	bool _isRunning;
    f32 _targetTickRate;

    NovusConnection* _novusConnection;
	ConcurrentQueue<Message> _inputQueue;
	ConcurrentQueue<Message> _outputQueue;
	FrameworkRegistryPair _updateFramework;
};