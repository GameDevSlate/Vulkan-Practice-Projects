#pragma once

#include<optional>
#include<vector>
#include <GLFW/glfw3.h>
#include<vulkan/vulkan.hpp>
#include"LogicalDevice.h"

class PhysicalDevice
{
public:
	inline static const std::vector<const char*> s_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		std::optional<uint32_t> presentFamily;

		[[nodiscard]] bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	static void PickPhysicalDevice(vk::PhysicalDevice& physical_device,
	                               vk::Instance instance,
	                               vk::SurfaceKHR surface,
	                               GLFWwindow* app_window);

	static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

	static void CreateCommandPool(vk::CommandPool& command_pool, vk::PhysicalDevice physical_device, vk::Device device);

private:
	// This is for finding the first suitable device (might not be the best)
	static bool IsDeviceSuitable(vk::PhysicalDevice device);

	// This is for finding the best suitable device out of many
	static unsigned RateDeviceSuitability(vk::PhysicalDevice device);

	static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);

	// Surface of the application being used
	inline static vk::SurfaceKHR m_appSurface;

	// The current window of the application being used
	inline static GLFWwindow* m_appWindow;
};
