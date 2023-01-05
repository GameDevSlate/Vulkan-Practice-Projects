#pragma once
#include <vulkan/vulkan.hpp>

class Texture
{
public:
	Texture(const std::string& file_path,
	        vk::Device device,
	        vk::PhysicalDevice physical_device,
	        vk::CommandPool command_pool,
	        const vk::Queue& queue);

	void Destroy(vk::Device device) const;

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

	static void TransitionImageLayout(vk::Device device,
	                                  vk::CommandPool command_pool,
	                                  const vk::Queue& graphics_queue,
	                                  vk::Image image,
	                                  vk::Format format,
	                                  vk::ImageLayout old_layout,
	                                  vk::ImageLayout new_layout);

	const char* m_filePath;

	vk::Image m_textureImage;

	vk::DeviceMemory m_textureImageMemory;
};
