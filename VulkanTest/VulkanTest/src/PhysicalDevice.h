#pragma once

#include<vulkan/vulkan.hpp>
class PhysicalDevice
{
public:
	static void PickPhysicalDevice(vk::Instance instance, vk::PhysicalDevice& physicalDevice);

private:
	// This is for finding the first suitable device (might not be the best)
	static bool IsDeviceSuitable(vk::PhysicalDevice device);
	// This is for finding the best sutiable device out of many
	static int RateDeviceSuitability(vk::PhysicalDevice device);
};

