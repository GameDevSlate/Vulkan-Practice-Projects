#pragma once
#include<vector>
#include <vulkan/vulkan.hpp>

#include "Vertex.h"

class Buffer
{
public:
	static void CreateVertexBuffer(vk::Buffer& vertex_buffer,
	                               vk::DeviceMemory& vertex_buffer_memory,
	                               vk::Device device,
	                               vk::PhysicalDevice physical_device,
	                               const std::vector<Vertex>& vertices,
	                               vk::CommandPool command_pool,
	                               vk::Queue graphics_queue);

	static void CreateIndexBuffer(vk::Buffer& index_buffer,
	                              vk::DeviceMemory& index_buffer_memory,
	                              vk::Device device,
	                              vk::PhysicalDevice physical_device,
	                              const std::vector<uint16_t>& indices,
	                              vk::CommandPool command_pool,
	                              vk::Queue graphics_queue);

	static void CreateUniformBuffers(std::vector<vk::Buffer>& uniform_buffers,
	                                 std::vector<vk::DeviceMemory>& uniform_buffers_memory,
	                                 vk::Device device,
	                                 vk::PhysicalDevice physical_device);

	static void CreateBuffer(vk::Device device,
	                         vk::PhysicalDevice physical_device,
	                         vk::DeviceSize size,
	                         vk::BufferUsageFlags usage,
	                         vk::MemoryPropertyFlags properties,
	                         vk::Buffer& buffer,
	                         vk::DeviceMemory& buffer_memory);

private:
	static void CopyBuffer(vk::Device device,
	                       vk::CommandPool command_pool,
	                       vk::Queue graphics_queue,
	                       vk::Buffer src_buffer,
	                       vk::Buffer dst_buffer,
	                       vk::DeviceSize size);

	static uint32_t FindMemoryType(uint32_t type_filter,
	                               vk::MemoryPropertyFlags properties,
	                               vk::PhysicalDevice physical_device);

	friend class Texture;
};
