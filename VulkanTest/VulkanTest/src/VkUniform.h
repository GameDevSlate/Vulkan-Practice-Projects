#pragma once
#include <vulkan/vulkan.hpp>

class VkUniform
{
public:
	static void CreateDescriptorSetLayout(vk::Device device, vk::DescriptorSetLayout& descriptor_set_layout);

	static void UpdateUniformBuffer(vk::Device device,
	                                uint32_t current_image,
	                                vk::Extent2D swap_chain_extent,
<<<<<<< HEAD
	                                const std::vector<vk::DeviceMemory>&
=======
	                                std::vector<vk::DeviceMemory>&
>>>>>>> main
	                                uniform_buffers_memory);
};
