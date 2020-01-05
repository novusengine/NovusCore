#include "ClientHandler.h"
#include <Networking/MessageHandler.h>
#include <Networking/InputQueue.h>
#include "Window.h"
#include "Camera.h"

#include "Utils/ServiceLocator.h"

#include <Renderer.h>

ClientHandler::ClientHandler()
    : _isRunning(false), _inputQueue(256), _outputQueue(256)
{
}

ClientHandler::~ClientHandler()
{
    delete _renderer;
}

void ClientHandler::Start()
{
    if (_isRunning)
        return;

    ServiceLocator::SetMainInputQueue(&_inputQueue);

    // Setup Network Lib
    InputQueue::SetInputQueue(&_inputQueue);
    MessageHandler::Create();

    std::thread thread = std::thread(&ClientHandler::Run, this);
    thread.detach();
}

void ClientHandler::Stop()
{
    if (!_isRunning)
        return;

    Message message;
    message.code = MSG_IN_EXIT;
    PassMessage(message);
}

void ClientHandler::PassMessage(Message& message)
{
    _inputQueue.enqueue(message);
}

bool ClientHandler::TryGetMessage(Message& message)
{
    return _outputQueue.try_dequeue(message);
}

void ClientHandler::Run()
{
    _isRunning = true;

    Message setupCompleteMessage;
    setupCompleteMessage.code = MSG_OUT_SETUP_COMPLETE;
    _outputQueue.enqueue(setupCompleteMessage);

    Timer timer;
    f32 targetDelta = 1.0f / 30.0f;

    _window = new Window();
    _window->Init(1920, 1080);

    _camera = new Camera(Vector3(0, 0, -5));
    _camera->SetInputManager(_window->GetInputManager());

    _renderer = new Renderer();
    _renderer->Init(_window->GetWindow());

    while (true)
    {
        f32 deltaTime = timer.GetDeltaTime();
        timer.Tick();

        if (!Update(deltaTime))
            break;

        Render();

        // Wait for tick rate, this might be an overkill implementation but it has the even tickrate I've seen - MPursche
        for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta - 0.0025f; deltaTime = timer.GetDeltaTime())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta; deltaTime = timer.GetDeltaTime())
        {
            std::this_thread::yield();
        }
    }

    // Clean up stuff here

    Message exitMessage;
    exitMessage.code = MSG_OUT_EXIT_CONFIRM;
    _outputQueue.enqueue(exitMessage);
}

bool ClientHandler::Update(f32 deltaTime)
{
    bool shouldExit = _window->Update(deltaTime) == false;
    if (shouldExit)
        return false;

    Message message;
    while (_inputQueue.try_dequeue(message))
    {
        if (message.code == -1)
            assert(false);

        if (message.code == MSG_IN_EXIT)
        {
            return false;
        }
        else if (message.code == MSG_IN_PRINT)
        {
            _outputQueue.enqueue(message);
        }
        else if (message.code == MSG_IN_PING)
        {
            Message pongMessage;
            pongMessage.code = MSG_OUT_PRINT;
            pongMessage.message = new std::string("PONG!");
            _outputQueue.enqueue(pongMessage);
        }
    }

    _camera->Update(deltaTime);
    _renderer->SetViewMatrix(_camera->GetViewMatrix().Inverted());

    _renderer->RegisterRenderableCube(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0.1f, 0.1f, 0.1f));

    return true;
}

void ClientHandler::Render()
{
    _renderer->Render();
    _renderer->Present();
    _window->Present();
}
