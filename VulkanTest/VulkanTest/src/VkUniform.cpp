#define VULKAN_HPP_NO_CONSTRUCTORS
#define GLM_FORCE_RADIANS

#include "VkUniform.h"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "UniformBufferObject.h"

void VkUniform::CreateDescriptorSetLayout(const vk::Device device, vk::DescriptorSetLayout& descriptor_set_layout)
{
	vk::DescriptorSetLayoutBinding ubo_layout_binding{
		.binding = 0,
		.descriptorType = vk::DescriptorType::eUniformBuffer,
		.descriptorCount = 1,
		.stageFlags = vk::ShaderStageFlagBits::eVertex,
		// optional
		.pImmutableSamplers = nullptr
	};

	vk::DescriptorSetLayoutCreateInfo layout_info{
		.bindingCount = 1,
		.pBindings = &ubo_layout_binding
	};

	if (device.createDescriptorSetLayout(&layout_info, nullptr, &descriptor_set_layout) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create descriptor set layout!");
}

void VkUniform::UpdateUniformBuffer(const vk::Device device,
                                    const uint32_t current_image,
                                    const vk::Extent2D swap_chain_extent,
                                    const std::vector<vk::DeviceMemory>&
                                    uniform_buffers_memory)
{
	static auto start_time = std::chrono::high_resolution_clock::now();

	auto current_time = std::chrono::high_resolution_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

	UniformBufferObject ubo{};

	ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.proj = glm::perspective(glm::radians(45.0f),
	                            swap_chain_extent.width / static_cast<float>(swap_chain_extent.height), 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	void* data;

	device.mapMemory(uniform_buffers_memory[current_image], 0, sizeof(ubo), {}, &data);

	memcpy(data, &ubo, sizeof(ubo));

	device.unmapMemory(uniform_buffers_memory[current_image]);
}
