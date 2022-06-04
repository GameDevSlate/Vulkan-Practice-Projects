#pragma once

#include<optional>
#include<vector>
#include<vulkan/vulkan.hpp>
#include"LogicalDevice.h"

class PhysicalDevice
{
public:
	static void PickPhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface, vk::PhysicalDevice& physical_device);

private:
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		std::optional<uint32_t> presentFamily;

		[[nodiscard]] bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;

		std::vector<vk::SurfaceFormatKHR> formats;

		std::vector<vk::PresentModeKHR> presentModes;
	};

	// This is for finding the first suitable device (might not be the best)
	static bool IsDeviceSuitable(vk::PhysicalDevice device);

	// This is for finding the best suitable device out of many
	static int RateDeviceSuitability(vk::PhysicalDevice device);

	static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);

	static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

	static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);

	static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);

	friend void LogicalDevice::CreateLogicalDevice(
		vk::PhysicalDevice physical_device,
		vk::Device& device,
		vk::Queue& graphics_queue,
		vk::Queue& present_queue);

	inline static vk::SurfaceKHR m_appSurface;

	inline static const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
