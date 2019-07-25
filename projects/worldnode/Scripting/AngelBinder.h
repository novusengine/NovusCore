//
//     Copyright � 2011 - Jo�o Francisco Biondo Trinca
//          a.k.a WoLfulus <wolfulus@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <assert.h>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <string>
#include <mutex>
#include <functional>
#include <type_traits>
#include <Utils/ConcurrentQueue.h>
#include <angelscript.h>
#include "Addons/scriptbuilder/scriptbuilder.h"
#include <Utils/StringUtils.h>

#if defined(_WIN32)
#include <windows.h>
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

static std::string demangle(const char* name) { return name; }

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#else
#include <unistd.h>
#include <cxxabi.h>
static std::string demangle(const char* mangled_name)
{
    std::size_t len = 0;
    int status = 0;
    std::unique_ptr<char, decltype(&std::free)> ptr(
        __cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status), &std::free);
    return ptr.get();
}
#endif

#ifdef AS_USE_NAMESPACE
#define AB_BEGIN_NAMESPACE \
    namespace AngelBinder  \
    {
#define AB_END_NAMESPACE }
#define AB_NAMESPACE_QUALIFIER AngelBinder::
#else
#define AB_BEGIN_NAMESPACE
#define AB_END_NAMESPACE
#define AB_NAMESPACE_QUALIFIER
#endif

///
/// Namespace start
///
AB_BEGIN_NAMESPACE

///
/// Macro definitions
///
#define AB_THROW true
#define AB_NOTHROW false

// #define AB_VERBOSE_OUTPUT

#if defined(AB_VERBOSE_OUTPUT)
#define AB_MESSAGE_INVOKE(i, m)    \
    if (this->onMessage() != NULL) \
        this->onMessage()(i, m);
#define AB_MESSAGE_INVOKE_STATIC(s, i, m) \
    if ((s)->onMessage() != NULL)         \
        (s)->onMessage()(i, m);
#else
#define AB_MESSAGE_INVOKE(i, m)
#define AB_MESSAGE_INVOKE_STATIC(s, i, m)
#endif

#define AB_SCRIPT_ASSERT(x, m, t, i)          \
    {                                         \
        if (!(x))                             \
        {                                     \
            if ((t))                          \
            {                                 \
                if ((i)->onMessage() != NULL) \
                    (i)->onMessage()(i, m);   \
                throw std::runtime_error(m);  \
            }                                 \
            else                              \
            {                                 \
                if ((i)->onMessage() != NULL) \
                    (i)->onMessage()(i, m);   \
            }                                 \
        }                                     \
    }

#define AB_PUSH_ARG(v, x) v.parameters().push_back(Type<x>::toString());

#define AB_FUNCTION(f) AS_NAMESPACE_QUALIFIER asFUNCTION(##f)
#define AB_FUNCTIONPR(f, p, r) AS_NAMESPACE_QUALIFIER asFUNCTIONPR(##f, ##p, ##r)
#define AB_METHOD(c, p, r, ptr) AS_NAMESPACE_QUALIFIER asSMethodPtr<sizeof(void (c::*)())>::Convert(AS_METHOD_AMBIGUITY_CAST(r(c::*) p)(ptr))

const std::unordered_map<u32, std::string> typeOverriders =
    {
        {"class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"_h, "string"},
        {"std::string"_h, "string"}};

///
/// Type name string conversion
///
template <typename T>
struct TypeString
{
    ///
    /// Retrieves the typeid of T, and then get its name.
    ///
    static std::string value()
    {
        std::string name = demangle(typeid(T).name());

        auto it = typeOverriders.find(StringUtils::fnv1a_32(name.c_str(), name.size()));
        if (it != typeOverriders.end())
            return it->second;

        return name;
    }
};

/// Empty declaration to use as pointer into the engine
class Engine;
class Module;
class Context;

///
/// Macro to help declaring built-in types.
///
#define AB_TRANSLATE_TYPE(t, n)                 \
    template <>                                 \
    struct AB_NAMESPACE_QUALIFIER TypeString<t> \
    {                                           \
        static std::string value()              \
        {                                       \
            return n;                           \
        }                                       \
    };

///
/// Macro to help declaring parameter setters.
///
#define AB_PARAMETER_SETTER(t, f)                  \
    template <>                                    \
    struct ParameterSetter<t>                      \
    {                                              \
        void operator()(Context* context, t value) \
        {                                          \
            context->f(value);                     \
        }                                          \
    };

///
/// Macro to help declaring return value readers.
///
#define AB_RETURN_READER(t, f)         \
    template <>                        \
    struct ReturnReader<t>             \
    {                                  \
        t operator()(Context* context) \
        {                              \
            return (t)context->f();    \
        }                              \
    };

///
/// Built-in type definition
///
AB_TRANSLATE_TYPE(char, "int8")
AB_TRANSLATE_TYPE(unsigned char, "uint8")
AB_TRANSLATE_TYPE(short, "int16")
AB_TRANSLATE_TYPE(unsigned short, "uint16")
AB_TRANSLATE_TYPE(int, "int")
AB_TRANSLATE_TYPE(unsigned int, "uint")
AB_TRANSLATE_TYPE(long long, "int64")
AB_TRANSLATE_TYPE(unsigned long long, "uint64")
AB_TRANSLATE_TYPE(float, "float")
AB_TRANSLATE_TYPE(double, "double")
AB_TRANSLATE_TYPE(bool, "bool")

///
/// Base type class used to convert template into strings.
///
template <typename T>
struct Type
{
    static std::string toString()
    {
        return TypeString<T>::value();
    }
};

///
/// Pointers
///
template <typename T>
struct Type<T*>
{
    static std::string toString()
    {
        return TypeString<T>::value() + "@";
    }
};

///
/// Constant types
///
template <typename T>
struct Type<const T>
{
    static std::string toString()
    {
        return "const " + TypeString<T>::value();
    }
};

///
/// Constant pointer types
///
template <typename T>
struct Type<const T*>
{
    static std::string toString()
    {
        return "const " + TypeString<T>::value() + "@";
    }
};

///
/// Reference types
///
template <typename T>
struct Type<T&>
{
    static std::string toString()
    {
        return TypeString<T>::value() + "& in";
    }
};

///
/// Constant reference types
///
template <typename T>
struct Type<const T&>
{
    static std::string toString()
    {
        return TypeString<T>::value() + "& in";
    }
};

///
/// AngelScript types
///
typedef AS_NAMESPACE_QUALIFIER asIScriptEngine ASEngine;
typedef AS_NAMESPACE_QUALIFIER asIScriptContext ASContext;
typedef AS_NAMESPACE_QUALIFIER asIScriptModule ASModule;
typedef AS_NAMESPACE_QUALIFIER CScriptBuilder ASBuilder;

///
/// Thread locker
///
class ThreadLocker
{
private:
    /// Objects
#if defined(_WIN32)
    CRITICAL_SECTION _section;
#else
    std::mutex _mutex;
#endif

public:
    ///
    /// Constructors / destructors
    ///
    ThreadLocker();
    ~ThreadLocker();

    ///
    /// Procedures
    ///
    void lock();
    void unlock();
};

///
/// Automatic thread locker
///
class ScopedLocker
{
private:
    ///
    /// Reference to the thread locker
    ///
    ThreadLocker& _locker;

public:
    ///
    /// Locks the thread locker
    ///
    ScopedLocker(ThreadLocker& locker);

    ///
    /// Unlocks the thread locker
    ///
    ~ScopedLocker();
};

///
/// Context pool class
///
class ContextPool
{
private:
    /// Stores the engine instance
    Engine* _engine;

    /// Stores the list of pre-allocated contexts
    std::vector<Context*> _contexts;

    /// Stores the queue of available contexts
    moodycamel::ConcurrentQueue<int> _available;

    /// Stores the current count of items on the queue
    unsigned int _availableCount;

    /// Stores if the class is being released
    bool _releasing;

    /// For multithreading synchronization
    ThreadLocker _locker;

protected:
    ///
    /// Constructors / destructors
    ///
    ContextPool(Engine* engine);
    ~ContextPool();

    ///
    /// Engine instance
    ///
    Engine* engine();

    ///
    /// Returns a context to the queue
    ///
    void ret(int id);

    ///
    /// Retrieves a context from the pool
    ///
    Context* get();

private:
    ///
    /// Waits for all contexts to be available again into the pool
    ///
    void wait();

    ///
    /// Friend classes
    ///
    friend class Context;
    friend class Engine;
};

///
/// Script engine
///
class Engine
{

    template <typename F>
    friend class Function;

public:
    /// Script engine's message callback
    typedef void (*MessageCallback)(Engine*, std::string);

private:
    /// Script Engine Instance
    ASEngine* _engine;

    /// Script Engine Builder
    ASBuilder* _builder;

    /// Message callbacks
    MessageCallback _messages;

    /// Modules
    std::unordered_map<std::string, Module*> _modules;

    // Type Hashes
    std::unordered_map<u32, size_t> _typeHashes;

    /// Stores the context pool
    ContextPool _pool;

    /// Raised when script machine writes a message
    static void onScriptMessage(const AS_NAMESPACE_QUALIFIER asSMessageInfo* msg, void* param);

    ///
    /// Startup stuff
    ///
    void initialize();
    void uninitialize();

public:
    ///
    /// Constructors / destructors
    ///
    Engine();
    Engine(MessageCallback callback);
    ~Engine();

    ///
    /// Message callbacks
    ///
    MessageCallback& onMessage();

    ///
    /// AngelScript stuff
    ///
    ASEngine* asEngine();
    ASBuilder* asBuilder();

    ///
    /// Creates a new module, or return an existent one.
    ///
    Module* createModule(std::string name);

    ///
    /// Creates a new module, or return an existent one.
    ///
    Module* getModule(std::string name);

    ///
    /// Sleeps the current thread for a while
    ///
    static void sleep(int ms);

    //protected:
    ///
    /// Retrieves a context from the pool
    ///
    Context* getContext();
    Context* getContext(int function);

    template <typename T>
    void RegisterObjectHash(u32 nameHash)
    {
        _typeHashes[nameHash] = typeid(T).hash_code();
    }

    size_t GetTypeHash(u32 nameHash)
    {
        assert(_typeHashes.find(nameHash) != _typeHashes.end());

        return _typeHashes[nameHash];
    }
};

///
/// Script Function
///
class Context
{
    /// Friend function class
    template <typename T>
    friend class Function;

    /// Friend pool class
    friend class ContextPool;

private:
    /// Stores the current context id (from pool)
    int _id;

    /// Count of parameters set to this context
    int _params;

    /// AngelScript's context
    ASContext* _context;

    /// Engine instance
    ContextPool& _pool;

protected:
    ///
    /// Creates a new context
    ///
    Context(ContextPool& engine, int id);

    ///
    /// Releases the context
    ///
    ~Context();

    ///
    /// Script exception callback
    ///
    void exceptionCallback(asIScriptContext* context);

public:
    ///
    /// Executes the context
    ///
    void execute();

    ///
    /// Prepares the execution of the context
    ///
    void prepare(int function);
    void prepare(asIScriptFunction* function);

    ///
    /// Release this context
    ///
    void release();

    ///
    /// Parameter setters
    ///
    void setAddress(void* value);
    void setObject(void* value);
    void setByte(asBYTE value);
    void setWord(asWORD value);
    void setDWord(unsigned int value);
    void setQWord(unsigned long long value);
    void setFloat(float value);
    void setDouble(double value);
    void setBool(bool value);

    ///
    /// Return readers
    ///
    void* readAddress();
    void* readObject();
    asBYTE readByte();
    asWORD readWord();
    asDWORD readDWord();
    asQWORD readQWord();
    float readFloat();
    double readDouble();
    bool readBool();
};

///
/// Parameter setters
///
template <typename T>
struct ParameterSetter
{
    void operator()(Context* context, T& value)
    {
        context->setObject((void*)&value);
    }
};

template <typename T>
struct ParameterSetter<T*>
{
    void operator()(Context* context, T* value)
    {
        ParameterSetter<T>()(context, *value);
    }
};

template <typename T>
struct ParameterSetter<const T>
{
    void operator()(Context* context, const T value)
    {
        ParameterSetter<T>()(context, (T&)(value));
    }
};

template <typename T>
struct ParameterSetter<const T*>
{
    void operator()(Context* context, const T* value)
    {
        ParameterSetter<T>()(context, (T&)(*value));
    }
};

template <typename T>
struct ParameterSetter<T&>
{
    void operator()(Context* context, T& value)
    {
        ParameterSetter<T>()(context, value);
    }
};

template <typename T>
struct ParameterSetter<const T&>
{
    void operator()(Context* context, const T& value)
    {
        ParameterSetter<T>()(context, value);
    }
};

///
/// Special parameter types.
///
AB_PARAMETER_SETTER(char, setByte);
AB_PARAMETER_SETTER(unsigned char, setByte);
AB_PARAMETER_SETTER(short, setWord);
AB_PARAMETER_SETTER(unsigned short, setWord);
AB_PARAMETER_SETTER(int, setDWord);
AB_PARAMETER_SETTER(unsigned int, setDWord);
AB_PARAMETER_SETTER(long long, setQWord);
AB_PARAMETER_SETTER(unsigned long long, setQWord);
AB_PARAMETER_SETTER(float, setFloat);
AB_PARAMETER_SETTER(double, setDouble);
AB_PARAMETER_SETTER(bool, setBool);

///
/// Parameter setters
///
template <typename T>
struct ReturnReader
{
    T operator()(Context* context)
    {
        return *static_cast<T*>(context->readObject());
    }
};

template <typename T>
struct ReturnReader<T*>
{
    T* operator()(Context* context)
    {
        return static_cast<T*>(context->readObject());
    }
};

template <typename T>
struct ReturnReader<const T>
{
    const T operator()(Context* context)
    {
        return ReturnReader<T>()(context);
    }
};

template <typename T>
struct ReturnReader<const T*>
{
    const T* operator()(Context* context)
    {
        return ReturnReader<T*>()(context);
    }
};

template <typename T>
struct ReturnReader<T&>
{
    T& operator()(Context* context)
    {
        return ReturnReader<T>()(context);
    }
};

template <typename T>
struct ReturnReader<const T&>
{
    const T& operator()(Context* context)
    {
        return ReturnReader<T>()(context);
    }
};

///
/// Special return types.
///
AB_RETURN_READER(char, readByte);
AB_RETURN_READER(unsigned char, readByte);
AB_RETURN_READER(short, readWord);
AB_RETURN_READER(unsigned short, readWord);
AB_RETURN_READER(int, readDWord);
AB_RETURN_READER(unsigned int, readDWord);
AB_RETURN_READER(long long, readQWord);
AB_RETURN_READER(unsigned long long, readQWord);
AB_RETURN_READER(float, readFloat);
AB_RETURN_READER(double, readDouble);
AB_RETURN_READER(bool, readBool);

#define AB_FUNCTION_CHECK()                             \
    if (this->_engine == NULL || this->_function == -1) \
    throw std::runtime_error("Script function pointer not initialized.")

#define AB_FUNCTION_CONSTRUCTOR()              \
    friend class Module;                       \
                                               \
private:                                       \
    Engine* _engine;                           \
    int _function;                             \
                                               \
public:                                        \
    Function()                                 \
        : _engine(NULL), _function(-1)         \
    {                                          \
    }                                          \
                                               \
protected:                                     \
    Function(Engine* engine, int function)     \
        : _engine(engine), _function(function) \
    {                                          \
    }

///
/// Dummy function class
///
template <typename F>
class Function
{
    friend class Module;
    static std::string decompose(std::string name)
    {
        return "";
    }
};

///
/// Function definitions
///

template <>
class Function<void()>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()()
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "()";
        return stream.str();
    }
};

