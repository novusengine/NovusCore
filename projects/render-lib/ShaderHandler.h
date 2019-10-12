#pragma once
#include <NovusTypes.h>
#include <vulkan/vulkan.h>
#include <vector>
#include "ShaderHandle.h"

class ShaderHandler
{
public:
    ShaderHandler() {};
    ~ShaderHandler();
    
    ShaderHandleVert LoadShaderVert(VkDevice device, const std::string& shaderPath) { return static_cast<ShaderHandleVert>(LoadShader(device, shaderPath, vertexShaders)); }
    ShaderHandleFrag LoadShaderFrag(VkDevice device, const std::string& shaderPath) { return static_cast<ShaderHandleFrag>(LoadShader(device, shaderPath, fragmentShaders)); }
    ShaderHandleComp LoadShaderComp(VkDevice device, const std::string& shaderPath) { return static_cast<ShaderHandleComp>(LoadShader(device, shaderPath, computeShaders)); }

    VkShaderModule GetShaderModuleVert(ShaderHandleVert handle) { return GetShaderModule(vertexShaders, static_cast<__ShaderHandle>(handle)); }
    VkShaderModule GetShaderModuleFrag(ShaderHandleFrag handle) { return GetShaderModule(fragmentShaders, static_cast<__ShaderHandle>(handle)); }
    VkShaderModule GetShaderModuleComp(ShaderHandleComp handle) { return GetShaderModule(computeShaders, static_cast<__ShaderHandle>(handle)); }

private:
    struct LoadedShader
    {
        __ShaderHandle handle;
        VkShaderModule shaderModule;
        std::string name;
        std::string path;
        VkDevice device;
    };

    typedef std::vector<LoadedShader> LoadedShaders;
    __ShaderHandle LoadShader(VkDevice device, const std::string& shaderPath, LoadedShaders& shaders);
    VkShaderModule GetShaderModule(LoadedShaders& shaders, __ShaderHandle handle);

    typedef std::vector<char> ShaderBinary;
    ShaderBinary ReadFile(const std::string& filename);
    VkShaderModule CreateShaderModule(VkDevice device, const ShaderBinary& binary);
private:
    LoadedShaders vertexShaders;
    LoadedShaders fragmentShaders;
    LoadedShaders computeShaders;
};