#pragma once

#include<optional>
#include<vector>
#include <GLFW/glfw3.h>
#include<vulkan/vulkan.hpp>
#include"LogicalDevice.h"

class PhysicalDevice
{
public:
	static void PickPhysicalDevice(vk::PhysicalDevice& physical_device,
	                               vk::Instance instance,
	                               vk::SurfaceKHR surface,
	                               GLFWwindow* app_window);

	static void CreateSwapChain(vk::SwapchainKHR& swap_chain,
	                            std::vector<vk::Image>& swap_chain_images,
	                            vk::PhysicalDevice physical_device,
	                            vk::Device device);

private:
	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;

		std::vector<vk::SurfaceFormatKHR> formats;

		std::vector<vk::PresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		std::optional<uint32_t> presentFamily;

		[[nodiscard]] bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	// This is for finding the first suitable device (might not be the best)
	static bool IsDeviceSuitable(vk::PhysicalDevice device);

	// This is for finding the best suitable device out of many
	static int RateDeviceSuitability(vk::PhysicalDevice device);

	static bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);

	static QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);

	static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);

	static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);

	static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);

	static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

	friend void LogicalDevice::CreateLogicalDevice(
		vk::Device& device,
		vk::PhysicalDevice physical_device,
		vk::Queue& graphics_queue,
		vk::Queue& present_queue);

	// Surface of the application being used
	inline static vk::SurfaceKHR m_appSurface;

	// The current window of the application being used
	inline static GLFWwindow* m_appWindow;

	inline static const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
