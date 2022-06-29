#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

class SwapChain
{
public:
	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;

		std::vector<vk::SurfaceFormatKHR> formats;

		std::vector<vk::PresentModeKHR> presentModes;
	};

	static void CreateSwapChain(vk::SwapchainKHR& swap_chain,
	                            std::vector<vk::Image>& images,
	                            vk::Format& format,
	                            vk::Extent2D& extent,
	                            vk::PhysicalDevice physical_device,
	                            vk::Device device,
	                            vk::SurfaceKHR app_surface,
	                            GLFWwindow* app_window);

	static void CreateImageViews(std::vector<vk::ImageView>& image_views,
	                             vk::Device device,
	                             std::vector<vk::Image> images,
	                             vk::Format image_format);

	static void CreateFrameBuffers(std::vector<vk::Framebuffer>& frame_buffers,
	                               vk::Device device,
	                               std::vector<vk::ImageView> image_views,
	                               vk::Extent2D extent,
	                               vk::RenderPass render_pass);

	static SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR app_surface);

private:
	static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);

	static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);

	static vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* app_window);
};
