//
//     Copyright � 2011 - Jo�o Francisco Biondo Trinca
//          a.k.a WoLfulus <wolfulus@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#if defined(_WIN32)
#include <windows.h>
#endif

#include "AngelBinder.h"

#pragma warning(disable: 4355)

#define AB_INIT_CHECK()		AB_SCRIPT_ASSERT(this->_engine != NULL && this->_builder != NULL, "Script not initialized.", AB_THROW, this)
#define AB_UNINIT_CHECK()	AB_SCRIPT_ASSERT(this->_engine == NULL && this->_builder == NULL, "Script already initialized.", AB_THROW, this)

#ifdef AS_USE_NAMESPACE
using namespace AngelScript;
#endif

AB_BEGIN_NAMESPACE
 
/**
 * Engine
 **/

void Engine::initialize()
{
	AB_UNINIT_CHECK()
	{
		this->_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		AB_SCRIPT_ASSERT(this->_engine != NULL, "Could not create AngelScript instance.", AB_THROW, this);

		this->_builder = new CScriptBuilder();
		AB_SCRIPT_ASSERT(this->_builder != NULL, "Could not create CScriptBuilder instance.", AB_THROW, this);

		this->_engine->SetMessageCallback(asFUNCTION(&Engine::onScriptMessage), this, asCALL_CDECL);
		this->_engine->SetEngineProperty(asEP_SCRIPT_SCANNER, 0);
	}
}

void Engine::uninitialize()
{
	if(this->_builder != NULL)
	{
		delete this->_builder;
	}
	this->_engine = NULL;
	if(this->_engine != NULL)
	{
		this->_engine->Release();
	}
	this->_engine = NULL;
}

void Engine::onScriptMessage( const asSMessageInfo *msg, void *param )
{
	Engine* engine = (Engine*)(param);
	if(engine != NULL)
	{
		std::stringstream stream;
		switch(msg->type)
		{
		case asMSGTYPE_ERROR:
			stream << "[Error] ";
			break;
		case asMSGTYPE_INFORMATION:
			stream << "[Information] ";
			break;
		case asMSGTYPE_WARNING:
			stream << "[Warning] ";
			break;
		}
		stream << "Line (" << msg->row << ", " << msg->col << ") : " << msg->message;
		if(engine->onMessage() != NULL)
		{
			engine->onMessage()(engine, stream.str());
		}
	}
}

Engine::Engine()
	: _engine(NULL), _builder(NULL), _messages(NULL), _pool(this)
{
	this->initialize();
}

Engine::Engine( MessageCallback callback )
	: _engine(NULL), _builder(NULL), _messages(callback), _pool(this)
{
	this->initialize();
}

Engine::~Engine()
{
	this->uninitialize();
}

ASEngine* Engine::asEngine()
{
	return this->_engine;
}

ASBuilder* Engine::asBuilder()
{
	return this->_builder;
}

Engine::MessageCallback& Engine::onMessage()
{
	return this->_messages;
}

Module* Engine::createModule( std::string name )
{
	std::unordered_map<std::string, Module*>::iterator it = this->_modules.find(name);
	if(it != this->_modules.end())
	{
		return it->second;
	}
	Module* module = new Module(*this, name);
	if(module == NULL)
	{
		throw std::runtime_error("Couldn't create script module instance.");
	}
	this->_modules[name] = module;
	return module;
}

Module* Engine::getModule( std::string name )
{
	std::unordered_map<std::string, Module*>::iterator it = this->_modules.find(name);
	if(it != this->_modules.end())
	{
		return it->second;
	}
	else
	{
		throw std::runtime_error("Couldn't create script module instance.");
	}
}

Context* Engine::getContext()
{
	Context* context = this->_pool.get();
	return context;
}

Context* Engine::getContext(int function)
{
	Context* context = this->_pool.get();
	context->prepare(function);
	return context;
}

void Engine::sleep( int ms )
{
#if defined(_WIN32)
	Sleep(ms);
#else
	usleep(ms*1000);
#endif
}

/**
 * Script Module
 **/

Module::Module(Engine& engine, std::string module)
	: _name(module), _engine(engine)
{
	int r = this->_engine.asBuilder()->StartNewModule(this->_engine.asEngine(), this->_name.c_str());
	AB_SCRIPT_ASSERT(r >= 0, "Failed to create new script module.", AB_THROW, &engine);
}

Module::~Module(void)
{
}

