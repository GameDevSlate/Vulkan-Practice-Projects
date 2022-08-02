#pragma once
#include <vulkan/vulkan.hpp>

class VkUniform
{
public:
	static void CreateDescriptorSetLayout(vk::Device device, vk::DescriptorSetLayout& descriptor_set_layout);

	static void UpdateUniformBuffer(vk::Device device,
	                                uint32_t current_image,
	                                vk::Extent2D swap_chain_extent,
	                                std::vector<vk::DeviceMemory>&
	                                uniform_buffers_memory);
};
