#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Texture.h"
#include "Buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& file_path, const vk::Device device, const vk::PhysicalDevice physical_device)
{
	int tex_width, tex_height, tex_channels;

	std::string path = "assets/textures/" + file_path;

	m_filePath = path.c_str();

	stbi_uc* pixels = stbi_load(m_filePath, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

	vk::DeviceSize image_size = tex_width * tex_height * 4;

	if (!pixels)
		throw std::runtime_error("Failed to load texture image!");

	Buffer::CreateBuffer(device, physical_device, image_size, vk::BufferUsageFlagBits::eTransferSrc,
	                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	                     m_stagingBuffer, m_stagingBufferMemory);

	void* data;
	device.mapMemory(m_stagingBufferMemory, 0, image_size, {}, &data);
	memcpy(data, pixels, image_size);
	device.unmapMemory(m_stagingBufferMemory);

	stbi_image_free(pixels);

	CreateImage(device, physical_device, tex_width, tex_height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
	            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
	            vk::MemoryPropertyFlagBits::eDeviceLocal, m_textureImage, m_textureImageMemory);
}

void Texture::CreateImage(const vk::Device device,
                          const vk::PhysicalDevice physical_device,
                          uint32_t width,
                          uint32_t height,
                          vk::Format format,
                          vk::ImageTiling tiling,
                          vk::ImageUsageFlags usage,
                          const vk::MemoryPropertyFlags properties,
                          vk::Image& image,
                          vk::DeviceMemory& image_memory)
{
	vk::ImageCreateInfo image_info{
		// Optional
		.flags = {},
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent{
			.width = width,
			.height = height,
			.depth = 1
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	if (device.createImage(&image_info, nullptr, &image) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create image!");

	vk::MemoryRequirements mem_requirements;
	device.getImageMemoryRequirements(image, &mem_requirements);

	vk::MemoryAllocateInfo alloc_info{
		.allocationSize = mem_requirements.size,
		.memoryTypeIndex = Buffer::FindMemoryType(mem_requirements.memoryTypeBits,
		                                          properties, physical_device)
	};

	if (device.allocateMemory(&alloc_info, nullptr, &image_memory) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate image memory!");

	device.bindImageMemory(image, image_memory, 0);
}
