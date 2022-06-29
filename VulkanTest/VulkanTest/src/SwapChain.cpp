#define VULKAN_HPP_NO_CONSTRUCTORS

#include "SwapChain.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <GLFW/glfw3.h>

#include "PhysicalDevice.h"

/**
 * \brief Creates and allocates a swap chain, while also allocating its images, format, and extent.
 * \param swap_chain The swap chain in which its contents will be allocated.
 * \param images The array of images that the swap chain can hold.
 * \param format of the swap chain.
 * \param extent of the swap chain.
 * \param physical_device The GPU to be used.
 * \param device The logical device that will handle the creation of the new swap chain.
 * \param app_surface The window surface of the current application.
 * \param app_window The actual window of the current application.
 */
void SwapChain::CreateSwapChain(vk::SwapchainKHR& swap_chain,
                                std::vector<vk::Image>& images,
                                vk::Format& format,
                                vk::Extent2D& extent,
                                const vk::PhysicalDevice physical_device,
                                const vk::Device device,
                                vk::SurfaceKHR app_surface,
                                GLFWwindow* app_window)
{
	const auto [chain_capabilities, chain_formats, chain_present_modes] =
		QuerySwapChainSupport(physical_device, app_surface);

	vk::SurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(chain_formats);
	vk::PresentModeKHR present_mode = ChooseSwapPresentMode(chain_present_modes);
	vk::Extent2D swap_extent = ChooseSwapExtent(chain_capabilities, app_window);

	// Set the minimum amount of images that should be in the swap chain
	uint32_t image_count = chain_capabilities.minImageCount + 1;

	// Set the maximum amount of images that should be in the swap chain ---zero means no maximum
	if (chain_capabilities.maxImageCount > 0 && image_count > chain_capabilities.maxImageCount)
		image_count = chain_capabilities.maxImageCount;

	// Make the create info for the swap chain
	vk::SwapchainCreateInfoKHR create_info{
		.surface = app_surface,
		.minImageCount = image_count,
		.imageFormat = surface_format.format,
		.imageColorSpace = surface_format.colorSpace,
		.imageExtent = swap_extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment
	};

	const auto [indices_graphics_family, indices_present_family] = PhysicalDevice::FindQueueFamilies(physical_device);
	const uint32_t queue_family_indices[] = {indices_graphics_family.value(), indices_present_family.value()};

	if (indices_graphics_family != indices_present_family) {
		create_info.imageSharingMode = vk::SharingMode::eConcurrent;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	} else {
		create_info.imageSharingMode = vk::SharingMode::eExclusive;
		// Optional
		create_info.queueFamilyIndexCount = 0;
		// Optional
		create_info.pQueueFamilyIndices = nullptr;
	}

	create_info.preTransform = chain_capabilities.currentTransform;
	create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	create_info.presentMode = present_mode;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	if (device.createSwapchainKHR(&create_info, nullptr, &swap_chain) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create swap chain!");

	// Populate that amount of swap chain images
	device.getSwapchainImagesKHR(swap_chain, &image_count, nullptr);

	images.resize(image_count);

	device.getSwapchainImagesKHR(swap_chain, &image_count, images.data());

	// Lastly, set the swap chain image format and extent
	format = surface_format.format;
	extent = swap_extent;
}

/**
 * \brief Creates and allocates new image views from a swap chain.
 * \param image_views of the swap chain (to be allocated).
 * \param device The logical device that will handle the creation of the image views.
 * \param images of the swap chain.
 * \param image_format of the swap chain.
 */
void SwapChain::CreateImageViews(std::vector<vk::ImageView>& image_views,
                                 const vk::Device device,
                                 std::vector<vk::Image> images,
                                 vk::Format image_format)
{
	// Populate the amount of available swap chain image views
	// based on the amount of swap chain images
	image_views.resize(images.size());

	for (size_t i = 0; i < images.size(); i++) {

		vk::ImageViewCreateInfo create_info{
			.image = images[i],
			.viewType = vk::ImageViewType::e2D,
			.format = image_format,
			.components = {
				.r = vk::ComponentSwizzle::eIdentity,
				.g = vk::ComponentSwizzle::eIdentity,
				.b = vk::ComponentSwizzle::eIdentity,
				.a = vk::ComponentSwizzle::eIdentity
			},
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		if (device.createImageView(&create_info, nullptr, &image_views[i]) != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create image views");
	}
}

/**
 * \brief Creates frame buffers of a swap chain.
 * \param frame_buffers of a swap chain (to be allocated).
 * \param device The logical device that will handle the creation of the frame buffer.
 * \param image_views of the swap chain.
 * \param extent of the swap chain.
 * \param render_pass A render pass that holds the type of operations that will go through the buffer.
 */
void SwapChain::CreateFrameBuffers(std::vector<vk::Framebuffer>& frame_buffers,
                                   const vk::Device device,
                                   const std::vector<vk::ImageView> image_views,
                                   vk::Extent2D extent,
                                   vk::RenderPass render_pass)
{
	// Populate the amount of frame buffers
	// based on the amount of image views
	frame_buffers.resize(image_views.size());

	for (size_t i = 0; i < image_views.size(); i++) {
		vk::ImageView attachments[] = {image_views[i]};

		vk::FramebufferCreateInfo framebuffer_info{
			.renderPass = render_pass,
			.attachmentCount = 1,
			.pAttachments = attachments,
			.width = extent.width,
			.height = extent.height,
			.layers = 1
		};

		if (device.createFramebuffer(&framebuffer_info, nullptr, &frame_buffers[i]) != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create framebuffer!");
	}
}

SwapChain::SwapChainSupportDetails SwapChain::QuerySwapChainSupport(const vk::PhysicalDevice device,
                                                                    const vk::SurfaceKHR app_surface)
{
	// Allocate the possible surface capabilities
	SwapChainSupportDetails details;
	device.getSurfaceCapabilitiesKHR(app_surface, &details.capabilities);

	// Query the supported surface formats
	uint32_t format_count;
	device.getSurfaceFormatsKHR(app_surface, &format_count, nullptr);

	if (format_count != 0) {
		details.formats.resize(format_count);
		device.getSurfaceFormatsKHR(app_surface, &format_count, details.formats.data());
	}

	// Query the supported present modes
	uint32_t present_mode_count;
	device.getSurfacePresentModesKHR(app_surface, &present_mode_count, nullptr);

	if (present_mode_count != 0) {
		details.presentModes.resize(present_mode_count);

		device.getSurfacePresentModesKHR(app_surface, &present_mode_count, details.presentModes.data());
	}

	return details;
}

vk::SurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats)
{
	// Look for 8-bit per channel sRGB format, with a nonlinear color space
	for (const auto& available_format : available_formats)
		if (available_format.format == vk::Format::eB8G8R8A8Srgb && available_format.colorSpace ==
		    vk::ColorSpaceKHR::eSrgbNonlinear)
			return available_format;

	// In case the sRGB color format cannot be found, just pick the first one.
	return available_formats[0];
}

vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes)
{
	for (const auto& available_present_mode : available_present_modes)
		if (available_present_mode == vk::PresentModeKHR::eMailbox)
			return available_present_mode;

	// In case mailbox present mode cannot be found, pick FIFO, which is guaranteed
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* app_window)
{
	// Return the current extent of the window size if we are under the maximum size
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	// Otherwise, Clamp the current extent of the window

	int width, height;

	glfwGetFramebufferSize(app_window, &width, &height);

	vk::Extent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

	actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width,
	                                 capabilities.maxImageExtent.width);

	actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height,
	                                  capabilities.maxImageExtent.height);

	return actual_extent;
}
