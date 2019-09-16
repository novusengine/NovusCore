#pragma once
#include <Utils/ConcurrentQueue.h>
#include <Utils/Message.h>
#include <cassert>

class ServiceLocator
{
public:
    static moodycamel::ConcurrentQueue<Message>* GetMainInputQueue() 
    {
        assert(_mainInputQueue != nullptr);
        return _mainInputQueue; 
    }
    static void SetMainInputQueue(moodycamel::ConcurrentQueue<Message>* mainInputQueue)
    {
        assert(_mainInputQueue == nullptr);
        _mainInputQueue = mainInputQueue;
    }

private:
    ServiceLocator() { }
    static moodycamel::ConcurrentQueue<Message>* _mainInputQueue;
};