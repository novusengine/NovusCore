#pragma once
#include <NovusTypes.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <optional>

#include "ShaderHandler.h"
#include "TextureHandler.h"

class NovusDevice;
class NovusModel;
struct GLFWwindow;

struct UniformBufferObject 
{
    Matrix view;
    Matrix proj;
};

struct QueueFamilyIndices 
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() 
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails 
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Renderer
{
public:
    Renderer() {}
    ~Renderer();
    bool Init(GLFWwindow* window);
    
    void Render();
    void Present();

    void SetViewMatrix(const Matrix& viewMatrix) { _ubo.view = viewMatrix; }

    void RegisterRenderableCube(Vector3& position, Vector3& rotation = Vector3::Zero, Vector3& scale = Vector3::One);

private:
    void InitVulkan();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateDescriptorSetLayout();
    void CreateGraphicsPipeline();
    void CreateFrameBuffers();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void CreateSyncObjects();
    // This is where the model gets created
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    void CreateTextureSampler();

    std::vector<const char*> GetRequiredExtensions();
    bool CheckValidationLayerSupport();
    int RateDeviceSuitability(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void UpdateSharedUniformBuffer();

private:
    GLFWwindow* _window;
    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debugMessenger;

    NovusDevice* _device;

    VkSurfaceKHR _surface;

    ShaderHandler _shaderHandler;
    TextureHandler _textureHandler;
    VkSampler _textureSampler;

    VkDescriptorSetLayout _descriptorSetLayout;
    VkPipelineLayout _pipelineLayout;
    VkPipeline _graphicsPipeline;
    VkRenderPass _renderPass;

    std::vector<VkBuffer> _uniformBuffers;
    std::vector<VkDeviceMemory> _uniformBuffersMemory;

    VkDescriptorPool _descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> _commandBuffers;

    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    size_t _currentFrame = 0;

    uint32_t _imageIndex;

    UniformBufferObject _ubo;

    NovusModel* _model;
    std::vector<Matrix> _cubesToRender;
};