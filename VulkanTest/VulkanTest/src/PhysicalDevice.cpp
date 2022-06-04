#include "PhysicalDevice.h"
#include<map>
#include<set>

void PhysicalDevice::PickPhysicalDevice(vk::Instance instance,
                                        vk::SurfaceKHR surface,
                                        vk::PhysicalDevice& physical_device)
{
	// Pass the window surface to the private member of this
	m_appSurface = surface;

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

		int score = RateDeviceSuitability(device);
		// Passing the score as an array proxy
		candidates.insert({score, device});
	}

	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first > 0)
		physical_device = candidates.rbegin()->second;
	else
		throw std::runtime_error("Failed to find a suitable GPU!");
}

bool PhysicalDevice::IsDeviceSuitable(const vk::PhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);

		// Might use Demorgan's law here later
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

int PhysicalDevice::RateDeviceSuitability(vk::PhysicalDevice device)
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
	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	score += extensionsSupported ? 500 : 0;

	// Add to the score if this device has supported swapchain capabilities
	if (extensionsSupported) {
		auto [capabilities, formats, present_modes] = QuerySwapChainSupport(device);

		score += !formats.empty() && !present_modes.empty() ? 500 : 0;
	}

	// Application can't function without geomerty shaders
	if (!device_features.geometryShader)
		return 0;

	return score;
}

bool PhysicalDevice::CheckDeviceExtensionSupport(const vk::PhysicalDevice device)
{
	uint32_t extensionCount;

	// Count the ammount of extensions and assign them to extensionCount
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);

	// Allocate the extensions to the vector
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

	// Create a set of required extensions from the available extensions
	std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

// This is a mouthfull because the first PhysicalDevice is to indicate the class file,
// and the second is for the actual class object itself
PhysicalDevice::QueueFamilyIndices PhysicalDevice::FindQueueFamilies(const vk::PhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Logic to find queue family indices to populate struct with
	uint32_t queueFamilyCount = 0;

	device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

	std::vector<vk::QueueFamilyProperties> queue_families(queueFamilyCount);

	device.getQueueFamilyProperties(&queueFamilyCount, queue_families.data());

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
	// Allocate the possible surface capabilites
	SwapChainSupportDetails details;
	device.getSurfaceCapabilitiesKHR(m_appSurface, &details.capabilities);

	// Query the supported surface formats
	uint32_t format_count;
	device.getSurfaceFormatsKHR(m_appSurface, &format_count, details.formats.data());

	// Query the supporeted present modes
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
	for (const auto& available_format : available_formats)
		if (available_format.format == vk::Format::eB8G8R8A8Srgb && available_format.colorSpace ==
		    vk::ColorSpaceKHR::eSrgbNonlinear)
			return available_format;

	// In case the sRGB color format cannot be found just pick the first one.
	return available_formats[0];
}
