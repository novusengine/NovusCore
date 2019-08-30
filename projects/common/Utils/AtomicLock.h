#pragma once
#include <atomic>
#include <cassert>

#ifndef NDEBUG
template <typename T>
struct TypeLock
{
    std::atomic<i32> lock;
};
template <typename T>
std::atomic<i32>& GetWriters()
{
    static TypeLock<T> typelock;
    return typelock.lock;
}
template <typename T>
std::atomic<i32>& GetReaders()
{
    static TypeLock<T> typelock;
    return typelock.lock;
}

template <typename T>
struct WriteLock
{
    WriteLock()
    {
        auto writercount = GetWriters<T>().fetch_add(1);
        auto readercount = GetReaders<T>().load();

        assert(writercount == 0);
        assert(readercount == 0);
    }
    ~WriteLock()
    {
        GetWriters<T>()--;
    }
};

template <typename T>
struct ReadLock
{
    ReadLock()
    {
        auto writercount = GetWriters<T>().load();
        GetReaders<T>()++;

        assert(writercount == 0);
    }
    ~ReadLock()
    {
        GetReaders<T>()--;
    }
};

#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#define LockWrite(comp) auto TOKENPASTE2(wmutex, __LINE__) = WriteLock<comp>{};
#define LockRead(comp) auto TOKENPASTE2(rmutex, __LINE__) = ReadLock<comp>{};

#else
#define LockWrite(comp) ((void)0)
#define LockRead(comp) ((void)0)
#endif