template <typename R>
class Function<R()>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()()
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);

        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "()";
        return stream.str();
    }
};

template <typename A1>
class Function<void(A1)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1>
class Function<R(A1)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2>
class Function<void(A1, A2)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2>
class Function<R(A1, A2)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3>
class Function<void(A1, A2, A3)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3>
class Function<R(A1, A2, A3)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4>
class Function<void(A1, A2, A3, A4)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4>
class Function<R(A1, A2, A3, A4)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5>
class Function<void(A1, A2, A3, A4, A5)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
class Function<R(A1, A2, A3, A4, A5)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class Function<void(A1, A2, A3, A4, A5, A6)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
class Function<R(A1, A2, A3, A4, A5, A6)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
class Function<void(A1, A2, A3, A4, A5, A6, A7)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
class Function<R(A1, A2, A3, A4, A5, A6, A7)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ParameterSetter<A12>()(ctx, a12);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ParameterSetter<A12>()(ctx, a12);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ParameterSetter<A12>()(ctx, a12);
        ParameterSetter<A13>()(ctx, a13);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ParameterSetter<A12>()(ctx, a12);
        ParameterSetter<A13>()(ctx, a13);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ")";
        return stream.str();
    }
};

template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
class Function<void(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ParameterSetter<A12>()(ctx, a12);
        ParameterSetter<A13>()(ctx, a13);
        ParameterSetter<A14>()(ctx, a14);
        ctx->execute();
        ctx->release();
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << "void " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ", " << Type<A14>::toString() << ")";
        return stream.str();
    }
};

