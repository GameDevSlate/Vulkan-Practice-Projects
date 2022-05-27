#pragma once

#include<vulkan/vulkan.hpp>
#include<optional>
#include<vector>
#include"LogicalDevice.h"
class PhysicalDevice
{

public:
	static void PickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface, vk::PhysicalDevice& physicalDevice);

private:

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		inline bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	// This is for finding the first suitable device (might not be the best)
	static bool IsDeviceSuitable(vk::PhysicalDevice device);
	// This is for finding the best sutiable device out of many
	static int RateDeviceSuitability(vk::PhysicalDevice device);

	static bool CheckDeviceExtensionSuppport(vk::PhysicalDevice device);
	
	static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

	static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);

	static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

	friend void LogicalDevice::CreateLogicalDevice(	vk::Instance instance,
													vk::PhysicalDevice physicalDevice,
													vk::Device& device,
													vk::Queue& graphicsQueue,
													vk::Queue& presentQueue);

	inline static vk::SurfaceKHR m_appSurface;

	inline static const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

