#define VULKAN_HPP_NO_CONSTRUCTORS

#include "HelloTriangleApplication.h"

#include<iostream>

#include "DebugUtils.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "OpenGLShader.h"
#include "PhysicalDevice.h"
#include "SwapChain.h"
#include "ValidationLayers.h"
#include "VertexBuffer.h"

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

	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(m_window, this);

	glfwSetFramebufferSizeCallback(m_window, FrameBufferResizeCallback);
}

void HelloTriangleApplication::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
	app->m_frameBufferResized = true;
}

void HelloTriangleApplication::InitVulkan()
{
	CreateInstance();

	DebugUtils::SetupDebugMessenger(m_instance, m_debugMessenger);

	CreateSurface();

	PhysicalDevice::PickPhysicalDevice(m_physicalDevice, m_instance, m_surface, m_window);

	LogicalDevice::CreateLogicalDevice(m_device, m_physicalDevice, m_graphicsQueue, m_presentQueue);

	SwapChain::CreateSwapChain(m_swapChain, m_swapChainImages, m_swapChainImageFormat, m_swapChainExtent,
	                           m_physicalDevice, m_device, m_surface, m_window);

	SwapChain::CreateImageViews(m_swapChainImageViews, m_device, m_swapChainImages, m_swapChainImageFormat);

	const OpenGLShader triangle_shader("Triangle", "assets/shaders/Triangle.vert", "assets/shaders/Triangle.frag");

	GraphicsPipeline::CreateRenderPass(m_renderPass, m_device, m_swapChainImageFormat);

	GraphicsPipeline::CreateGraphicsPipeline(m_graphicsPipeline, m_pipelineLayout, m_renderPass, m_device,
	                                         m_swapChainExtent, triangle_shader);

	SwapChain::CreateFrameBuffers(m_swapChainFrameBuffers, m_device, m_swapChainImageViews, m_swapChainExtent,
	                              m_renderPass);

	PhysicalDevice::CreateCommandPool(m_commandPool, m_physicalDevice, m_device);

	VertexBuffer::CreateVertexBuffer(m_vertexBuffer, m_vertexBufferMemory, m_device, m_physicalDevice, m_vertices);

	CreateCommandBuffers();

	CreateSyncObjects();
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

	auto& create_info = chain.get<vk::InstanceCreateInfo>();

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

void HelloTriangleApplication::RecreateSwapChain()
{
	// Handling the case when the window is minimized
	int width = 0, height = 0;

	glfwGetFramebufferSize(m_window, &width, &height);

	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(m_window, &width, &height);
		glfwWaitEvents();
	}

	m_device.waitIdle();

	CleanUpSwapChain();

	SwapChain::CreateSwapChain(m_swapChain, m_swapChainImages, m_swapChainImageFormat, m_swapChainExtent,
	                           m_physicalDevice, m_device, m_surface, m_window);

	SwapChain::CreateImageViews(m_swapChainImageViews, m_device, m_swapChainImages, m_swapChainImageFormat);

	const OpenGLShader triangle_shader("Triangle", "assets/shaders/Triangle.vert", "assets/shaders/Triangle.frag");

	GraphicsPipeline::CreateRenderPass(m_renderPass, m_device, m_swapChainImageFormat);

	GraphicsPipeline::CreateGraphicsPipeline(m_graphicsPipeline, m_pipelineLayout, m_renderPass, m_device,
	                                         m_swapChainExtent, triangle_shader);

	SwapChain::CreateFrameBuffers(m_swapChainFrameBuffers, m_device, m_swapChainImageViews, m_swapChainExtent,
	                              m_renderPass);
}

void HelloTriangleApplication::CleanUpSwapChain()
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
}

void HelloTriangleApplication::CreateCommandBuffers()
{
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	vk::CommandBufferAllocateInfo alloc_info{
		.commandPool = m_commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size())
	};

	if (m_device.allocateCommandBuffers(&alloc_info, m_commandBuffers.data()) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to allocate command buffers!");
}