template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
class Function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
    AB_FUNCTION_CONSTRUCTOR();

public:
    R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14)
    {
        AB_FUNCTION_CHECK();
        Context* ctx = this->_engine->getContext(this->_function);
        ParameterSetter<A1>()(ctx, a1);
        ParameterSetter<A2>()(ctx, a2);
        ParameterSetter<A3>()(ctx, a3);
        ParameterSetter<A4>()(ctx, a4);
        ParameterSetter<A5>()(ctx, a5);
        ParameterSetter<A6>()(ctx, a6);
        ParameterSetter<A7>()(ctx, a7);
        ParameterSetter<A8>()(ctx, a8);
        ParameterSetter<A9>()(ctx, a9);
        ParameterSetter<A10>()(ctx, a10);
        ParameterSetter<A11>()(ctx, a11);
        ParameterSetter<A12>()(ctx, a12);
        ParameterSetter<A13>()(ctx, a13);
        ParameterSetter<A14>()(ctx, a14);
        ctx->execute();
        R ret = ReturnReader<R>()(ctx);
        ctx->release();
        return ret;
    }
    static std::string decompose(std::string name)
    {
        std::stringstream stream;
        stream << Type<R>::toString() << " " << name << "(" << Type<A1>::toString() << ", " << Type<A2>::toString() << ", " << Type<A3>::toString() << ", " << Type<A4>::toString() << ", " << Type<A5>::toString() << ", " << Type<A6>::toString() << ", " << Type<A7>::toString() << ", " << Type<A8>::toString() << ", " << Type<A9>::toString() << ", " << Type<A10>::toString() << ", " << Type<A11>::toString() << ", " << Type<A12>::toString() << ", " << Type<A13>::toString() << ", " << Type<A14>::toString() << ")";
        return stream.str();
    }
};

