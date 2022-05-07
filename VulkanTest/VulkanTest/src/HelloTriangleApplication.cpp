#define VULKAN_HPP_NO_CONSTRUCTORS

#include "HelloTriangleApplication.h"
#include<iostream>
#include<vector>

void HelloTriangleApplication::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	CleanUp();
}

void HelloTriangleApplication::InitWindow()
{
	// Initialize GLFW
	glfwInit();

	// Set up GLFW to not use OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Disable window resizing for now
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::InitVulkan()
{
	CreateInstance();
}

void HelloTriangleApplication::CreateInstance()
{
	// Creating the application info
	vk::ApplicationInfo applicationInfo{	.sType = vk::StructureType::eApplicationInfo,
											.pApplicationName = "Hello Triangle",
											.applicationVersion = VK_MAKE_VERSION(1,0,0),
											.pEngineName = "No Engine",
											.engineVersion = VK_MAKE_VERSION(1, 0, 0),
											.apiVersion = VK_API_VERSION_1_3};
	
	// Creating the information to create an instance
	vk::InstanceCreateInfo createInfo{	.sType = vk::StructureType::eInstanceCreateInfo,
										.pApplicationInfo = &applicationInfo};

	// Setting extenstions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	// Validation layers
	createInfo.enabledLayerCount = 0;
	
	if (vk::createInstance(&createInfo, nullptr, &m_instance) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create instance!");

	// Vulkan extensions
	uint32_t extensionCount = 0;
	vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<vk::ExtensionProperties> extensions(extensionCount);

	vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "Available extensions:\n";

	for (const auto& extension : extensions)
		std::cout << '\t' << extension.extensionName << '\n';
}

void HelloTriangleApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
	}
}

void HelloTriangleApplication::CleanUp()
{
	// Destroy Vulkan instance
	m_instance.destroy();

	// Destroy the window pointer
	glfwDestroyWindow(m_window);

	// Finish using GLFW
	glfwTerminate();
}
