#define VULKAN_HPP_NO_CONSTRUCTORS

#include "PhysicalDevice.h"
#include <algorithm> // Necessary for std::clamp
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include<map>
#include<set>

void PhysicalDevice::PickPhysicalDevice(vk::PhysicalDevice& physical_device,
                                        const vk::Instance instance,
                                        const vk::SurfaceKHR surface,
                                        GLFWwindow* app_window)
{
	// Pass the window surface to the private member of this
	m_appSurface = surface;

	// Pass the actual window to the member of this
	m_appWindow = app_window;

	// Get the amount of physical devices available
	uint32_t device_count;
	instance.enumeratePhysicalDevices(&device_count, nullptr);

	if (device_count == 0)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	// Create a vector that holds all physical devices and set its data
	std::vector<vk::PhysicalDevice> devices(device_count);

	instance.enumeratePhysicalDevices(&device_count, devices.data());

	// Use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, vk::PhysicalDevice> candidates;

	// Give a score to each possible device
	for (const auto& device : devices) {

		unsigned int score = RateDeviceSuitability(device);
		// Passing the score as an array proxy
		candidates.insert({score, device});
	}

	// Check if the best candidate is suitable at all.
	// rbegin() because highest score is the last in the multimap
	if (candidates.rbegin()->first > 0)
		physical_device = candidates.rbegin()->second;
	else
		throw std::runtime_error("Failed to find a suitable GPU!");
}

void PhysicalDevice::CreateSwapChain(vk::SwapchainKHR& swap_chain,
                                     std::vector<vk::Image>& swap_chain_images,
                                     vk::Format& swap_chain_format,
                                     vk::Extent2D& swap_chain_extent,
                                     const vk::PhysicalDevice physical_device,
                                     const vk::Device device)
{
	const auto [chain_capabilities, chain_formats, chain_present_modes] = QuerySwapChainSupport(physical_device);

	vk::SurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(chain_formats);
	vk::PresentModeKHR present_mode = ChooseSwapPresentMode(chain_present_modes);
	vk::Extent2D extent = ChooseSwapExtent(chain_capabilities);

	// Set the minimum amount of images that should be in the swap chain
	uint32_t image_count = chain_capabilities.minImageCount + 1;

	// Set the maximum amount of images that should be in the swap chain ---zero means no maximum
	if (chain_capabilities.maxImageCount > 0 && image_count > chain_capabilities.maxImageCount)
		image_count = chain_capabilities.maxImageCount;

	// Make the create info for the swap chain
	vk::SwapchainCreateInfoKHR create_info{
		.surface = m_appSurface,
		.minImageCount = image_count,
		.imageFormat = surface_format.format,
		.imageColorSpace = surface_format.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment
	};

	const auto [indices_graphics_family, indices_present_family] = FindQueueFamilies(physical_device);
	const uint32_t queue_family_indices[] = {indices_graphics_family.value(), indices_present_family.value()};

	if (indices_graphics_family != indices_present_family) {
		create_info.imageSharingMode = vk::SharingMode::eConcurrent;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	} else {
		create_info.imageSharingMode = vk::SharingMode::eExclusive;
		create_info.queueFamilyIndexCount = 0;     // Optional
		create_info.pQueueFamilyIndices = nullptr; // Optional
	}

	create_info.preTransform = chain_capabilities.currentTransform;
	create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	create_info.presentMode = present_mode;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	if (device.createSwapchainKHR(&create_info, nullptr, &swap_chain) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create swap chain!");

	// Populate that amount of swap chain images
	device.getSwapchainImagesKHR(swap_chain, &image_count, nullptr);

	swap_chain_images.resize(image_count);

	device.getSwapchainImagesKHR(swap_chain, &image_count, swap_chain_images.data());

	// Lastly, set the swap chain image format and extent
	swap_chain_format = surface_format.format;
	swap_chain_extent = extent;
}

void PhysicalDevice::CreateCommandPool(vk::CommandPool& command_pool,
                                       const vk::PhysicalDevice physical_device,
                                       const vk::Device device)
{
	QueueFamilyIndices queue_family_indices = FindQueueFamilies(physical_device);

	vk::CommandPoolCreateInfo pool_info{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queue_family_indices.graphicsFamily.value()
	};

	if (device.createCommandPool(&pool_info, nullptr, &command_pool) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create command pool!");
}

bool PhysicalDevice::IsDeviceSuitable(const vk::PhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swap_chain_adequate = false;

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);

		// Might use Demorgan's law here later
		swap_chain_adequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swap_chain_adequate;
}

unsigned PhysicalDevice::RateDeviceSuitability(vk::PhysicalDevice device)
{
	vk::PhysicalDeviceProperties device_properties;
	vk::PhysicalDeviceFeatures device_features;

	// Get the individual device properties and features
	device.getProperties(&device_properties);
	device.getFeatures(&device_features);

	unsigned int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		score += 1000;

	// Maximum possible size of textures affects graphics quality
	score += device_properties.limits.maxImageDimension2D;

	// Add to the score if it has a graphics queue family
	QueueFamilyIndices indices = FindQueueFamilies(device);

	if (indices.IsComplete())
		score += indices.graphicsFamily.value();

	// Add to the score if this device has supported extensions
	bool extensions_supported = CheckDeviceExtensionSupport(device);

	score += extensions_supported ? 500 : 0;

	// Add to the score if this device has supported swap chain capabilities
	if (extensions_supported) {
		auto [capabilities, formats, present_modes] = QuerySwapChainSupport(device);

		score += !formats.empty() && !present_modes.empty() ? 500 : 0;
	}

	// Application can't function without geometry shaders
	if (!device_features.geometryShader)
		return 0;

	return score;
}

bool PhysicalDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice device)
{
	uint32_t extension_count;

	// Count the amount of extensions and assign them to extensionCount
	device.enumerateDeviceExtensionProperties(nullptr, &extension_count, nullptr);

	std::vector<vk::ExtensionProperties> available_extensions(extension_count);

	// Allocate the extensions to the vector
	device.enumerateDeviceExtensionProperties(nullptr, &extension_count, available_extensions.data());

	// Create a set of required extensions from the available extensions
	std::set<std::string> required_extensions(s_device_extensions.begin(), s_device_extensions.end());

	// Discard all unnecessary extensions
	for (const auto& extension : available_extensions)
		required_extensions.erase(extension.extensionName);

	return required_extensions.empty();
}

