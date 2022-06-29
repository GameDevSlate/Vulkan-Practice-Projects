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

	PhysicalDevice::CreateCommandPool(m_commandPool, m_physicalDevice, m_device);

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

void HelloTriangleApplication::CreateImageViews()
{
	// Populate the amount of available swap chain image views
	// based on the amount of swap chain images
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
	// Populate the amount of frame buffers
	// based on the amount of image views
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

void HelloTriangleApplication::CreateCommandBuffers()
{
	vk::CommandBufferAllocateInfo alloc_info{
		.commandPool = m_commandPool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	if (m_device.allocateCommandBuffers(&alloc_info, &m_commandBuffer) != vk::Result::eSuccess)
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

	command_buffer.draw(3, 1, 0, 0);

	// End recording commands

	command_buffer.endRenderPass();

	command_buffer.end();
}

void HelloTriangleApplication::CreateSyncObjects()
{
	vk::SemaphoreCreateInfo semaphore_info{};

	vk::FenceCreateInfo fence_info{
		.flags = vk::FenceCreateFlagBits::eSignaled
	};

	if (m_device.createSemaphore(&semaphore_info, nullptr, &m_imageAvailableSemaphore) != vk::Result::eSuccess ||
	    m_device.createSemaphore(&semaphore_info, nullptr, &m_renderFinishedSemaphore) != vk::Result::eSuccess ||
	    m_device.createFence(&fence_info, nullptr, &m_inFlightFence) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to create semaphores!");
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
	m_device.waitForFences(1, &m_inFlightFence, true, UINT64_MAX);

	m_device.resetFences(1, &m_inFlightFence);

	uint32_t image_index;

	m_device.acquireNextImageKHR(m_swapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &image_index);

	m_commandBuffer.reset();

	RecordCommandBuffer(m_commandBuffer, image_index);

	// Queue submission and synchronization
	vk::SubmitInfo submit_info;

	vk::Semaphore wait_semaphores[] = {m_imageAvailableSemaphore};

	vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_commandBuffer;

	vk::Semaphore signal_semaphores[] = {m_renderFinishedSemaphore};

	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	if (m_graphicsQueue.submit(1, &submit_info, m_inFlightFence) != vk::Result::eSuccess)
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

	m_presentQueue.presentKHR(&present_info);
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

void HelloTriangleApplication::CleanUp() const
{
	//Destroy semaphores and fence
	m_device.destroySemaphore(m_imageAvailableSemaphore, nullptr);
	m_device.destroySemaphore(m_renderFinishedSemaphore, nullptr);
	m_device.destroyFence(m_inFlightFence, nullptr);

	// Destroy command pool
	m_device.destroyCommandPool(m_commandPool, nullptr);

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
