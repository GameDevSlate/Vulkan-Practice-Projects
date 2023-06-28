#pragma once
#include <vulkan/vulkan.hpp>

#include "Texture.h"

class VkUniform
{
public:
	static void CreateDescriptorSetLayout(vk::Device device, vk::DescriptorSetLayout& descriptor_set_layout);

	static void CreateDescriptorPool(vk::Device device, vk::DescriptorPool& descriptor_pool);

	static void CreateDescriptorSets(vk::Device device,
	                                 std::vector<vk::DescriptorSet>& descriptor_sets,
	                                 const Texture& texture,
	                                 vk::DescriptorSetLayout descriptor_set_layout,
	                                 vk::DescriptorPool descriptor_pool,
	                                 const std::vector<vk::Buffer>& uniform_buffers);

	static void UpdateUniformBuffer(vk::Device device,
	                                uint32_t current_image,
	                                vk::Extent2D swap_chain_extent,
	                                const std::vector<vk::DeviceMemory>&
	                                uniform_buffers_memory);
};
