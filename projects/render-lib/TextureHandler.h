#pragma once
#include <NovusTypes.h>
#include <vulkan/vulkan.h>
#include <vector>
#include "TextureHandle.h"

typedef unsigned char stbi_uc;
class NovusDevice;

class TextureHandler
{
public:
    TextureHandler() {};
    ~TextureHandler();

    TextureHandle LoadTexture(NovusDevice* device, const std::string& texturePath);

    VkImageView GetTextureImageView(TextureHandle handle);

private:
    struct LoadedTexture
    {
        TextureHandle handle;
        int width;
        int height;
        int channels;
        std::string name;
        std::string path;

        VkImage textureImage;
        VkImageView textureImageView;

        VkDeviceMemory textureImageMemory;
        VkDevice device;
    };

    stbi_uc* ReadFile(const std::string& filename, int& width, int& height, int& channels);
    void CreateTexture(NovusDevice* device, LoadedTexture& texture, stbi_uc* pixels);
private:
    std::vector<LoadedTexture> loadedTextures;
};