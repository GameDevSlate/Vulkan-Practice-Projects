#define VULKAN_HPP_NO_CONSTRUCTORS

#include "HelloTriangleApplication.h"

#include<iostream>

#include"DebugUtils.h"
#include "GraphicsPipeline.h"
#include"LogicalDevice.h"
#include "OpenGLShader.h"
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
	PhysicalDevice::CreateSwapChain(m_swapChain, m_swapChainImages, m_swapChainImageFormat, m_swapChainExtent,
	                                m_physicalDevice, m_device);
	CreateImageViews();

	const OpenGLShader triangle_shader("Triangle", "assets/shaders/Triangle.vert", "assets/shaders/Triangle.frag");

	GraphicsPipeline::CreateRenderPass(m_renderPass, m_device, m_swapChainImageFormat);

	GraphicsPipeline::CreateGraphicsPipeline(m_graphicsPipeline, m_pipelineLayout, m_renderPass, m_device,
	                                         m_swapChainExtent, triangle_shader);

	CreateFrameBuffers();
}

void HelloTriangleApplication::CreateInstance()
{
	// Check if validation layers are enabled and exist
	if (ValidationLayers::enable_validation_layers && !ValidationLayers::CheckValidationLayerSupport())
		throw std::runtime_error("Validation layers requested, but not available");

	// Creating the application info
	vk::ApplicationInfo application_info{
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
	create_info.pApplicationInfo = &application_info;

	// Setting extensions
	auto extensions = GetRequiredExtensions();

	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	// Validation layers
	auto& debug_create_info = chain.get<vk::DebugUtilsMessengerCreateInfoEXT>();
	if (ValidationLayers::enable_validation_layers) {
		create_info.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validation_layers.size());
		create_info.ppEnabledLayerNames = ValidationLayers::validation_layers.data();

		DebugUtils::PopulateMessengerCreateInfo(debug_create_info);
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

void HelloTriangleApplication::CreateImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size());

	for (size_t i = 0; i < m_swapChainImages.size(); i++) {

		vk::ImageViewCreateInfo create_info{
			.image = m_swapChainImages[i],
			.viewType = vk::ImageViewType::e2D,
			.format = m_swapChainImageFormat,
			.components = {
				.r = vk::ComponentSwizzle::eIdentity,
				.g = vk::ComponentSwizzle::eIdentity,
				.b = vk::ComponentSwizzle::eIdentity,
				.a = vk::ComponentSwizzle::eIdentity
			},
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		if (m_device.createImageView(&create_info, nullptr, &m_swapChainImageViews[i]) != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create image views");
	}
}

void HelloTriangleApplication::CreateFrameBuffers()
{
	m_swapChainFrameBuffers.resize(m_swapChainImageViews.size());

	for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
		vk::ImageView attachments[] = {m_swapChainImageViews[i]};

		vk::FramebufferCreateInfo framebuffer_info{
			.renderPass = m_renderPass,
			.attachmentCount = 1,
			.pAttachments = attachments,
			.width = m_swapChainExtent.width,
			.height = m_swapChainExtent.height,
			.layers = 1
		};

		if (m_device.createFramebuffer(&framebuffer_info, nullptr, &m_swapChainFrameBuffers[i]) != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create framebuffer!");
	}
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

	std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (ValidationLayers::enable_validation_layers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void HelloTriangleApplication::CleanUp() const
{
	// Destroy swap chain frame buffers
	for (auto framebuffer : m_swapChainFrameBuffers)
		m_device.destroyFramebuffer(framebuffer, nullptr);

	// Destroy the pipeline, its layout, and render pass
	m_device.destroyPipeline(m_graphicsPipeline, nullptr);
	m_device.destroyPipelineLayout(m_pipelineLayout, nullptr);
	m_device.destroyRenderPass(m_renderPass, nullptr);

	//Destroy image views
	for (const vk::ImageView image_view : m_swapChainImageViews)
		m_device.destroyImageView(image_view, nullptr);

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
