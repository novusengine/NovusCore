#include "ScriptHandler.h"

#include <Utils/DebugHandler.h>
#include <Utils/Timer.h>

#include "AngelBinder.h"

// Angelscript Addons
#include "Addons/scriptarray/scriptarray.h"
#include "Addons/scriptbuilder/scriptbuilder.h"
#include "Addons/scriptstdstring/scriptstdstring.h"

#include "ScriptEngine.h"

// NovusCore functions
#include "GlobalFunctions.h"
#include "PacketFunctions.h"

// NovusCore hooks
#include "PlayerHooks.h"

namespace fs = std::filesystem;

std::string ScriptHandler::_path = "";
asio::io_service* ScriptHandler::_ioService = nullptr;

void ScriptHandler::ReloadScripts()
{
	NC_LOG_MESSAGE("Reloading scripts...");
    PlayerHooks::ClearHooks();

	if (_path != "")
	{
		LoadScriptDirectory(_path);
	}
}

void MessageCallback(const asSMessageInfo *msg, void *param)
{
	if (msg->type == asMSGTYPE_ERROR)
	{
		NC_LOG_ERROR("[Script]: %s (%d, %d) : %s\n", msg->section, msg->row, msg->col, msg->message);
	}
	else if (msg->type == asMSGTYPE_WARNING)
	{
		NC_LOG_WARNING("[Script]: %s (%d, %d) : %s\n", msg->section, msg->row, msg->col, msg->message);
	}
	else if (msg->type == asMSGTYPE_INFORMATION)
	{
		NC_LOG_MESSAGE("[Script]: %s (%d, %d) : %s\n", msg->section, msg->row, msg->col, msg->message);
	}
}

void ScriptHandler::LoadScriptDirectory(std::string& path)
{
	if (ScriptEngine::GetRegisterFunction() == nullptr)
	{
		ScriptEngine::SetRegisterFunction(&RegisterFunctions);
		ScriptEngine::SetIOService(_ioService);
	}
		
	_path = path;
	fs::path absolutePath = fs::absolute(path);

	Timer timer;
	size_t count = 0;
	for (auto& p : fs::recursive_directory_iterator(absolutePath))
	{
		if (p.is_directory())
			continue;

		if (LoadScript(p.path()))
		{
			count++;
		}
	}
	f32 msTimeTaken = timer.GetLifeTime()*1000;
	NC_LOG_SUCCESS("Loaded %u scripts in %.2f ms", count, msTimeTaken);
}

bool ScriptHandler::LoadScript(fs::path path)
{
	AngelBinder::Engine* engine = ScriptEngine::GetScriptEngine();

	std::string moduleName = path.filename().string();

	CScriptBuilder builder;
	int r = builder.StartNewModule(engine->asEngine(), moduleName.c_str());
	if (r < 0)
	{
		// If the code fails here it is usually because there
		// is no more memory to allocate the module
		NC_LOG_ERROR("[Script]: Unrecoverable error while starting a new module.");
		return false;
	}
	r = builder.AddSectionFromFile(path.string().c_str());
	if (r < 0)
	{
		// The builder wasn't able to load the file. Maybe the file
		// has been removed, or the wrong name was given, or some
		// preprocessing commands are incorrectly written.
		NC_LOG_ERROR("[Script]: Please correct the errors in the script and try again.\n");
		return false;
	}
	r = builder.BuildModule();
	if (r < 0)
	{
		// An error occurred. Instruct the script writer to fix the 
		// compilation errors that were listed in the output stream.
		NC_LOG_ERROR("[Script]: Please correct the errors in the script and try again.\n");
		return false;
	}

	asIScriptModule *mod = engine->asEngine()->GetModule(moduleName.c_str());
	asIScriptFunction *func = mod->GetFunctionByDecl("void main()");
	if (func == 0)
	{
		// The function couldn't be found. Instruct the script writer
		// to include the expected function in the script.
		NC_LOG_ERROR("[Script]: The script must have the function 'void main()'. Please add it and try again.\n");
		return false;
	}

	// Create our context, prepare it, and then execute
	asIScriptContext *ctx = engine->asEngine()->CreateContext();
	ctx->Prepare(func);
	r = ctx->Execute();
	if (r != asEXECUTION_FINISHED)
	{
		// The execution didn't complete as expected. Determine what happened.
		if (r == asEXECUTION_EXCEPTION)
		{
			// An exception occurred, let the script writer know what happened so it can be corrected.
			NC_LOG_ERROR("[Script]: An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
			ctx->Release();
			return false;
		}
	}

	ctx->Release();
	return true;
}

void ScriptHandler::RegisterFunctions(AngelBinder::Engine* engine)
{
	// Defaults
	int result;
	result = engine->asEngine()->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	assert(result >= 0);
	RegisterScriptArray(engine->asEngine(), true);
	RegisterStdString(engine->asEngine());
	RegisterStdStringUtils(engine->asEngine());

	// Packet functions
	RegisterGlobalFunctions(engine);
	RegisterPacketFunctions(engine);
}