#define VULKAN_HPP_NO_CONSTRUCTORS

#include "HelloTriangleApplication.h"
#include"ValidationLayers.h"
#include<iostream>

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
	SetupDebugMessenger();
}

void HelloTriangleApplication::CreateInstance()
{
	// Check if validation layers are enabled and exist
	if (ValidationLayers::enableValidationLayers && !ValidationLayers::checkValidationLayerSupport())
		throw std::runtime_error("Validation layers requested, but not available");

	// Creating the application info
	vk::ApplicationInfo applicationInfo{	.pApplicationName = "Hello Triangle",
											.applicationVersion = VK_MAKE_VERSION(1,0,0),
											.pEngineName = "No Engine",
											.engineVersion = VK_MAKE_VERSION(1, 0, 0),
											.apiVersion = VK_API_VERSION_1_3};
	
	// Creating a structure chain for .pNext Pointers
	vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> chain;

	vk::InstanceCreateInfo& createInfo = chain.get<vk::InstanceCreateInfo>();

	// Creating the information to create an instance
	createInfo.pApplicationInfo = &applicationInfo;

	// Setting extensions
	auto extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	// Validation layers
	vk::DebugUtilsMessengerCreateInfoEXT& debugCreateInfo = chain.get<vk::DebugUtilsMessengerCreateInfoEXT>();
	if (ValidationLayers::enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validation_layers.size());
		createInfo.ppEnabledLayerNames = ValidationLayers::validation_layers.data();

		PopulateMessengerCreateInfo(debugCreateInfo);
	}
	else {
		createInfo.enabledLayerCount = 0;
		chain.unlink<vk::DebugUtilsMessengerCreateInfoEXT>();
	}
	
	if (vk::createInstance(&createInfo, nullptr, &m_instance) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create instance!");
	
	std::cout << "Available extensions:\n";

	for (const auto& extension : extensions)
		std::cout << '\t' << extension << '\n';
}

void HelloTriangleApplication::PopulateMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
	
	createInfo = {	.messageSeverity =	vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
										vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
										vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
					.messageType =	vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
									vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
									vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
					.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(DebugCallback),
					.pUserData = nullptr};
}

void HelloTriangleApplication::SetupDebugMessenger()
{
	if (!ValidationLayers::enableValidationLayers) return;

	vk::DebugUtilsMessengerCreateInfoEXT messenger_ci;
	PopulateMessengerCreateInfo(messenger_ci);

	if (CreateDebugUtilsMessengerEXT(m_instance, &messenger_ci, nullptr, &m_debugMessenger) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to set up debug messenger!");
}

vk::Result HelloTriangleApplication::CreateDebugUtilsMessengerEXT(	vk::Instance instance,
																	const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
																	const vk::AllocationCallbacks* pAllocator,
																	vk::DebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));

	if (func != nullptr)
		return static_cast<vk::Result>(func(instance,
											(VkDebugUtilsMessengerCreateInfoEXT*)pCreateInfo,
											(VkAllocationCallbacks*)pAllocator,
											(VkDebugUtilsMessengerEXT*)pDebugMessenger));
	else
		return vk::Result::eErrorExtensionNotPresent;
}

void HelloTriangleApplication::DestroyDebugUtilsMessengerEXT(	vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger,
																const vk::AllocationCallbacks* pAllocator)
{
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

	if (func != nullptr)
		func(instance, debugMessenger, (VkAllocationCallbacks*)pAllocator);
}

void HelloTriangleApplication::MainLoop()
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

	if (ValidationLayers::enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL HelloTriangleApplication::DebugCallback(	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
																			vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
																			const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, 
																			void* pUserData)
{
	std::cerr << "Validation layer:" << pCallbackData->pMessage << std::endl;
	
	return VK_FALSE;
}

void HelloTriangleApplication::CleanUp()
{
	// Destroy Debug utility
	if (ValidationLayers::enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);

	// Destroy Vulkan instance
	m_instance.destroy();

	// Destroy the window pointer
	glfwDestroyWindow(m_window);

	// Finish using GLFW
	glfwTerminate();
}
