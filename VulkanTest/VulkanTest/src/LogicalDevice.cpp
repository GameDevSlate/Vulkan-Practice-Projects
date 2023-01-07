#define VULKAN_HPP_NO_CONSTRUCTORS

#include "LogicalDevice.h"
#include<set>
#include"PhysicalDevice.h"
#include"ValidationLayers.h"

void LogicalDevice::CreateLogicalDevice(
	vk::Device& device,
	vk::PhysicalDevice physical_device,
	vk::Queue& graphics_queue,
	vk::Queue& present_queue)
{
	// Get the queue family indices
	auto [index_graphics_family, index_present_family] = PhysicalDevice::FindQueueFamilies(physical_device);

	// Create a set of different queue create infos for drawing and presenting
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
	std::set unique_queue_families = {index_graphics_family.value(), index_present_family.value()};

	// Set the priority to 1.0f
	float queue_priority = 1.0f;
	for (uint32_t queue_family : unique_queue_families) {

		vk::DeviceQueueCreateInfo queue_create_info{
			.queueFamilyIndex = queue_family,
			.queueCount = 1,
			.pQueuePriorities = &queue_priority
		};

		queue_create_infos.push_back(queue_create_info);
	}

	// Get the physical device features
	vk::PhysicalDeviceFeatures device_features{
		.samplerAnisotropy = static_cast<vk::Bool32>(true)
	};

	// Make the Logical Device create info
	vk::DeviceCreateInfo create_info{
		.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
		.pQueueCreateInfos = queue_create_infos.data(),
		.enabledExtensionCount = static_cast<uint32_t>(PhysicalDevice::s_device_extensions.size()),
		.ppEnabledExtensionNames = PhysicalDevice::s_device_extensions.data(),
		.pEnabledFeatures = &device_features
	};

	// If we are debugging, then add the validation layers
	if (ValidationLayers::enable_validation_layers) {
		create_info.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validation_layers.size());
		create_info.ppEnabledLayerNames = ValidationLayers::validation_layers.data();
	} else
		create_info.enabledLayerCount = 0;

	// Create the Logical Device, and if there is an error, throw an exception
	if (physical_device.createDevice(&create_info, nullptr, &device) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create Logical Device!");

	device.getQueue(index_graphics_family.value(), 0, &graphics_queue);
	device.getQueue(index_present_family.value(), 0, &present_queue);
}
