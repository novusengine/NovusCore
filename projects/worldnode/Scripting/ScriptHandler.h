#pragma once
#include <string>
#include <filesystem>
#include <asio.hpp>
#include <entt.hpp>
#include "AngelBinder.h"

namespace AngelBinder
{
class Engine;
}

class ScriptHandler
{
public:
    static void SetIOService(asio::io_service* service) { _ioService = service; }
    static void SetRegistry(entt::registry* registry) { _registry = registry; }
    static void LoadScriptDirectory(std::string& path);
    static void ReloadScripts();

private:
    static bool LoadScript(std::filesystem::path path);
    static void RegisterFunctions(AB_NAMESPACE_QUALIFIER Engine* engine);

    ScriptHandler();

private:
    static std::string _path;
    static asio::io_service* _ioService;
    static entt::registry* _registry;
};