///
/// Script Engine
///
class Module
{
private:
    ///
    /// Script Module Name
    ///
    std::string _name;

    ///
    /// AngelBinder engine
    ///
    Engine& _engine;

public:
    ///
    /// Constructors
    ///
    Module(Engine& engine, std::string name);
    ~Module(void);

    ///
    /// Retrieves the engine instance
    ///
    Engine& engine();

    ///
    /// Script compiler
    ///
    bool compile(std::string file);

    template <typename F>
    Function<F> getFunction(std::string name)
    {
        int funcid = this->getFunctionByDecl(Function<F>::decompose(name));
        AB_SCRIPT_ASSERT(funcid >= 0, std::string("Function '" + Function<F>::decompose(name) + "' could not be found.").c_str(), AB_THROW, &this->_engine);
        Function<F> func(&this->_engine, funcid);
        return func;
    }

private:
    int getFunctionByName(std::string name)
    {
        ASModule* module = this->_engine.asEngine()->GetModule(this->_name.c_str());

        return module->GetFunctionByName(name.c_str())->GetId();
    }

    int getFunctionByDecl(std::string decl)
    {
        ASModule* module = this->_engine.asEngine()->GetModule(this->_name.c_str());

        return module->GetFunctionByDecl(decl.c_str())->GetId();
    }
};

///
/// VariableExporter parser
///
class VariableClass
{
private:
    /// Type of the variable
    std::string _type;

    /// Name of the variable
    std::string _name;

    /// Address of the variable
    void* _address;

public:
    ///
    /// Constructs a variable class
    ///
    VariableClass(std::string type, std::string name, void* address);

    ///
    /// Returns the name of this variable
    ///
    std::string name();

    ///
    /// Returns the type of this variable
    ///
    std::string type();

    ///
    /// Decomposes the exported variable into an equivalent string
    ///
    std::string decompose();

    ///
    /// Returns the address of the variable
    ///
    void* address();
};

///
/// Base exporter
///
class BaseExporter
{
public:
    virtual void finish(Engine& engine) = 0;
};

///
/// Global variable exporter
///
class VariableExporter : public BaseExporter
{

    /// Friend exporter class
    friend class Exporter;

protected:
    /// Stores all exported entries
    std::queue<VariableClass> _entries;

protected:
    ///
    /// Variable exporter
    ///
    VariableExporter();

    ///
    /// Called when registering is needed.
    ///
    virtual void finish(Engine& instance);

public:
    template <typename T>
    VariableExporter& def(std::string name, T* address)
    {
        VariableClass var(Type<T>::toString(), name, address);
        this->_entries.push(var);
        return *this;
    }
};

///
/// Calling conventions
///
enum CallingConvention
{
    CallStdcall = AS_NAMESPACE_QUALIFIER asCALL_STDCALL,
    CallCdecl = AS_NAMESPACE_QUALIFIER asCALL_CDECL
    /* TODO: Add more? */
};

///
/// FunctionClass parser
///
class FunctionClass
{
private:
    /// Stores the return type string
    std::string _ret;

    /// Stores the function name
    std::string _name;

    /// Stores a vector of arguments
    std::vector<std::string> _params;

    /// Calling convention
    CallingConvention _conv;

    /// Stores the pointer to the function
    AS_NAMESPACE_QUALIFIER asSFuncPtr _func;

public:
    ///
    /// FunctionClass constructor
    ///
    FunctionClass(std::string ret, std::string name, CallingConvention conv, AS_NAMESPACE_QUALIFIER asSFuncPtr func);

    ///
    /// Returns the function name
    ///
    std::string name();

    ///
    /// Returns the parameters
    ///
    std::vector<std::string>& parameters();

    ///
    /// Returns the pointer to the function
    ///
    AS_NAMESPACE_QUALIFIER asSFuncPtr address();

    ///
    /// Decomposes the function into an equivalent string
    ///
    std::string decompose();

    ///
    /// Gets the function calling convention
    ///
    CallingConvention convention();
};

///
/// FunctionClass export container
///
class FunctionExporter : public BaseExporter
{

    /// Friend exporter class
    friend class Exporter;

protected:
    /// Stores all exported entries
    std::queue<FunctionClass> _entries;

protected:
    ///
    /// Function exporter
    ///
    FunctionExporter();