// This is a mouthful because the first PhysicalDevice is to indicate the class file,
// and the second is for the actual class object itself
PhysicalDevice::QueueFamilyIndices PhysicalDevice::FindQueueFamilies(const vk::PhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Logic to find queue family indices to populate struct with
	uint32_t queue_family_count = 0;

	device.getQueueFamilyProperties(&queue_family_count, nullptr);

	std::vector<vk::QueueFamilyProperties> queue_families(queue_family_count);

	device.getQueueFamilyProperties(&queue_family_count, queue_families.data());

	int i = 0;
	for (const auto& queue_family : queue_families) {
		// Check that one of the queue families has the graphics bit flag (VK_QUEUE_GRAPHICS_BIT)
		if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
			indices.graphicsFamily = i;

		// Check that one of the queue families can present (display in a window),
		// in case the queue is different from drawing
		vk::Bool32 present_support = false;
		device.getSurfaceSupportKHR(i, m_appSurface, &present_support);

		if (present_support)
			indices.presentFamily = i;

		if (indices.IsComplete())
			break;

		i++;
	}

	return indices;
}

PhysicalDevice::SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(const vk::PhysicalDevice device)
{
	// Allocate the possible surface capabilities
	SwapChainSupportDetails details;
	device.getSurfaceCapabilitiesKHR(m_appSurface, &details.capabilities);

	// Query the supported surface formats
	uint32_t format_count;
	device.getSurfaceFormatsKHR(m_appSurface, &format_count, nullptr);

	if (format_count != 0) {
		details.formats.resize(format_count);
		device.getSurfaceFormatsKHR(m_appSurface, &format_count, details.formats.data());
	}

	// Query the supported present modes
	uint32_t present_mode_count;
	device.getSurfacePresentModesKHR(m_appSurface, &present_mode_count, nullptr);

	if (present_mode_count != 0) {
		details.presentModes.resize(present_mode_count);

		device.getSurfacePresentModesKHR(m_appSurface, &present_mode_count, details.presentModes.data());
	}

	return details;
}

vk::SurfaceFormatKHR PhysicalDevice::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats)
{
	// Look for 8-bit per channel sRGB format, with a nonlinear color space
	for (const auto& available_format : available_formats)
		if (available_format.format == vk::Format::eB8G8R8A8Srgb && available_format.colorSpace ==
		    vk::ColorSpaceKHR::eSrgbNonlinear)
			return available_format;

	// In case the sRGB color format cannot be found, just pick the first one.
	return available_formats[0];
}

vk::PresentModeKHR PhysicalDevice::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes)
{
	for (const auto& available_present_mode : available_present_modes)
		if (available_present_mode == vk::PresentModeKHR::eMailbox)
			return available_present_mode;

	// In case we cannot find the mailbox present mode, pick FIFO, which is guaranteed
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D PhysicalDevice::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	// Return the current extent of the window size if we are under the maximum size
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;

	// Otherwise, Clamp the current extent of the window

	int width, height;

	glfwGetFramebufferSize(m_appWindow, &width, &height);

	vk::Extent2D actual_extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

	actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width,
	                                 capabilities.maxImageExtent.width);

	actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height,
	                                  capabilities.maxImageExtent.height);

	return actual_extent;
}
