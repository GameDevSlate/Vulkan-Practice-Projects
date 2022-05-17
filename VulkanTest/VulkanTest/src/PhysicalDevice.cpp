#include "PhysicalDevice.h"
#include<vector>
#include<map>

void PhysicalDevice::PickPhysicalDevice(vk::Instance instance, vk::PhysicalDevice& physicalDevice)
{
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
	vk::PhysicalDeviceProperties deviceProperties;
	vk::PhysicalDeviceFeatures deviceFeatures;

	// Get the individual device properties and features
	device.getProperties(&deviceProperties);
	device.getFeatures(&deviceFeatures);

	return deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu && deviceFeatures.geometryShader;
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

	// Application can't function without geomerty shaders
	if (!deviceFeatures.geometryShader)
		return 0;

	return score;
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

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}