    ///
    /// Called when registering is needed.
    ///
    virtual void finish(Engine& instance);

public:
    ///
    /// Templated function exporters
    ///
    template <typename R>
    FunctionExporter& def(std::string name, R (*func)())
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R>
	FunctionExporter& def(std::string name, R(__cdecl *func)())
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1>
    FunctionExporter& def(std::string name, R (*func)(A1))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2>
    FunctionExporter& def(std::string name, R (*func)(A1, A2))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7, A8))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        AB_PUSH_ARG(fc, A8);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        AB_PUSH_ARG(fc, A8);
        AB_PUSH_ARG(fc, A9);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        AB_PUSH_ARG(fc, A8);
        AB_PUSH_ARG(fc, A9);
        AB_PUSH_ARG(fc, A10);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        AB_PUSH_ARG(fc, A8);
        AB_PUSH_ARG(fc, A9);
        AB_PUSH_ARG(fc, A10);
        AB_PUSH_ARG(fc, A11);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        AB_PUSH_ARG(fc, A8);
        AB_PUSH_ARG(fc, A9);
        AB_PUSH_ARG(fc, A10);
        AB_PUSH_ARG(fc, A11);
        AB_PUSH_ARG(fc, A12);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        AB_PUSH_ARG(fc, A8);
        AB_PUSH_ARG(fc, A9);
        AB_PUSH_ARG(fc, A10);
        AB_PUSH_ARG(fc, A11);
        AB_PUSH_ARG(fc, A12);
        AB_PUSH_ARG(fc, A13);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); 
		this->_entries.push(fc);
		return *this;
	}*/

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
    FunctionExporter& def(std::string name, R (*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
    {
        FunctionClass fc(Type<R>::toString(), name, CallCdecl, AS_NAMESPACE_QUALIFIER asFUNCTION(func));
        AB_PUSH_ARG(fc, A1);
        AB_PUSH_ARG(fc, A2);
        AB_PUSH_ARG(fc, A3);
        AB_PUSH_ARG(fc, A4);
        AB_PUSH_ARG(fc, A5);
        AB_PUSH_ARG(fc, A6);
        AB_PUSH_ARG(fc, A7);
        AB_PUSH_ARG(fc, A8);
        AB_PUSH_ARG(fc, A9);
        AB_PUSH_ARG(fc, A10);
        AB_PUSH_ARG(fc, A11);
        AB_PUSH_ARG(fc, A12);
        AB_PUSH_ARG(fc, A13);
        AB_PUSH_ARG(fc, A14);
        this->_entries.push(fc);
        return *this;
    }

    /*template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
	FunctionExporter& def(std::string name, R(__cdecl *func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
	{
		FunctionClass fc(Type<R>::toString(), name, CallCdecl, AB_FUNCTION(func));
		AB_PUSH_ARG(fc, A1); AB_PUSH_ARG(fc, A2); AB_PUSH_ARG(fc, A3); AB_PUSH_ARG(fc, A4); AB_PUSH_ARG(fc, A5); AB_PUSH_ARG(fc, A6); AB_PUSH_ARG(fc, A7); AB_PUSH_ARG(fc, A8); AB_PUSH_ARG(fc, A9); AB_PUSH_ARG(fc, A10); AB_PUSH_ARG(fc, A11); AB_PUSH_ARG(fc, A12); AB_PUSH_ARG(fc, A13); AB_PUSH_ARG(fc, A14); 
		this->_entries.push(fc);
		return *this;
	}*/
};

///
/// Dummy constructors / destructors
///
void DummyConstructor(void* memory);
void DummyDestructor(void* memory);

///
/// Class destructor wrapper
///

template <typename T>
void DestructorWrapper(void* memory)
{
    ((T*)memory)->~T();
}

///
/// Class constructor helper
///

template <typename T>
void ConstructorWrapper(void* memory)
{
    new (memory) T();
}

template <typename T, typename A1>
void ConstructorWrapper(A1 a1, void* memory)
{
    new (memory) T(a1);
}

template <typename T, typename A1, typename A2>
void ConstructorWrapper(A1 a1, A2 a2, void* memory)
{
    new (memory) T(a1, a2);
}

template <typename T, typename A1, typename A2, typename A3>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, void* memory)
{
    new (memory) T(a1, a2, a3);
}

template <typename T, typename A1, typename A2, typename A3, typename A4>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, void* memory)
{
    new (memory) T(a1, a2, a3, a4);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
}

template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
void ConstructorWrapper(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14, void* memory)
{
    new (memory) T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
}

///
/// Class constructors
///
class ConstructorClass
{
private:
    /// Stores a vector of arguments
    std::vector<std::string> _params;

    /// Stores the pointer to the function
    AS_NAMESPACE_QUALIFIER asSFuncPtr _func;

public:
    ///
    /// ConstructorClass constructor
    ///
    ConstructorClass(AS_NAMESPACE_QUALIFIER asSFuncPtr func)
        : _func(func)
    {
    }

    ///
    /// Returns the parameters
    ///
    std::vector<std::string>& parameters()
    {
        return this->_params;
    }

    ///
    /// Returns the pointer to the function
    ///
    AS_NAMESPACE_QUALIFIER asSFuncPtr address()
    {
        return this->_func;
    }

    ///
    /// Decomposes the constructor into an equivalent string
    ///
    std::string decompose()
    {
        std::stringstream stream;
        stream << "void f(";
        for (size_t i = 0; i < this->_params.size(); i++)
        {
            stream << this->_params[i];
            if (i != this->_params.size() - 1)
            {
                stream << ", ";
            }
        }
        stream << ")";
        return stream.str();
    }
};

///
/// Class methods
///
class MethodClass
{
private:
    /// Stores the return value
    std::string _ret;

    /// Stores the name of the method
    std::string _name;

    /// Stores a vector of arguments
    std::vector<std::string> _params;

    /// Stores the function reference
    AS_NAMESPACE_QUALIFIER asSFuncPtr _func;

public:
    ///
    /// MethodClass constructor
    ///
    MethodClass(std::string name, std::string ret, AS_NAMESPACE_QUALIFIER asSFuncPtr func)
        : _ret(ret), _name(name), _func(func)
    {
    }

    ///
    /// Returns the parameter list
    ///
    std::vector<std::string>& parameters()
    {
        return this->_params;
    }

    ///
    /// Returns the structure for the method
    ///
    AS_NAMESPACE_QUALIFIER asSFuncPtr address()
    {
        return this->_func;
    }

    ///
    /// Decomposes the method into an equivalent string
    ///
    std::string decompose()
    {
        std::stringstream stream;
        stream << this->_ret << " " << this->_name << "(";
        for (size_t i = 0; i < this->_params.size(); i++)
        {
            stream << this->_params[i];
            if (i != this->_params.size() - 1)
            {
                stream << ", ";
            }
        }
        stream << ")";
        return stream.str();
    }
};

///
/// Class accessors
///
class AccessorClass
{

    template <typename T>
    friend class ClassExporter;

private:
    /// Stores the name of the method
    std::string _name;

