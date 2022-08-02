#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Buffer.h"

#include "HelloTriangleApplication.h"
#include "UniformBufferObject.h"

void Buffer::CreateVertexBuffer(vk::Buffer& vertex_buffer,
                                vk::DeviceMemory& vertex_buffer_memory,
                                const vk::Device device,
                                const vk::PhysicalDevice physical_device,
                                const std::vector<Vertex> vertices,
                                const vk::CommandPool command_pool,
                                const vk::Queue graphics_queue)
{
	vk::DeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

	vk::Buffer staging_buffer;

	vk::DeviceMemory staging_buffer_memory;

	CreateBuffer(device, physical_device, buffer_size, vk::BufferUsageFlagBits::eTransferSrc,
	             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging_buffer,
	             staging_buffer_memory);

	// Filling the vertex buffer
	void* data;

	device.mapMemory(staging_buffer_memory, 0, buffer_size, {}, &data);
	memcpy(data, vertices.data(), buffer_size);
	device.unmapMemory(staging_buffer_memory);

	CreateBuffer(device, physical_device, buffer_size,
	             vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
	             vk::MemoryPropertyFlagBits::eDeviceLocal, vertex_buffer, vertex_buffer_memory);

	CopyBuffer(device, command_pool, graphics_queue, staging_buffer, vertex_buffer, buffer_size);

	// Clean up temporary buffers
	device.destroyBuffer(staging_buffer, nullptr);
	device.freeMemory(staging_buffer_memory, nullptr);
}

void Buffer::CreateIndexBuffer(vk::Buffer& index_buffer,
                               vk::DeviceMemory& index_buffer_memory,
                               const vk::Device device,
                               const vk::PhysicalDevice physical_device,
                               const std::vector<uint16_t> indices,
                               const vk::CommandPool command_pool,
                               const vk::Queue graphics_queue)
{
	vk::DeviceSize buffer_size = sizeof(indices[0]) * indices.size();

	vk::Buffer staging_buffer;
	vk::DeviceMemory staging_buffer_memory;

	CreateBuffer(device, physical_device, buffer_size, vk::BufferUsageFlagBits::eTransferSrc,
	             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging_buffer,
	             staging_buffer_memory);

	void* data;
	device.mapMemory(staging_buffer_memory, 0, buffer_size, {}, &data);
	memcpy(data, indices.data(), buffer_size);
	device.unmapMemory(staging_buffer_memory);

	CreateBuffer(device, physical_device, buffer_size,
	             vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
	             vk::MemoryPropertyFlagBits::eDeviceLocal, index_buffer, index_buffer_memory);

	CopyBuffer(device, command_pool, graphics_queue, staging_buffer, index_buffer, buffer_size);

	device.destroyBuffer(staging_buffer, nullptr);
	device.freeMemory(staging_buffer_memory, nullptr);
}

void Buffer::CreateUniformBuffers(std::vector<vk::Buffer>& uniform_buffers,
                                  std::vector<vk::DeviceMemory>& uniform_buffers_memory,
                                  const vk::Device device,
                                  const vk::PhysicalDevice physical_device)
{
	vk::DeviceSize buffer_size = sizeof(UniformBufferObject);

	uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		CreateBuffer(device, physical_device, buffer_size, vk::BufferUsageFlagBits::eUniformBuffer,
		             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		             uniform_buffers[i], uniform_buffers_memory[i]);
}

void Buffer::CreateBuffer(const vk::Device device,
                          const vk::PhysicalDevice physical_device,
                          vk::DeviceSize size,
                          vk::BufferUsageFlags usage,
                          const vk::MemoryPropertyFlags properties,
                          vk::Buffer& buffer,
                          vk::DeviceMemory& buffer_memory)
{
	vk::BufferCreateInfo buffer_info{
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive
	};

	if (device.createBuffer(&buffer_info, nullptr, &buffer) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create buffer!");

	vk::MemoryRequirements mem_requirements;
	device.getBufferMemoryRequirements(buffer, &mem_requirements);

	vk::MemoryAllocateInfo alloc_info{
		.allocationSize = mem_requirements.size,
		.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties, physical_device)
	};

	if (device.allocateMemory(&alloc_info, nullptr, &buffer_memory) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate buffer memory!");

	device.bindBufferMemory(buffer, buffer_memory, 0);
}

void Buffer::CopyBuffer(const vk::Device device,
                        vk::CommandPool command_pool,
                        const vk::Queue graphics_queue,
                        const vk::Buffer src_buffer,
                        const vk::Buffer dst_buffer,
                        vk::DeviceSize size)
{
	vk::CommandBufferAllocateInfo alloc_info{
		.commandPool = command_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	vk::CommandBuffer command_buffer;
	device.allocateCommandBuffers(&alloc_info, &command_buffer);

	// Start recording command buffer
	vk::CommandBufferBeginInfo begin_info{
		.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
	};

	command_buffer.begin(&begin_info);

	vk::BufferCopy copy_region{
		// Optional
		.srcOffset = 0,
		// Optional
		.dstOffset = 0,
		.size = size
	};

	command_buffer.copyBuffer(src_buffer, dst_buffer, 1, &copy_region);

	command_buffer.end();

	vk::SubmitInfo submit_info{
		.commandBufferCount = 1,
		.pCommandBuffers = &command_buffer
	};

	graphics_queue.submit(1, &submit_info, VK_NULL_HANDLE);
	graphics_queue.waitIdle();

	device.freeCommandBuffers(command_pool, 1, &command_buffer);
}

uint32_t Buffer::FindMemoryType(const uint32_t type_filter,
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
