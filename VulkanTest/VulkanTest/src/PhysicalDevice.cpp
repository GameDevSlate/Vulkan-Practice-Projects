#include "PhysicalDevice.h"
#include<map>
#include<set>

void PhysicalDevice::PickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface, vk::PhysicalDevice& physicalDevice)
{
	// Pass the window surface to the private member of this
	m_appSurface = surface;

	// Get the amount of physical devices available
	uint32_t deviceCount;
	instance.enumeratePhysicalDevices(&deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	// Create a vector that holds all physical devices and set its data
	std::vector<vk::PhysicalDevice> devices(deviceCount);

	instance.enumeratePhysicalDevices(&deviceCount, devices.data());

	// Use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, vk::PhysicalDevice> candidates;

	// Give a score to each possible device
	for (const auto& device : devices) {
			
		int score = RateDeviceSuitability(device);
		// Passing the score as an array proxy
		candidates.insert({ score, device });
	}

	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first > 0)
		physicalDevice = candidates.rbegin()->second;
	else
		throw std::runtime_error("Failed to find a suitable GPU!");
}

bool PhysicalDevice::IsDeviceSuitable(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensionSuppport(device);

	bool swapChainAdequate = false;
	
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);

		// Might use Demorgan's law here later
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

int PhysicalDevice::RateDeviceSuitability(vk::PhysicalDevice device)
{
	vk::PhysicalDeviceProperties deviceProperties;
	vk::PhysicalDeviceFeatures deviceFeatures;

	// Get the individual device properties and features
	device.getProperties(&deviceProperties);
	device.getFeatures(&deviceFeatures);

	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		score += 1000;

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Add to the score if it has a graphics queue family
	QueueFamilyIndices indices = FindQueueFamilies(device);

	if (indices.isComplete())
		score += indices.graphicsFamily.value();

	// Add to the score if this device has supported extensions
	bool extensionsSupported = CheckDeviceExtensionSuppport(device);

	score += extensionsSupported ? 500 : 0;

	// Add to the score if this device has supported swapchain capabilities
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);

		score += !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty() ? 500 : 0;
	}

	// Application can't function without geomerty shaders
	if (!deviceFeatures.geometryShader)
		return 0;

	return score;
}

bool PhysicalDevice::CheckDeviceExtensionSuppport(vk::PhysicalDevice device)
{
	uint32_t extensionCount;

	// Count the ammount of extensions and assign them to extensionCount
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);

	// Allocate the extensions to the vector
	device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

	// Create a set of required extensions from the available extensions
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}

// This is a mouthfull because the first PhysicalDevice is to indicate the class file,
// and the second is for the actual class object itself
PhysicalDevice::PhysicalDevice::QueueFamilyIndices PhysicalDevice::FindQueueFamilies(vk::PhysicalDevice device)
{
	QueueFamilyIndices indices;
	
	// Logic to find queue family indices to populate struct with
	uint32_t queueFamilyCount = 0;

	device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);

	device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		// Check that one of the queue families has the graphics bit flag (VK_QUEUE_GRAPHICS_BIT)
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			indices.graphicsFamily = i;

		// Check that one of the queue families can present (display in a window),
		// in case the queue is different from drawing
		vk::Bool32 presentSupport = false;
		device.getSurfaceSupportKHR(i, m_appSurface, &presentSupport);

		if (presentSupport)
			indices.presentFamily = i;

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}

PhysicalDevice::PhysicalDevice::SwapChainSupportDetails PhysicalDevice::QuerySwapChainSupport(vk::PhysicalDevice device)
{
	// Allocate the possible surface capabilites
	SwapChainSupportDetails details;
	device.getSurfaceCapabilitiesKHR(m_appSurface, &details.capabilities);

	// Query the supported surface formats
	uint32_t formatCount;
	device.getSurfaceFormatsKHR(m_appSurface, &formatCount, details.formats.data());

	// Query the supporeted present modes
	uint32_t presentModeCount;
	device.getSurfacePresentModesKHR(m_appSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		
		device.getSurfacePresentModesKHR(m_appSurface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

vk::SurfaceFormatKHR PhysicalDevice::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return availableFormat;

	// In case the sRGB color format cannot be found just pick the first one.
	return availableFormats[0];
}
