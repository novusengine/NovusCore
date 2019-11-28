#include "TextureHandler.h"
#include "NovusDevice.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureHandler::~TextureHandler()
{
    for (LoadedTexture& texture : loadedTextures)
    {
        vkDestroyImageView(texture.device, texture.textureImageView, nullptr);

        vkDestroyImage(texture.device, texture.textureImage, nullptr);
        vkFreeMemory(texture.device, texture.textureImageMemory, nullptr);
    }
}

TextureHandle TextureHandler::LoadTexture(NovusDevice* device, const std::string& texturePath)
{
    LoadedTexture texture;

    stbi_uc* pixels;
    pixels = ReadFile(texturePath, texture.width, texture.height, texture.channels);
    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    CreateTexture(device, texture, pixels);

    size_t handle = loadedTextures.size();

    // Make sure we haven't exceeded the limit of the TextureHandle type, if this hits you need to change type of TextureHandle to something bigger
    assert(handle <= TextureHandle::MaxValue());
    using type = type_safe::underlying_type<TextureHandle>;
   
    texture.device = device->device;
    texture.path = texturePath;
    texture.handle = TextureHandle(static_cast<type>(handle));

    loadedTextures.push_back(texture);

    return texture.handle;
}

VkImageView TextureHandler::GetTextureImageView(TextureHandle handle)
{
    using type = type_safe::underlying_type<TextureHandle>;

    // Lets make sure this handle exists
    assert(loadedTextures.size() > static_cast<type>(handle));
    return loadedTextures[static_cast<type>(handle)].textureImageView;
}

stbi_uc* TextureHandler::ReadFile(const std::string& filename, int& width, int& height, int& channels)
{
    return stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
}

void TextureHandler::CreateTexture(NovusDevice* device, LoadedTexture& texture, stbi_uc* pixels)
{
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    VkDeviceSize imageSize = texture.width * texture.height * 4;

    device->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device->device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device->device, stagingBufferMemory);

    stbi_image_free(pixels);

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(texture.width);
    imageInfo.extent.height = static_cast<uint32_t>(texture.height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(device->device, &imageInfo, nullptr, &texture.textureImage) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device->device, texture.textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(device->device, &allocInfo, nullptr, &texture.textureImageMemory) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device->device, texture.textureImage, texture.textureImageMemory, 0);

    device->TransitionImageLayout(texture.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device->CopyBufferToImage(stagingBuffer, texture.textureImage, static_cast<uint32_t>(texture.width), static_cast<uint32_t>(texture.height));
    device->TransitionImageLayout(texture.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = texture.textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    texture.textureImageView = device->CreateImageView(texture.textureImage, VK_FORMAT_R8G8B8A8_UNORM);


}