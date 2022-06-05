#define VULKAN_HPP_NO_CONSTRUCTORS

#include "HelloTriangleApplication.h"
#include<iostream>
#include"DebugUtils.h"
#include"LogicalDevice.h"
#include"PhysicalDevice.h"
#include"ValidationLayers.h"

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
	DebugUtils::SetupDebugMessenger(m_instance, m_debugMessenger);
	CreateSurface();
	PhysicalDevice::PickPhysicalDevice(m_physicalDevice, m_instance, m_surface, m_window);
	LogicalDevice::CreateLogicalDevice(m_device, m_physicalDevice, m_graphicsQueue, m_presentQueue);
	PhysicalDevice::CreateSwapChain(m_swapChain, m_swapChainImages, m_physicalDevice, m_device);
}

void HelloTriangleApplication::CreateInstance()
{
	// Check if validation layers are enabled and exist
	if (ValidationLayers::enable_validation_layers && !ValidationLayers::CheckValidationLayerSupport())
		throw std::runtime_error("Validation layers requested, but not available");

	// Creating the application info
	vk::ApplicationInfo applicationInfo{
		.pApplicationName = "Hello Triangle",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};

	// Creating a structure chain for .pNext Pointers
	vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> chain;

	vk::InstanceCreateInfo& create_info = chain.get<vk::InstanceCreateInfo>();

	// Creating the information to create an instance
	create_info.pApplicationInfo = &applicationInfo;

	// Setting extensions
	auto extensions = GetRequiredExtensions();

	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	// Validation layers
	auto& debugCreateInfo = chain.get<vk::DebugUtilsMessengerCreateInfoEXT>();
	if (ValidationLayers::enable_validation_layers) {
		create_info.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validation_layers.size());
		create_info.ppEnabledLayerNames = ValidationLayers::validation_layers.data();

		DebugUtils::PopulateMessengerCreateInfo(debugCreateInfo);
	} else {
		create_info.enabledLayerCount = 0;
		chain.unlink<vk::DebugUtilsMessengerCreateInfoEXT>();
	}

	if (createInstance(&create_info, nullptr, &m_instance) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create instance!");

	std::cout << "Available extensions:\n";

	for (const auto& extension : extensions)
		std::cout << '\t' << extension << '\n';
}

void HelloTriangleApplication::CreateSurface()
{
	// Casting has to be done because glfwCreateWindowSurface only takes C structs from vulkan.h
	if (static_cast<vk::Result>(glfwCreateWindowSurface(m_instance, m_window, nullptr,
	                                                    reinterpret_cast<VkSurfaceKHR*>(&m_surface))) !=
	    vk::Result::eSuccess)
		throw std::runtime_error("Failed to create window surface!");
}

void HelloTriangleApplication::MainLoop() const
{
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
	}
}

std::vector<const char*> HelloTriangleApplication::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (ValidationLayers::enable_validation_layers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void HelloTriangleApplication::CleanUp() const
{
	// Destroy Swap Chain
	m_device.destroySwapchainKHR(m_swapChain, nullptr);

	// Destroy Logical Device
	m_device.destroy(nullptr);

	// Destroy Debug utility
	if (ValidationLayers::enable_validation_layers)
		DebugUtils::DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);

	// Destroy window surface
	m_instance.destroySurfaceKHR(m_surface, nullptr);

	// Destroy Vulkan instance
	m_instance.destroy();

	// Destroy the window pointer
	glfwDestroyWindow(m_window);

	// Finish using GLFW
	glfwTerminate();
}
