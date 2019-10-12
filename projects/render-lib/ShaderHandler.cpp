#include "ShaderHandler.h"
#include <Utils/DebugHandler.h>
#include <fstream>
#include <limits>

ShaderHandler::~ShaderHandler()
{
    for (LoadedShader shader : vertexShaders)
    {
        vkDestroyShaderModule(shader.device, shader.shaderModule, nullptr);
    }
    for (LoadedShader shader : fragmentShaders)
    {
        vkDestroyShaderModule(shader.device, shader.shaderModule, nullptr);
    }
    for (LoadedShader shader : computeShaders)
    {
        vkDestroyShaderModule(shader.device, shader.shaderModule, nullptr);
    }
}

__ShaderHandle ShaderHandler::LoadShader(VkDevice device, const std::string& shaderPath, ShaderHandler::LoadedShaders& shaders)
{
    ShaderBinary shaderBinary = ReadFile(shaderPath);
    size_t handle = shaders.size();

    // Make sure we haven't exceeded the limit of the ShaderHandle type, if this hits you need to change type of __ShaderHandle to something bigger
    assert(handle <= __ShaderHandle::MaxValue());
    using type = type_safe::underlying_type<__ShaderHandle>;

    LoadedShader loadedShader;
    loadedShader.handle = __ShaderHandle(static_cast<type>(handle));
    loadedShader.shaderModule = CreateShaderModule(device, shaderBinary);
    loadedShader.path = shaderPath;

    shaders.push_back(loadedShader);

    return loadedShader.handle;
}


VkShaderModule ShaderHandler::GetShaderModule(LoadedShaders& shaders, __ShaderHandle handle)
{
    using type = type_safe::underlying_type<__ShaderHandle>;

    // Lets make sure this handle exists
    assert(shaders.size() > static_cast<type>(handle));
    return shaders[static_cast<type>(handle)].shaderModule;
}

ShaderHandler::ShaderBinary ShaderHandler::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) 
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    ShaderBinary buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

VkShaderModule ShaderHandler::CreateShaderModule(VkDevice device, const ShaderHandler::ShaderBinary& binary)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = binary.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(binary.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}