// Adding the Vulkan C++ header, and ignoring all warnings
// the are only "use enum class instead" warning.
#pragma warning(push, 0)
#include<vulkan/vulkan.hpp>
#pragma warning(pop)

#include<GLFW/glfw3.h>

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
	void CleanUp();

	GLFWwindow* m_window;
	vk::Instance m_instance;
};