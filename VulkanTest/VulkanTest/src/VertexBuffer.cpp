#define VULKAN_HPP_NO_CONSTRUCTORS

#include "VertexBuffer.h"

void VertexBuffer::CreateVertexBuffer(vk::Buffer& vertex_buffer,
                                      vk::DeviceMemory& vertex_buffer_memory,
                                      const vk::Device device,
                                      const vk::PhysicalDevice physical_device,
                                      const std::vector<Vertex> vertices)
{
	// Creating the buffer
	vk::BufferCreateInfo buffer_info{
		.size = sizeof(vertices[0]) * vertices.size(),
		.usage = vk::BufferUsageFlagBits::eVertexBuffer,
		.sharingMode = vk::SharingMode::eExclusive
	};

	if (device.createBuffer(&buffer_info, nullptr, &vertex_buffer) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create vertex buffer!");

	// Allocating memory to the buffer
	vk::MemoryRequirements mem_requirements;
	device.getBufferMemoryRequirements(vertex_buffer, &mem_requirements);

	vk::MemoryAllocateInfo alloc_info{
		.allocationSize = mem_requirements.size,
		.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits,
		                                  vk::MemoryPropertyFlagBits::eHostVisible |
		                                  vk::MemoryPropertyFlagBits::eHostCoherent, physical_device)
	};

	if (device.allocateMemory(&alloc_info, nullptr, &vertex_buffer_memory) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate vertex buffer memory!");

	device.bindBufferMemory(vertex_buffer, vertex_buffer_memory, 0);

	// Filling the vertex buffer
	void* data;

	device.mapMemory(vertex_buffer_memory, 0, buffer_info.size, {}, &data);
	memcpy(data, vertices.data(), buffer_info.size);
	device.unmapMemory(vertex_buffer_memory);
}

uint32_t VertexBuffer::FindMemoryType(const uint32_t type_filter,
                                      const vk::MemoryPropertyFlags properties,
                                      const vk::PhysicalDevice physical_device)
{
	vk::PhysicalDeviceMemoryProperties mem_properties{};
	physical_device.getMemoryProperties(&mem_properties);

	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		if (type_filter & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	throw std::runtime_error("Failed to find suitable memory type!");
}
