#pragma once
#include <NovusTypes.h>
#include <NovusTypeHeader.h>
#include <vulkan/vulkan.h>
#include <vector>
#include "TextureHandle.h"

struct Vertex
{
    Vector3 pos;
    Vector3 normal;
    Vector2 texCoord;

    static VkVertexInputBindingDescription GetBindingDescription() 
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
};

class NovusDevice;
class NovusModel
{
    // Update this when the format exported from the converter gets changed
    const NovusTypeHeader EXPECTED_TYPE_HEADER = NovusTypeHeader(42, 2);
public:
    ~NovusModel();

    bool LoadFromFile(NovusDevice* device, const std::string& filePath);
    NovusTypeHeader& GetTypeHeader() { return _typeHeader; }

    VkBuffer& GetVertexBuffer(){ return _vertexBuffer; }
    u32 GetVertexCount() { return _vertexCount; }

    VkBuffer& GetIndexBuffer(){ return _indexBuffer; }
    u32 GetIndexCount() { return _indexCount; }

    void UpdateUniformBuffer(Matrix& modelMatrix, size_t imageIndex);
    VkBuffer& GetUniformBuffer(size_t frame) { return _uniformBuffers[frame]; }

    VkPrimitiveTopology& GetPrimitiveTopology() { return _primitiveTopology; }
    VkVertexInputBindingDescription GetBindingDescription() { return Vertex::GetBindingDescription(); }
    std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() { return _attributeDescriptions; }

    TextureHandle GetTextureHandle() { return _textureHandle; }
    void SetTextureHandle(TextureHandle handle) { _textureHandle = handle; }

private:
    void CreateVertexBuffer(std::vector<Vertex>& vertices);
    void CreateIndexBuffer(std::vector<i16>& indices);
    void CreateUniformBuffers();
    
    
private:
    NovusTypeHeader _typeHeader;
    NovusDevice* _device;

    Matrix _modelMatrix;
    
    u32 _vertexCount;
    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;

    u32 _indexCount;
    VkBuffer _indexBuffer;
    VkDeviceMemory _indexBufferMemory;

    std::vector<VkBuffer> _uniformBuffers;
    std::vector<VkDeviceMemory> _uniformBuffersMemory;

    VkPrimitiveTopology _primitiveTopology;
    std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;

    TextureHandle _textureHandle;
};