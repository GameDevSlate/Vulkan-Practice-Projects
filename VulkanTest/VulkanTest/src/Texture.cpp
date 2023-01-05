#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Texture.h"
#include "Buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& file_path,
                 const vk::Device device,
                 const vk::PhysicalDevice physical_device,
                 const vk::CommandPool command_pool,
                 const vk::Queue& queue)
{
	int tex_width, tex_height, tex_channels;

	std::string cached_root_path = "assets/textures/" + file_path;

	m_filePath = cached_root_path.c_str();

	stbi_uc* pixels = stbi_load(m_filePath, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

	vk::DeviceSize image_size = tex_width * tex_height * 4;

	if (!pixels)
		throw std::runtime_error("Failed to load texture image!");

	vk::Buffer staging_buffer;
	vk::DeviceMemory staging_buffer_memory;

	Buffer::CreateBuffer(device, physical_device, image_size, vk::BufferUsageFlagBits::eTransferSrc,
	                     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	                     staging_buffer, staging_buffer_memory);

	void* data;
	device.mapMemory(staging_buffer_memory, 0, image_size, {}, &data);
	memcpy(data, pixels, image_size);
	device.unmapMemory(staging_buffer_memory);

	stbi_image_free(pixels);

	CreateImage(device, physical_device, tex_width, tex_height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
	            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
	            vk::MemoryPropertyFlagBits::eDeviceLocal, m_textureImage, m_textureImageMemory);

	TransitionImageLayout(device, command_pool, queue, m_textureImage, vk::Format::eR8G8B8A8Srgb,
	                      vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

	Buffer::CopyBufferToImage(device, command_pool, queue, staging_buffer, m_textureImage,
	                          static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));

	TransitionImageLayout(device, command_pool, queue, m_textureImage, vk::Format::eR8G8B8A8Srgb,
	                      vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

	device.destroyBuffer(staging_buffer, nullptr);
	device.freeMemory(staging_buffer_memory, nullptr);
}

void Texture::Destroy(const vk::Device device) const
{
	device.destroyImage(m_textureImage, nullptr);
	device.freeMemory(m_textureImageMemory, nullptr);
}

void Texture::CreateImage(const vk::Device device,
                          const vk::PhysicalDevice physical_device,
                          uint32_t width,
                          uint32_t height,
                          vk::Format format,
                          vk::ImageTiling tiling,
                          vk::ImageUsageFlags usage,
                          const vk::MemoryPropertyFlags properties,
                          vk::Image& image,
                          vk::DeviceMemory& image_memory)
{
	vk::ImageCreateInfo image_info{
		// Optional
		.flags = {},
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent{
			.width = width,
			.height = height,
			.depth = 1
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	if (device.createImage(&image_info, nullptr, &image) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create image!");

	vk::MemoryRequirements mem_requirements;
	device.getImageMemoryRequirements(image, &mem_requirements);

	vk::MemoryAllocateInfo alloc_info{
		.allocationSize = mem_requirements.size,
		.memoryTypeIndex = Buffer::FindMemoryType(mem_requirements.memoryTypeBits,
		                                          properties, physical_device)
	};

	if (device.allocateMemory(&alloc_info, nullptr, &image_memory) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate image memory!");

	device.bindImageMemory(image, image_memory, 0);
}

void Texture::TransitionImageLayout(const vk::Device device,
                                    const vk::CommandPool command_pool,
                                    const vk::Queue& graphics_queue,
                                    vk::Image image,
                                    vk::Format format,
                                    vk::ImageLayout old_layout,
                                    vk::ImageLayout new_layout)
{
	vk::CommandBuffer command_buffer = Buffer::BeginSingleTimeCommands(device, command_pool);

	vk::ImageMemoryBarrier barrier{
		.srcAccessMask = vk::AccessFlagBits::eNone,
		.dstAccessMask = vk::AccessFlagBits::eNone,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
	};

	vk::PipelineStageFlags source_stage, destination_stage;

	if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
		destination_stage = vk::PipelineStageFlagBits::eTransfer;

	} else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout ==
	           vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		source_stage = vk::PipelineStageFlagBits::eTransfer;
		destination_stage = vk::PipelineStageFlagBits::eFragmentShader;

	} else
		throw std::invalid_argument("Unsupported layout transition!");

	command_buffer.pipelineBarrier(source_stage,
	                               destination_stage,
	                               vk::DependencyFlagBits::eByRegion,
	                               0, nullptr, 0, nullptr, 1, &barrier);

	Buffer::EndSingleTimeCommands(device, command_pool, graphics_queue, command_buffer);
}
