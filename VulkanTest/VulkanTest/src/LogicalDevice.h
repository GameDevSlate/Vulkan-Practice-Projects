#pragma once
#include<vulkan/vulkan.hpp>

class LogicalDevice
{
public:
	static void CreateLogicalDevice(
		vk::Device& device,
		vk::PhysicalDevice physical_device,
		vk::Queue& graphics_queue,
		vk::Queue& present_queue);
};
