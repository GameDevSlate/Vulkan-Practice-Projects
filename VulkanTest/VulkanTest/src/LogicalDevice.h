#pragma once
#include<vulkan/vulkan.hpp>

class LogicalDevice
{
public:
	static void CreateLogicalDevice(vk::Instance instance,
									vk::PhysicalDevice physicalDevice,
									vk::Device& device,
									vk::Queue& graphicsQueue,
									vk::Queue& presentQueue);
};

