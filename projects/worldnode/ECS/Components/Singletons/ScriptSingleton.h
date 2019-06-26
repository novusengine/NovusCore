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
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <Utils/ConcurrentQueue.h>

struct ScriptSingleton
{
    ScriptSingleton() : _systemCompleteCount(0), _queueCreateLock(), _transactionQueues() 
    {
        _transactionQueues.push_back(moodycamel::ConcurrentQueue<std::function<void()>>());
    }

    ScriptSingleton& operator=(const ScriptSingleton& o)
    {
        return *this;
    }

    void CompleteSystem()
    {
        _systemCompleteCount++;
        if (_systemCompleteCount >= _transactionQueues.size())
        {
            std::lock_guard lock(_queueCreateLock);
            _transactionQueues.push_back(moodycamel::ConcurrentQueue<std::function<void()>>());
        }
    }

    void ResetCompletedSystems()
    {
        _systemCompleteCount = 0;
    }
    
    void AddTransaction(std::function<void()> const& transaction)
    {
        _transactionQueues[_systemCompleteCount].enqueue(transaction);
    }

    void ExecuteTransactions()
    {
        for (moodycamel::ConcurrentQueue<std::function<void()>>& transactionQueue : _transactionQueues)
        {
            std::function<void()> transaction;
            while(transactionQueue.try_dequeue(transaction))
            {
                transaction();
            }
        }
    }
    
private:
    std::atomic<u32> _systemCompleteCount;
    std::mutex _queueCreateLock;
    std::vector<moodycamel::ConcurrentQueue<std::function<void()>>> _transactionQueues;
};