#pragma once
#include<vector>
#include <vulkan/vulkan.hpp>

#include "Vertex.h"

class VertexBuffer
{
public:
	static void CreateVertexBuffer(vk::Buffer& vertex_buffer,
	                               vk::DeviceMemory& vertex_buffer_memory,
	                               vk::Device device,
	                               vk::PhysicalDevice physical_device,
	                               std::vector<Vertex> vertices,
	                               vk::CommandPool command_pool,
	                               vk::Queue graphics_queue);

private:
	static void CreateBuffer(vk::Device device,
	                         vk::PhysicalDevice physical_device,
	                         vk::DeviceSize size,
	                         vk::BufferUsageFlags usage,
	                         vk::MemoryPropertyFlags properties,
	                         vk::Buffer& buffer,
	                         vk::DeviceMemory& buffer_memory);

	static void CopyBuffer(vk::Device device,
	                       vk::CommandPool command_pool,
	                       vk::Queue graphics_queue,
	                       vk::Buffer src_buffer,
	                       vk::Buffer dst_buffer,
	                       vk::DeviceSize size);

	static uint32_t FindMemoryType(uint32_t type_filter,
	                               vk::MemoryPropertyFlags properties,
	                               vk::PhysicalDevice physical_device);
};