    /// Stores if the get function has been set
    bool _getfuncset;

    /// Stores the set function reference
    MethodClass _getfunc;

    /// Stores if the set function has been set
    bool _setfuncset;

    /// Stores the get function reference
    MethodClass _setfunc;

protected:
    enum AcessorType
    {
        ReadOnly = 1,
        WriteOnly = 2
    };

    ///
    /// AccessorClass constructor
    ///
    AccessorClass(std::string name, std::string type, AS_NAMESPACE_QUALIFIER asSFuncPtr getfunc, AS_NAMESPACE_QUALIFIER asSFuncPtr setfunc, bool indexed = false)
        : _name(name),
          _getfuncset(true),
          _getfunc("get_" + name, type, getfunc),
          _setfuncset(true),
          _setfunc("set_" + name, "void", setfunc)
    {
        if (indexed)
        {
            this->_getfunc.parameters().push_back("int");
            this->_setfunc.parameters().push_back("int");
        }
        this->_setfunc.parameters().push_back(type);
    }

    ///
    /// AccessorClass constructor
    ///
    AccessorClass(std::string name, std::string type, AcessorType acessortype, AS_NAMESPACE_QUALIFIER asSFuncPtr func, bool indexed = false)
        : _name(name),
          _getfuncset((acessortype & ReadOnly) == ReadOnly),
          _getfunc("get_" + name, type, func),
          _setfuncset((acessortype & WriteOnly) == WriteOnly),
          _setfunc("set_" + name, "void", func)
    {
        if (indexed)
        {
            this->_getfunc.parameters().push_back("int");
            this->_setfunc.parameters().push_back("int");
        }
        if ((acessortype & WriteOnly) == WriteOnly)
        {
            this->_setfunc.parameters().push_back(type);
        }
    }

    ///
    /// Accessor name
    ///
    std::string name()
    {
        return this->_name;
    }

    ///
    /// Returns if the acessor has a get function
    ///
    bool hasGet()
    {
        return this->_getfuncset;
    }

    ///
    /// Returns if the acessor has a set function
    ///
    bool hasSet()
    {
        return this->_setfuncset;
    }

    ///
    /// Returns the structure for the get method
    ///
    MethodClass get()
    {
        return this->_getfunc;
    }

    ///
    /// Returns the structure for the set method
    ///
    MethodClass set()
    {
        return this->_setfunc;
    }
};

///
/// Class constructors
///
class MemberClass
{
private:
    /// Stores the name of the member
    std::string _name;

    /// Stores the type of the member
    std::string _type;

    /// Stores the offset of the member
    int _offset;

public:
    ///
    /// MemberClass constructor
    ///
    MemberClass(std::string name, std::string type, int offset)
        : _name(name), _type(type), _offset(offset)
    {
    }

    ///
    /// Returns the member name
    ///
    std::string name()
    {
        return this->_name;
    }

    ///
    /// Returns the structure for the get method
    ///
    std::string type()
    {
        return this->_type;
    }

    ///
    /// Returns the structure for the set method
    ///
    int offset()
    {
        return this->_offset;
    }

    ///
    /// Decomposes the member declaration
    ///
    std::string decompose()
    {
        return this->_type + " " + this->_name;
    }
};

