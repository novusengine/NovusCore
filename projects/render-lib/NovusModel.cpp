#include "NovusModel.h"
#include <iostream>
#include <fstream>
#include <cassert>

#include "NovusDevice.h"

NovusModel::~NovusModel()
{
    vkDestroyBuffer(_device->device, _indexBuffer, nullptr);
    vkFreeMemory(_device->device, _indexBufferMemory, nullptr);

    vkDestroyBuffer(_device->device, _vertexBuffer, nullptr);
    vkFreeMemory(_device->device, _vertexBufferMemory, nullptr);
}

bool NovusModel::LoadFromFile(NovusDevice* device, const std::string& filePath)
{
    _device = device;

    std::ifstream file(filePath, std::ios::binary);

    // get length of file:
    file.seekg(0, file.end);
    i32 fileLength = static_cast<int>(file.tellg());
    file.seekg(0, file.beg);

    // Make sure it's big enough to at least have a type header
    assert(fileLength > sizeof(NovusTypeHeader));

    // Read and verify the type header
    file.read((char*)&_typeHeader, sizeof(NovusTypeHeader));
    assert(_typeHeader == EXPECTED_TYPE_HEADER);

    // Read vertex count
    file.read((char*)&_vertexCount, sizeof(u32));

    std::vector<Vertex> vertices(_vertexCount);

    // Read vertices
    for (u32 i = 0; i < _vertexCount; i++)
    {
        file.read((char*)& vertices[i].pos, sizeof(Vector3));
        file.read((char*)& vertices[i].normal, sizeof(Vector3));
        file.read((char*)& vertices[i].texCoord, sizeof(Vector2));
    }

    // Read index type
    file.read((char*)&_primitiveTopology, sizeof(VkPrimitiveTopology));
    
    // Read index count
    file.read((char*)&_indexCount, sizeof(u32));

    // Read indices
    std::vector<i16> indices(_indexCount);
    for (u32 i = 0; i < _indexCount; i++)
    {
        file.read((char*)& indices[i], sizeof(i16));
    }

    CreateVertexBuffer(vertices);
    CreateIndexBuffer(indices);
    CreateUniformBuffers();

    _attributeDescriptions.resize(2);
    _attributeDescriptions[0].binding = 0;
    _attributeDescriptions[0].location = 0;
    _attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    _attributeDescriptions[0].offset = offsetof(Vertex, pos);

    _attributeDescriptions[1].binding = 0;
    _attributeDescriptions[1].location = 1;
    _attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    _attributeDescriptions[1].offset = offsetof(Vertex, texCoord);
    
    return true;
}

void NovusModel::CreateVertexBuffer(std::vector<Vertex>& vertices)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    _device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(_device->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(_device->device, stagingBufferMemory);

    _device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertexBuffer, _vertexBufferMemory);

    _device->CopyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

    vkDestroyBuffer(_device->device, stagingBuffer, nullptr);
    vkFreeMemory(_device->device, stagingBufferMemory, nullptr);
}

void NovusModel::CreateIndexBuffer(std::vector<i16>& indices)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    _device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(_device->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(_device->device, stagingBufferMemory);

    _device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

    _device->CopyBuffer(stagingBuffer, _indexBuffer, bufferSize);

    vkDestroyBuffer(_device->device, stagingBuffer, nullptr);
    vkFreeMemory(_device->device, stagingBufferMemory, nullptr);
}

void NovusModel::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(Matrix);

    _uniformBuffers.resize(_device->swapChainImages.size());
    _uniformBuffersMemory.resize(_device->swapChainImages.size());

    for (size_t i = 0; i < _device->swapChainImages.size(); i++)
    {
        _device->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);
    }
}

void NovusModel::UpdateUniformBuffer(Matrix& modelMatrix, size_t imageIndex)
{
    // Model matrix
    _modelMatrix = modelMatrix;

    void* data;
    vkMapMemory(_device->device, _uniformBuffersMemory[imageIndex], 0, sizeof(_modelMatrix), 0, &data);
    memcpy(data, &_modelMatrix, sizeof(_modelMatrix));
    vkUnmapMemory(_device->device, _uniformBuffersMemory[imageIndex]);
}