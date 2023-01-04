#pragma once
#include <vulkan/vulkan.hpp>

class Texture
{
public:
	Texture(const std::string& file_path, vk::Device device, vk::PhysicalDevice physical_device);

private:
	static void CreateImage(vk::Device device,
	                        vk::PhysicalDevice physical_device,
	                        uint32_t width,
	                        uint32_t height,
	                        vk::Format format,
	                        vk::ImageTiling tiling,
	                        vk::ImageUsageFlags usage,
	                        vk::MemoryPropertyFlags properties,
	                        vk::Image& image,
	                        vk::DeviceMemory& image_memory);

	const char* m_filePath;

	vk::Buffer m_stagingBuffer;

	vk::DeviceMemory m_stagingBufferMemory;

	vk::Image m_textureImage;

	vk::DeviceMemory m_textureImageMemory;
};