///
/// Class exporter
///
template <typename T>
class ClassExporter
    : public BaseExporter
{

    /// Friend exporter class
    friend class Exporter;

protected:
    /// Initial flags
    int _flags;

    /// Stores all exported constructors
    std::queue<ConstructorClass> _ctors;

    /// Stores all assignment operators
    // std::queue<AssignClass> _assigns;

    /// Stores all copy constructors
    // asSFuncPtr _copy;

    /// Stores all exported constructors
    std::queue<MethodClass> _methods;

    /// Stores all exported accessors
    std::queue<AccessorClass> _accessors;

    /// Stores all exported accessors
    std::queue<MemberClass> _members;

    /// Stores if the destructor has been set
    bool _dtorset;

    /// Stores if the destructor has been set
    bool _ctorset;

    /// Stores the destructor pointer
    AS_NAMESPACE_QUALIFIER asSFuncPtr _dtor;

    /// Stores the dummy constructor pointer
    AS_NAMESPACE_QUALIFIER asSFuncPtr _ctor;

protected:
    ///
    /// Function exporter
    ///
    ClassExporter(int flags)
        : _flags(flags), _dtorset(false), _ctorset(false)
    {
        // Couldn't find any definitions for asMemClear, so far it's been tested and no functional behavior have changed.
        //asMemClear(&this->_ctor, sizeof(this->_ctor));
        //asMemClear(&this->_dtor, sizeof(this->_dtor));
    }

    ///
    /// Called when registering is needed.
    ///
    virtual void finish(Engine& instance)
    {

        int flags = this->_flags;
        flags |= AS_NAMESPACE_QUALIFIER asOBJ_VALUE;
        flags |= AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS;
        flags |= !this->_ctors.empty() || this->_ctorset ? AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_CONSTRUCTOR : 0;
        flags |= this->_dtorset ? AS_NAMESPACE_QUALIFIER asOBJ_APP_CLASS_DESTRUCTOR : 0;
        flags |= std::is_pod<T>::value == true ? AS_NAMESPACE_QUALIFIER asOBJ_POD : 0;
        // flags |= this->_assigns.empty() ? 0 : asOBJ_APP_CLASS_ASSIGNMENT;
        // flags |= this->_copy == NULL ? 0 : asOBJ_APP_CLASS_COPY_CONSTRUCTOR;

        std::string name = Type<T>::toString();
        AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering type '" + name + "'");
        int r = instance.asEngine()->RegisterObjectType(Type<T>::toString().c_str(), sizeof(T), flags);
        AB_SCRIPT_ASSERT(r >= 0, std::string("Could not register type '" + name + "'").c_str(), AB_THROW, &instance);

        if (this->_ctorset && !this->_ctors.empty())
        {
            AB_SCRIPT_ASSERT(false, std::string("Can't register dummy constructor if there's another constructors.").c_str(), AB_THROW, &instance);
        }

        while (!this->_ctors.empty())
        {
            ConstructorClass memb = this->_ctors.front();
            std::string decomp = memb.decompose();
            AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering constructor for '" + name + "' as '" + decomp + "'");
            r = instance.asEngine()->RegisterObjectBehaviour(name.c_str(), AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT, decomp.c_str(), memb.address(), AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
            AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register constructor for type '" + name + "'").c_str(), AB_THROW, &instance);
            this->_ctors.pop();
        }

        if (this->_ctorset)
        {
            AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering dummy constructor for '" + name + "' as 'void f()'");
            int r = instance.asEngine()->RegisterObjectBehaviour(name.c_str(), AS_NAMESPACE_QUALIFIER asBEHAVE_CONSTRUCT, "void f()", this->_ctor, AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
            AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register dummy constructor for type '" + name + "'").c_str(), AB_THROW, &instance);
        }

        if (this->_dtorset)
        {
            AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering destructor for '" + name + "' as 'void f()'");
            r = instance.asEngine()->RegisterObjectBehaviour(name.c_str(), AS_NAMESPACE_QUALIFIER asBEHAVE_DESTRUCT, "void f()", this->_dtor, AS_NAMESPACE_QUALIFIER asCALL_CDECL_OBJLAST);
            AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register destructor for type '" + name + "'").c_str(), AB_THROW, &instance);
        }

        while (!this->_members.empty())
        {
            MemberClass memb = this->_members.front();
            AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering '" + name + "' member as '" + memb.decompose() + "'");

            r = instance.asEngine()->RegisterObjectProperty(name.c_str(), memb.decompose().c_str(), memb.offset());
            AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register member for type '" + name + "'").c_str(), AB_THROW, &instance);

            this->_members.pop();
        }

        while (!this->_accessors.empty())
        {
            AccessorClass memb = this->_accessors.front();
            AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering '" + name + "' accessor for property '" + memb.name() + "'");

            if (memb.hasSet())
            {
                MethodClass setf = memb.set();
                r = instance.asEngine()->RegisterObjectMethod(name.c_str(), setf.decompose().c_str(), setf.address(), AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
                AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register accessor's 'set' method for type '" + name + "'").c_str(), AB_THROW, &instance);
            }
            if (memb.hasGet())
            {
                MethodClass getf = memb.get();
                r = instance.asEngine()->RegisterObjectMethod(name.c_str(), getf.decompose().c_str(), getf.address(), AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
                AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register accessor's 'get' method for type '" + name + "'").c_str(), AB_THROW, &instance);
            }

            this->_accessors.pop();
        }

        while (!this->_methods.empty())
        {
            MethodClass memb = this->_methods.front();
            std::string decomp = memb.decompose();
            AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering method for '" + name + "' as '" + decomp + "'");
            r = instance.asEngine()->RegisterObjectMethod(name.c_str(), decomp.c_str(), memb.address(), AS_NAMESPACE_QUALIFIER asCALL_THISCALL);
            AB_SCRIPT_ASSERT(r >= 0, std::string("Can't register method for type '" + name + "'").c_str(), AB_THROW, &instance);
            this->_methods.pop();
        }
    }

public:
    ///
    /// Constructors
    ///
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR(ConstructorWrapper<T>, (void*), void));
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1>), (A1, void*), void));
        AB_PUSH_ARG(cc, A1);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2>), (A1, A2, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3>), (A1, A2, A3, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4>), (A1, A2, A3, A4, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5>), (A1, A2, A3, A4, A5, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6>), (A1, A2, A3, A4, A5, A6, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7>), (A1, A2, A3, A4, A5, A6, A7, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8>), (A1, A2, A3, A4, A5, A6, A7, A8, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        AB_PUSH_ARG(cc, A8);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        AB_PUSH_ARG(cc, A8);
        AB_PUSH_ARG(cc, A9);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        AB_PUSH_ARG(cc, A8);
        AB_PUSH_ARG(cc, A9);
        AB_PUSH_ARG(cc, A10);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        AB_PUSH_ARG(cc, A8);
        AB_PUSH_ARG(cc, A9);
        AB_PUSH_ARG(cc, A10);
        AB_PUSH_ARG(cc, A11);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        AB_PUSH_ARG(cc, A8);
        AB_PUSH_ARG(cc, A9);
        AB_PUSH_ARG(cc, A10);
        AB_PUSH_ARG(cc, A11);
        AB_PUSH_ARG(cc, A12);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        AB_PUSH_ARG(cc, A8);
        AB_PUSH_ARG(cc, A9);
        AB_PUSH_ARG(cc, A10);
        AB_PUSH_ARG(cc, A11);
        AB_PUSH_ARG(cc, A12);
        AB_PUSH_ARG(cc, A13);
        this->_ctors.push(cc);
        return *this;
    }

    template <typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
    ClassExporter& ctor()
    {
        ConstructorClass cc(AS_NAMESPACE_QUALIFIER asFUNCTIONPR((ConstructorWrapper<T, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>), (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, void*), void));
        AB_PUSH_ARG(cc, A1);
        AB_PUSH_ARG(cc, A2);
        AB_PUSH_ARG(cc, A3);
        AB_PUSH_ARG(cc, A4);
        AB_PUSH_ARG(cc, A5);
        AB_PUSH_ARG(cc, A6);
        AB_PUSH_ARG(cc, A7);
        AB_PUSH_ARG(cc, A8);
        AB_PUSH_ARG(cc, A9);
        AB_PUSH_ARG(cc, A10);
        AB_PUSH_ARG(cc, A11);
        AB_PUSH_ARG(cc, A12);
        AB_PUSH_ARG(cc, A13);
        AB_PUSH_ARG(cc, A14);
        this->_ctors.push(cc);
        return *this;
    }

    ///
    /// Destructor
    ///
    ClassExporter& dtor()
    {
        this->_dtor = AS_NAMESPACE_QUALIFIER asFUNCTION(&DestructorWrapper<T>);
        this->_dtorset = true;
        return *this;
    }

    ///
    /// Dummy destructor
    ///
    ClassExporter& dtor_dummy()
    {
        this->_dtor = AS_NAMESPACE_QUALIFIER asFUNCTION(&DummyDestructor);
        this->_dtorset = true;
        return *this;
    }

    ///
    /// Dummy constructor
    ///
    ClassExporter& ctor_dummy()
    {
        this->_ctor = AS_NAMESPACE_QUALIFIER asFUNCTION(&DummyConstructor);
        this->_ctorset = true;
        return *this;
    }

    ///
    /// Methods
    ///

    template <typename R>
    ClassExporter& method(std::string name, R (T::*func)())
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (), R, func));
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1>
    ClassExporter& method(std::string name, R (T::*func)(A1))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1), R, func));
        AB_PUSH_ARG(mthd, A1);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        AB_PUSH_ARG(mthd, A8);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        AB_PUSH_ARG(mthd, A8);
        AB_PUSH_ARG(mthd, A9);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        AB_PUSH_ARG(mthd, A8);
        AB_PUSH_ARG(mthd, A9);
        AB_PUSH_ARG(mthd, A10);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        AB_PUSH_ARG(mthd, A8);
        AB_PUSH_ARG(mthd, A9);
        AB_PUSH_ARG(mthd, A10);
        AB_PUSH_ARG(mthd, A11);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        AB_PUSH_ARG(mthd, A8);
        AB_PUSH_ARG(mthd, A9);
        AB_PUSH_ARG(mthd, A10);
        AB_PUSH_ARG(mthd, A11);
        AB_PUSH_ARG(mthd, A12);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        AB_PUSH_ARG(mthd, A8);
        AB_PUSH_ARG(mthd, A9);
        AB_PUSH_ARG(mthd, A10);
        AB_PUSH_ARG(mthd, A11);
        AB_PUSH_ARG(mthd, A12);
        AB_PUSH_ARG(mthd, A13);
        this->_methods.push(mthd);
        return *this;
    }

    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
    ClassExporter& method(std::string name, R (T::*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14))
    {
        MethodClass mthd(name, Type<R>::toString(), AB_METHOD(T, (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14), R, func));
        AB_PUSH_ARG(mthd, A1);
        AB_PUSH_ARG(mthd, A2);
        AB_PUSH_ARG(mthd, A3);
        AB_PUSH_ARG(mthd, A4);
        AB_PUSH_ARG(mthd, A5);
        AB_PUSH_ARG(mthd, A6);
        AB_PUSH_ARG(mthd, A7);
        AB_PUSH_ARG(mthd, A8);
        AB_PUSH_ARG(mthd, A9);
        AB_PUSH_ARG(mthd, A10);
        AB_PUSH_ARG(mthd, A11);
        AB_PUSH_ARG(mthd, A12);
        AB_PUSH_ARG(mthd, A13);
        AB_PUSH_ARG(mthd, A14);
        this->_methods.push(mthd);
        return *this;
    }

    ///
    /// Accessor functions
    ///
    template <typename V>
    ClassExporter& property(std::string name, V (T::*getf)() const, void (T::*setf)(V))
    {
        AccessorClass access(name, Type<V>::toString(), AB_METHOD(T, () const, V, getf), AB_METHOD(T, (V), void, setf));
        this->_accessors.push(access);
        return *this;
    }

    ///
    /// Accessor functions
    ///
    template <typename V>
    ClassExporter& property(std::string name, V (T::*getf)(int) const, void (T::*setf)(int, V))
    {
        AccessorClass access(name, Type<V>::toString(), AB_METHOD(T, (int)const, V, getf), AB_METHOD(T, (int, V), void, setf));
        this->_accessors.push(access);
        return *this;
    }

    ///
    /// Registers a read-only property
    ///
    template <typename V>
    ClassExporter& property_reader(std::string name, V (T::*getf)() const)
    {
        AccessorClass access(name, Type<V>::toString(), AccessorClass::ReadOnly, AB_METHOD(T, () const, V, getf));
        this->_accessors.push(access);
        return *this;
    }

    ///
    /// Registers a read-only property
    ///
    template <typename V>
    ClassExporter& property_reader(std::string name, V (T::*getf)(int) const)
    {
        AccessorClass access(name, Type<V>::toString(), AccessorClass::ReadOnly, AB_METHOD(T, (int)const, V, getf));
        this->_accessors.push(access);
        return *this;
    }

    ///
    /// Registers a write-only property
    ///
    template <typename V>
    ClassExporter& property_writer(std::string name, void (T::*setf)(V))
    {
        AccessorClass access(name, Type<V>::toString(), AccessorClass::WriteOnly, AB_METHOD(T, (V), void, setf));
        this->_accessors.push(access);
        return *this;
    }

    ///
    /// Registers a write-only property
    ///
    template <typename V>
    ClassExporter& property_writer(std::string name, void (T::*setf)(int, V))
    {
        AccessorClass access(name, Type<V>::toString(), AccessorClass::WriteOnly, AB_METHOD(T, (int, V), void, setf));
        this->_accessors.push(access);
        return *this;
    }

    ///
    /// Registers a member of the struct
    ///
    template <typename V>
    ClassExporter& member(std::string name, V T::*offset)
    {
        MemberClass memb(name, Type<V>::toString(), *(int*)&offset);
        this->_members.push(memb);
        return *this;
    }
};

///
/// Start exporting functions to the script
///
class Exporter
{
private:
    /// Stores the script instance.
    Engine& _engine;

protected:
    ///
    /// Initializes the exporter
    ///
    Exporter(Engine& engine);

public:
    ///
    /// Starts an exporter
    ///
    static Exporter Export(Engine& engine);

    ///
    /// FunctionExporter wrapper
    ///
    static FunctionExporter Functions();

    ///
    /// VariableExporter wrapper
    ///
    static VariableExporter Variables();

    ///
    /// ClassExporter wrapper
    ///
    template <typename T>
    static ClassExporter<T> Class(int flags = 0)
    {
        ClassExporter<T> exporter(flags);
        return exporter;
    };

    ///
    /// Exports the functions
    ///
    template <typename T>
    void operator[](T& exp)
    {
        BaseExporter* exporter = dynamic_cast<BaseExporter*>(&exp);
        if (exporter != NULL)
        {
            exporter->finish(this->_engine);
        }
    }
};

AB_END_NAMESPACE