bool Module::compile ( std::string file )
{
	int r = 0;

	r = this->_engine.asBuilder()->AddSectionFromFile(file.c_str()); 
	AB_SCRIPT_ASSERT(r >= 0, std::string("Couldn't add script from file '" + file + "'").c_str(), AB_THROW, &this->_engine);

	r = this->_engine.asBuilder()->BuildModule(); 
	AB_SCRIPT_ASSERT(r >= 0, "Couldn't build script file.", AB_THROW, &this->_engine);

	return true;
}

Engine& Module::engine()
{
	return this->_engine;
}

/**
 * Extras
 **/

Exporter::Exporter( Engine& module ) 
	: _engine(module)
{
}

Exporter Exporter::Export( Engine& engine )
{
	Exporter exporter(engine);
	return exporter;
}

FunctionExporter Exporter::Functions()
{
	FunctionExporter exporter;
	return exporter;
}

VariableExporter Exporter::Variables()
{
	VariableExporter exporter;
	return exporter;
}

void FunctionExporter::finish( Engine& instance )
{
	while(!this->_entries.empty())
	{
		FunctionClass function = this->_entries.front();
		std::string decomposition = function.decompose();
		AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering function '" + function.name() + "' as '" + decomposition + "'");
		int r = instance.asEngine()->RegisterGlobalFunction(decomposition.c_str(), function.address(), function.convention());
		AB_SCRIPT_ASSERT(r >= 0, std::string("Error registering function '" + function.name() + "'").c_str(), AB_THROW, &instance);
		this->_entries.pop();
	}
}

FunctionExporter::FunctionExporter()
{
}

/**
 * Function methods
 **/

std::string FunctionClass::decompose()
{
	std::stringstream stream;
	stream << this->_ret << " " << this->_name << "(";
	for(size_t i = 0; i < this->_params.size(); i++) 
	{
		stream << this->_params[i];
		if(i != this->_params.size() - 1) 
		{
			stream << ", ";
		}
	}
	stream << ")";
	return stream.str();
}

asSFuncPtr FunctionClass::address()
{
	return this->_func;
}

std::vector<std::string>& FunctionClass::parameters()
{
	return this->_params;
}

std::string FunctionClass::name()
{
	return this->_name;
}

FunctionClass::FunctionClass( std::string ret, std::string name, CallingConvention conv, asSFuncPtr func ) 
	: _ret(ret), _name(name), _conv(conv), _func(func)
{
}

CallingConvention FunctionClass::convention()
{
	return this->_conv;
}

VariableExporter::VariableExporter()
{
}

void VariableExporter::finish( Engine& instance )
{
	while(!this->_entries.empty())
	{
		VariableClass memb = this->_entries.front();
		AB_MESSAGE_INVOKE_STATIC(&instance, &instance, "Registering global var '" + memb.name() + "' as '" + memb.type() + "'");
		int r = instance.asEngine()->RegisterGlobalProperty(memb.decompose().c_str(), memb.address());
		AB_SCRIPT_ASSERT(r >= 0, std::string("Error registering global var '" + memb.name() + "'").c_str(), AB_THROW, &instance);
		this->_entries.pop();
	}
}

VariableClass::VariableClass( std::string type, std::string name, void* address ) 
	: _type(type), _name(name), _address(address)
{
}

std::string VariableClass::decompose()
{
	std::string ret = this->_type + " " + this->_name;
	return ret;
}

void* VariableClass::address()
{
	return this->_address;
}

std::string VariableClass::name()
{
	return this->_name;
}

std::string VariableClass::type()
{
	return this->_type;
}

void DummyConstructor( void* memory )
{
}

void DummyDestructor( void* memory )
{
}

Context::Context(ContextPool& pool, int id) 
	: _id(id), _params(0), _pool(pool)
{
	this->_context = pool.engine()->asEngine()->CreateContext();
	AB_SCRIPT_ASSERT(this->_context != NULL, "Context couldn't be created.", AB_THROW, pool.engine());
	this->_context->SetExceptionCallback(asMETHOD(Context, exceptionCallback), this, asCALL_THISCALL);
}

Context::~Context()
{
	this->_context->Release();
}

void Context::exceptionCallback( asIScriptContext *context )
{
	int col;  
	std::stringstream error;
	int row = context->GetExceptionLineNumber(&col);
	error <<  "A script exception occurred: "  << context->GetExceptionString() << " at position (" << row << ',' << col << ')';
	throw std::runtime_error(error.str().c_str());
}

