// Adding the Vulkan C++ header, and ignoring all warnings
// the are only "use enum class instead" warning.
#pragma warning(push, 0)
#include<vulkan/vulkan.hpp>
#pragma warning(pop)

#include<vector>
#include<GLFW/glfw3.h>

constexpr uint32_t WIDTH = 800;

constexpr uint32_t HEIGHT = 600;

#pragma once
class HelloTriangleApplication
{
public:
	void Run();

private:
	void InitWindow();

	void InitVulkan();

	void CreateInstance();

	void CreateSurface();

	void MainLoop() const;

	std::vector<const char*> GetRequiredExtensions();

	void CleanUp() const;

	GLFWwindow* m_window;

	vk::Instance m_instance;

	vk::DebugUtilsMessengerEXT m_debugMessenger;

	vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	vk::Device m_device;

	vk::Queue m_graphicsQueue;

	vk::SurfaceKHR m_surface;

	vk::Queue m_presentQueue;
};
