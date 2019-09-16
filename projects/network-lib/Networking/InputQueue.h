#pragma once
#include <Utils/ConcurrentQueue.h>
#include <Utils/Message.h>

class InputQueue
{
public:
    static void SetInputQueue(moodycamel::ConcurrentQueue<Message>* inputQueue)
    {
        assert(_inputQueue == nullptr);
        _inputQueue = inputQueue;
    }
    static void PassMessage(Message& message)
    {
        assert(_inputQueue != nullptr);
        _inputQueue->enqueue(message);
    }

private:
    InputQueue() { }
    static moodycamel::ConcurrentQueue<Message>* _inputQueue;
};