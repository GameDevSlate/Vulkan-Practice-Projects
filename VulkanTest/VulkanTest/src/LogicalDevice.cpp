#define VULKAN_HPP_NO_CONSTRUCTORS

#include "LogicalDevice.h"
#include"PhysicalDevice.h"
#include"ValidationLayers.h"

void LogicalDevice::CreateLogicalDevice(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device& device, vk::Queue& graphicsQueue)
{
	// Get the queue family indices
	PhysicalDevice::QueueFamilyIndices indices = PhysicalDevice::FindQueueFamilies(physicalDevice);

	// Make a Queue create info
	vk::DeviceQueueCreateInfo queueCreateInfo{	.queueFamilyIndex = indices.graphicsFamily.value(),
												.queueCount = 1};

	// Since we only have one queue, we'll set the priority to 1.0f
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	// Get the physical device features
	vk::PhysicalDeviceFeatures deviceFetures{};

	// Make the Logical Device create info
	vk::DeviceCreateInfo createInfo{	.queueCreateInfoCount = 1,
										.pQueueCreateInfos = &queueCreateInfo,
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
}
