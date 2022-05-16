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
	//void PopulateMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);
	//void SetupDebugMessenger();
	//vk::Result CreateDebugUtilsMessengerEXT(	vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	//											const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger);

	//void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator);
	void MainLoop();
	std::vector<const char*> GetRequiredExtensions();
	//static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	//														vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
	//														const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	//														void* pUserData);
	void CleanUp();

	GLFWwindow* m_window;
	vk::Instance m_instance;
	vk::DebugUtilsMessengerEXT m_debugMessenger;
	vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
};