#pragma once

#include<vulkan/vulkan.hpp>
#include<optional>
#include"LogicalDevice.h"
class PhysicalDevice
{

public:
	static void PickPhysicalDevice(vk::Instance instance, vk::PhysicalDevice& physicalDevice);

private:

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		inline bool isComplete() {
			return graphicsFamily.has_value();
		}
	};

	// This is for finding the first suitable device (might not be the best)
	static bool IsDeviceSuitable(vk::PhysicalDevice device);
	// This is for finding the best sutiable device out of many
	static int RateDeviceSuitability(vk::PhysicalDevice device);
	
	static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

	friend void LogicalDevice::CreateLogicalDevice(	vk::Instance instance,
													vk::PhysicalDevice physicalDevice,
													vk::Device& device,
													vk::Queue& graphicsQueue);
};

