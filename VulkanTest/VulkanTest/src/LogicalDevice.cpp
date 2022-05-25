#define VULKAN_HPP_NO_CONSTRUCTORS

#include "LogicalDevice.h"
#include"PhysicalDevice.h"
#include"ValidationLayers.h"
#include<set>

void LogicalDevice::CreateLogicalDevice(vk::Instance instance,
										vk::PhysicalDevice physicalDevice,
										vk::Device& device,
										vk::Queue& graphicsQueue,
										vk::Queue& presentQueue)
{
	// Get the queue family indices
	PhysicalDevice::QueueFamilyIndices indices = PhysicalDevice::FindQueueFamilies(physicalDevice);

	// Create a set of different queue create infos for drawing and presenting
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// Set the priority to 1.0f
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {

		vk::DeviceQueueCreateInfo queueCreateInfo{	.queueFamilyIndex = queueFamily,
													.queueCount = 1,
													.pQueuePriorities = &queuePriority};

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Get the physical device features
	vk::PhysicalDeviceFeatures deviceFetures{};

	// Make the Logical Device create info
	vk::DeviceCreateInfo createInfo{	.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
										.pQueueCreateInfos = queueCreateInfos.data(),
										.enabledExtensionCount = 0,
										.pEnabledFeatures = &deviceFetures};

	// If we are debbugging, then add the validation layers
	if (ValidationLayers::enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validation_layers.size());
		createInfo.ppEnabledLayerNames = ValidationLayers::validation_layers.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	// Create the Logical Device, and if there is an error, throw an exception
	if (physicalDevice.createDevice(&createInfo, nullptr, &device) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create Logical Device!");

	device.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue);
	device.getQueue(indices.presentFamily.value(), 0, &presentQueue);
}
