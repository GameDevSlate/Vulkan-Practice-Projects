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
	                               std::vector<Vertex> vertices);

private:
	static uint32_t FindMemoryType(uint32_t type_filter,
	                               vk::MemoryPropertyFlags properties,
	                               vk::PhysicalDevice physical_device);
};
