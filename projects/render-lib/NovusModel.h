#pragma once
#include <NovusTypes.h>
#include <NovusTypeHeader.h>
#include <vulkan/vulkan.h>
#include <vector>

class NovusDevice;
class NovusModel
{
    // Update this when the format exported from the converter gets changed
    const NovusTypeHeader EXPECTED_TYPE_HEADER = NovusTypeHeader(42, 1);
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
    VkVertexInputBindingDescription& GetBindingDescription() { return _bindingDescription; }
    std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions() { return _attributeDescriptions; }

private:
    void CreateVertexBuffer(std::vector<Vector3>& vertexPositions);
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
    VkVertexInputBindingDescription _bindingDescription;
    std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;

    
};