void HelloTriangleApplication::RecordCommandBuffer(const vk::CommandBuffer command_buffer, const uint32_t image_index)
{
	vk::CommandBufferBeginInfo begin_info{
		// optional
		// .flags,
		// optional
		.pInheritanceInfo = nullptr
	};

	if (command_buffer.begin(&begin_info) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to being recording command buffer!");

	vk::RenderPassBeginInfo render_pass_info{
		.renderPass = m_renderPass,
		.framebuffer = m_swapChainFrameBuffers[image_index],
		.renderArea{
			.offset = {0, 0},
			.extent = m_swapChainExtent
		}
	};

	vk::ClearValue clear_color{
		.color = {vk::ArrayWrapper1D<float, 4>{{0.0f, 0.0f, 0.0f, 1.0f}}}
	};

	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clear_color;

	// Begin recording commands

	command_buffer.beginRenderPass(&render_pass_info, vk::SubpassContents::eInline);

	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline);

	// Binding the vertex buffer
	vk::Buffer vertex_buffers[] = {m_vertexBuffer};
	vk::DeviceSize offsets[] = {0};

	command_buffer.bindVertexBuffers(0, 1, vertex_buffers, offsets);

	command_buffer.draw(static_cast<uint32_t>(m_vertices.size()), 1, 0, 0);

	// End recording commands

	command_buffer.endRenderPass();

	command_buffer.end();
}

void HelloTriangleApplication::CreateSyncObjects()
{
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	vk::SemaphoreCreateInfo semaphore_info{};

	vk::FenceCreateInfo fence_info{
		.flags = vk::FenceCreateFlagBits::eSignaled
	};

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		if (m_device.createSemaphore(&semaphore_info, nullptr, &m_imageAvailableSemaphores[i]) != vk::Result::eSuccess
		    ||
		    m_device.createSemaphore(&semaphore_info, nullptr, &m_renderFinishedSemaphores[i]) != vk::Result::eSuccess
		    ||
		    m_device.createFence(&fence_info, nullptr, &m_inFlightFences[i]) != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create synchronization objects for a frame!");
	}
}

void HelloTriangleApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
		DrawFrame();
	}

	m_device.waitIdle();
}

void HelloTriangleApplication::DrawFrame()
{
	// Wait until previous (or first) frame is finished
	m_device.waitForFences(1, &m_inFlightFences[m_currentFrame], true, UINT64_MAX);

	uint32_t image_index;

	vk::Result result = m_device.acquireNextImageKHR(m_swapChain, UINT64_MAX,
	                                                 m_imageAvailableSemaphores[m_currentFrame],
	                                                 VK_NULL_HANDLE, &image_index);

	if (result == vk::Result::eErrorOutOfDateKHR) {
		RecreateSwapChain();
		return;
	}
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
		throw std::runtime_error("Failed to acquire swap chain image!");

	m_device.resetFences(1, &m_inFlightFences[m_currentFrame]);

	m_commandBuffers[m_currentFrame].reset();

	RecordCommandBuffer(m_commandBuffers[m_currentFrame], image_index);

	// Queue submission and synchronization
	vk::SubmitInfo submit_info;

	vk::Semaphore wait_semaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};

	vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_commandBuffers[m_currentFrame];

	vk::Semaphore signal_semaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};

	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	if (m_graphicsQueue.submit(1, &submit_info, m_inFlightFences[m_currentFrame]) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to submit draw command buffer!");

	// Presentation
	vk::PresentInfoKHR present_info{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signal_semaphores
	};

	vk::SwapchainKHR swap_chains[] = {m_swapChain};

	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;
	present_info.pImageIndices = &image_index;
	// optional
	present_info.pResults = nullptr;

	result = m_presentQueue.presentKHR(&present_info);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || m_frameBufferResized) {
		m_frameBufferResized = false;
		RecreateSwapChain();
	} else if (result != vk::Result::eSuccess)
		throw std::runtime_error("Failed to present swap chain image!");

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

std::vector<const char*> HelloTriangleApplication::GetRequiredExtensions()
{
	uint32_t glfw_extension_count = 0;

	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

	if (ValidationLayers::enable_validation_layers)
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensions;
}

void HelloTriangleApplication::CleanUp()
{
	CleanUpSwapChain();

	// Free allocated GPU memory for the vertices
	m_device.destroyBuffer(m_vertexBuffer, nullptr);
	m_device.freeMemory(m_vertexBufferMemory, nullptr);

	// Destroy semaphores and fences
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_device.destroySemaphore(m_imageAvailableSemaphores[i], nullptr);
		m_device.destroySemaphore(m_renderFinishedSemaphores[i], nullptr);
		m_device.destroyFence(m_inFlightFences[i], nullptr);
	}

	// Destroy command pool
	m_device.destroyCommandPool(m_commandPool, nullptr);

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