void Context::execute()
{
	int r = this->_context->Execute();
	AB_SCRIPT_ASSERT(r == asEXECUTION_FINISHED, "Error executing call to script.", AB_THROW, this->_pool.engine());
}

void Context::setAddress( void* value )
{
	this->_context->SetArgAddress(this->_params++, value);
}

void Context::setObject( void* value )
{
	this->_context->SetArgObject(this->_params++, value);
}

void Context::setByte( asBYTE value )
{
	this->_context->SetArgByte(this->_params++, value);
}

void Context::setWord( asWORD value )
{
	this->_context->SetArgWord(this->_params++, value);
}

void Context::setDWord( unsigned int value )
{
	this->_context->SetArgDWord(this->_params++, value);
}

void Context::setQWord( unsigned long long value )
{
	this->_context->SetArgQWord(this->_params++, value);
}

void Context::setFloat( float value )
{
	this->_context->SetArgFloat(this->_params++, value);
}

void Context::setDouble( double value )
{
	this->_context->SetArgDouble(this->_params++, value);
}

void Context::setBool( bool value )
{
	this->_context->SetArgByte(this->_params++, value ? 1 : 0);
}

void* Context::readAddress()
{
	return this->_context->GetReturnAddress();
}

void* Context::readObject()
{
	return this->_context->GetReturnObject();
}

asBYTE Context::readByte()
{
	return this->_context->GetReturnByte();
}

asWORD Context::readWord()
{
	return this->_context->GetReturnWord();
}

asQWORD Context::readQWord()
{
	return this->_context->GetReturnQWord();
}

asDWORD Context::readDWord()
{
	return this->_context->GetReturnDWord();
}

float Context::readFloat()
{
	return this->_context->GetReturnFloat();
}

double Context::readDouble()
{
	return this->_context->GetReturnDouble();
}

bool Context::readBool()
{
	return (bool)(this->_context->GetReturnByte() > 0);
}

void Context::prepare( int function )
{
	this->_params = 0;
	this->_context->Prepare(_context->GetFunction(function));
}

void Context::prepare(asIScriptFunction* function)
{
	this->_params = 0;
	this->_context->Prepare(function);
}

void Context::release()
{
	this->_pool.ret(this->_id);
}

ContextPool::ContextPool( Engine* engine ) 
	: _engine(engine), _availableCount(0), _releasing(false)
{

}

ContextPool::~ContextPool()
{
	this->_releasing = true;
	this->wait();
	std::for_each(this->_contexts.begin(), this->_contexts.end(), [] (Context* ctx) {
		delete ctx;
	});
}

Engine* ContextPool::engine()
{
	return this->_engine;
}

void ContextPool::wait()
{
	while(this->_availableCount != this->_contexts.size())	
	{
		Engine::sleep(10);
	}
}

void ContextPool::ret( int id )
{
	ScopedLocker locker(this->_locker);
    this->_available.enqueue(id);
	this->_availableCount++;
}

Context* ContextPool::get()
{
	ScopedLocker locker(this->_locker);
	if(this->_releasing) 
	{
		return NULL;
	}

	int id = -1;
	if(!this->_available.try_dequeue(id))
	{
		Context* context = new Context(*this, (int)this->_contexts.size());
		this->_contexts.push_back(context);
		return context;
	}
	else
	{
		this->_availableCount--;
		return this->_contexts[id];
	}

	return this->_contexts.at(id);
}

#if defined(_WIN32)

ThreadLocker::ThreadLocker()
{
	InitializeCriticalSection(&this->_section);
}

ThreadLocker::~ThreadLocker()
{
	DeleteCriticalSection(&this->_section);
}

void ThreadLocker::lock()
{
	EnterCriticalSection(&this->_section);
}

void ThreadLocker::unlock()
{
	LeaveCriticalSection(&this->_section);
}

#else 
ThreadLocker::ThreadLocker()
{
}

ThreadLocker::~ThreadLocker()
{
}

void ThreadLocker::lock()
{
	_mutex.lock();
}

void ThreadLocker::unlock()
{
	_mutex.unlock();
}
#endif

ScopedLocker::ScopedLocker( ThreadLocker& locker ) : _locker(locker)
{
	this->_locker.lock();
}

ScopedLocker::~ScopedLocker()
{
	this->_locker.unlock();
}

AB_END_NAMESPACE
