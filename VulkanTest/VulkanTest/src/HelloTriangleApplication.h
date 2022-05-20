// Adding the Vulkan C++ header, and ignoring all warnings
// the are only "use enum class instead" warning.
#pragma warning(push, 0)
#include<vulkan/vulkan.hpp>
#pragma warning(pop)

#include<GLFW/glfw3.h>
#include<vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#pragma once
class HelloTriangleApplication
{
public:
	void Run();

private:
	void InitWindow();
	void InitVulkan();
	void CreateInstance();
	void MainLoop();
	std::vector<const char*> GetRequiredExtensions();
	void CleanUp();

	GLFWwindow* m_window;
	vk::Instance m_instance;
	vk::DebugUtilsMessengerEXT m_debugMessenger;
	vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	vk::Device m_device;
	vk::Queue m_graphicsQueue;
};