#pragma once
#include <concepts>
#include <type_traits>
#include<vector>
#include <vulkan/vulkan.hpp>

#include "Vertex.h"

template < typename T >
concept vertex_or_index = std::same_as<T, Vertex> || std::same_as<T, uint16_t>;

class Buffer
{
public:
	static void CreateUniformBuffers(std::vector<vk::Buffer>& uniform_buffers,
	                                 std::vector<vk::DeviceMemory>& uniform_buffers_memory,
	                                 vk::Device device,
	                                 vk::PhysicalDevice physical_device);

	template < vertex_or_index T >
	static void CreatePrimitiveBuffer(vk::Buffer& primitive_buffer,
	                                  vk::DeviceMemory& buffer_memory,
	                                  vk::Device device,
	                                  vk::PhysicalDevice physical_device,
	                                  const std::vector<T>& primitives,
	                                  vk::CommandPool command_pool,
	                                  vk::Queue graphics_queue);

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

	static vk::CommandBuffer BeginSingleTimeCommands(vk::Device device, vk::CommandPool command_pool);

	static void EndSingleTimeCommands(vk::Device device,
	                                  vk::CommandPool command_pool,
	                                  const vk::Queue& queue,
	                                  vk::CommandBuffer command_buffer);

	static void CopyBufferToImage(vk::Device device,
	                              vk::CommandPool command_pool,
	                              const vk::Queue& queue,
	                              vk::Buffer buffer,
	                              vk::Image image,
	                              uint32_t width,
	                              uint32_t height);

	friend class Texture;
};

/**
 * \brief Creates a buffer for a two kinds of primitive inputs: Vertices or Indices
 * \tparam T Either a Vertex object, or a uint16_t.
 * \param primitive_buffer The buffer where its data will be written to.
 * \param buffer_memory The type of memory that the buffer will have.
 * \param device The logical device that handles the creation of the buffer, and allocation of its memory.
 * \param physical_device The device that will do graphics calculations(GPU, integrated graphics) used to find the memory capacity.
 * \param primitives The array of the primitives.
 * \param command_pool The command pool used to create command buffers for the operations copying from one buffer to another.
 * \param graphics_queue The queue family utilized to perform the commands from the command pool.
 */
template < vertex_or_index T >
void Buffer::CreatePrimitiveBuffer(vk::Buffer& primitive_buffer,
                                   vk::DeviceMemory& buffer_memory,
                                   const vk::Device device,
                                   const vk::PhysicalDevice physical_device,
                                   const std::vector<T>& primitives,
                                   const vk::CommandPool command_pool,
                                   const vk::Queue graphics_queue)
{
	vk::DeviceSize buffer_size = sizeof(primitives[0]) * primitives.size();

	vk::Buffer staging_buffer;

	vk::DeviceMemory staging_buffer_memory;

	CreateBuffer(device, physical_device, buffer_size, vk::BufferUsageFlagBits::eTransferSrc,
	             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, staging_buffer,
	             staging_buffer_memory);

	// Filling the vertex buffer
	void* data;

	device.mapMemory(staging_buffer_memory, 0, buffer_size, {}, &data);
	memcpy(data, primitives.data(), buffer_size);
	device.unmapMemory(staging_buffer_memory);

	vk::BufferUsageFlagBits type_flag;

	if (std::is_same_v<std::decay_t<decltype(primitives[0])>, Vertex>)
		type_flag = vk::BufferUsageFlagBits::eVertexBuffer;
	else
		type_flag = vk::BufferUsageFlagBits::eIndexBuffer;

	CreateBuffer(device, physical_device, buffer_size,
	             vk::BufferUsageFlagBits::eTransferDst | type_flag,
	             vk::MemoryPropertyFlagBits::eDeviceLocal, primitive_buffer, buffer_memory);

	CopyBuffer(device, command_pool, graphics_queue, staging_buffer, primitive_buffer, buffer_size);

	// Clean up temporary buffers
	device.destroyBuffer(staging_buffer, nullptr);
	device.freeMemory(staging_buffer_memory, nullptr);
}
