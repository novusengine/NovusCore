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