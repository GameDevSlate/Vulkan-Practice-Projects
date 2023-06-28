#define VULKAN_HPP_NO_CONSTRUCTORS
#define GLM_FORCE_RADIANS

#include "VkUniform.h"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "HelloTriangleApplication.h"
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

	vk::DescriptorSetLayoutBinding sample_layout_binding{
		.binding = 1,
		.descriptorType = vk::DescriptorType::eCombinedImageSampler,
		.descriptorCount = 1,
		.stageFlags = vk::ShaderStageFlagBits::eFragment,
		.pImmutableSamplers = nullptr,
	};

	std::array bindings = {ubo_layout_binding, sample_layout_binding};

	vk::DescriptorSetLayoutCreateInfo layout_info{
		.bindingCount = static_cast<uint32_t>(bindings.size()),
		.pBindings = bindings.data()
	};

	if (device.createDescriptorSetLayout(&layout_info, nullptr, &descriptor_set_layout) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create descriptor set layout!");
}

void VkUniform::CreateDescriptorPool(const vk::Device device, vk::DescriptorPool& descriptor_pool)
{
	std::array<vk::DescriptorPoolSize, 2> pool_sizes{};

	pool_sizes[0].type = vk::DescriptorType::eUniformBuffer;
	pool_sizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	pool_sizes[1].type = vk::DescriptorType::eCombinedImageSampler;
	pool_sizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	vk::DescriptorPoolCreateInfo pool_info{
		.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		.poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
		.pPoolSizes = pool_sizes.data()
	};

	if (device.createDescriptorPool(&pool_info, nullptr, &descriptor_pool) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create descriptor pool!");
}

void VkUniform::CreateDescriptorSets(const vk::Device device,
                                     std::vector<vk::DescriptorSet>& descriptor_sets,
                                     const Texture& texture,
                                     const vk::DescriptorSetLayout descriptor_set_layout,
                                     const vk::DescriptorPool descriptor_pool,
                                     const std::vector<vk::Buffer>& uniform_buffers)
{
	std::vector layouts(MAX_FRAMES_IN_FLIGHT, descriptor_set_layout);

	vk::DescriptorSetAllocateInfo alloc_info{
		.descriptorPool = descriptor_pool,
		.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT),
		.pSetLayouts = layouts.data()
	};

	descriptor_sets.resize(MAX_FRAMES_IN_FLIGHT);

	if (device.allocateDescriptorSets(&alloc_info, descriptor_sets.data()) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate descriptor sets");

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vk::DescriptorBufferInfo buffer_info{
			.buffer = uniform_buffers[i],
			.offset = 0,
			.range = sizeof(UniformBufferObject)
		};

		vk::DescriptorImageInfo image_info{
			.sampler = texture.m_textureSampler,
			.imageView = texture.m_textureImageView,
			.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
		};

		std::array<vk::WriteDescriptorSet, 2> descriptor_writes{};

		descriptor_writes[0].dstSet = descriptor_sets[i];
		descriptor_writes[0].dstBinding = 0;
		descriptor_writes[0].dstArrayElement = 0;
		descriptor_writes[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptor_writes[0].descriptorCount = 1;
		descriptor_writes[0].pBufferInfo = &buffer_info;

		descriptor_writes[1].dstSet = descriptor_sets[i];
		descriptor_writes[1].dstBinding = 1;
		descriptor_writes[1].dstArrayElement = 0;
		descriptor_writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptor_writes[1].descriptorCount = 1;
		descriptor_writes[1].pImageInfo = &image_info;

		device.updateDescriptorSets(descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
	}
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
