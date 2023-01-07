#define VULKAN_HPP_NO_CONSTRUCTORS

#include "PhysicalDevice.h"

#include<map>
#include<set>

#include "SwapChain.h"

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
		auto swapChainSupport = SwapChain::QuerySwapChainSupport(device, m_appSurface);

		// Might use Demorgan's law here later
		swap_chain_adequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	vk::PhysicalDeviceFeatures supported_features;
	device.getFeatures(&supported_features);

	return indices.IsComplete() && extensionsSupported && swap_chain_adequate && supported_features.samplerAnisotropy;
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
		auto [capabilities, formats, present_modes] = SwapChain::QuerySwapChainSupport(device, m_appSurface);

		score += !formats.empty() && !present_modes.empty() ? 500 : 0;
	}

	// Add to the score if this device has anisotropic filtering
	score += device_features.samplerAnisotropy ? 500 : 0;

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
