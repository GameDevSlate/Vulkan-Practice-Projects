// Adding the Vulkan C++ header, and ignoring all warnings
// the are only "use enum class instead" warning.
#pragma once
#pragma warning(push, 0)
#include<vulkan/vulkan.hpp>
#pragma warning(pop)

#include<vector>
#include<GLFW/glfw3.h>

#include"Vertex.h"

constexpr uint32_t WIDTH = 800;

constexpr uint32_t HEIGHT = 600;

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

#pragma once
class HelloTriangleApplication
{
public:
	void Run();

private:
	void InitWindow();

	static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

	void InitVulkan();

	void CreateInstance();

	void CreateSurface();

	void RecreateSwapChain();

	void CleanUpSwapChain();

	void CreateCommandBuffers();

	void RecordCommandBuffer(vk::CommandBuffer command_buffer, uint32_t image_index);

	void CreateSyncObjects();

	void MainLoop();

	void DrawFrame();

	static std::vector<const char*> GetRequiredExtensions();

	void CleanUp();

	GLFWwindow* m_window = nullptr;

	vk::Instance m_instance;

	vk::DebugUtilsMessengerEXT m_debugMessenger;

	vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	vk::Device m_device;

	vk::Queue m_graphicsQueue;

	vk::SurfaceKHR m_surface;

	vk::Queue m_presentQueue;

	vk::SwapchainKHR m_swapChain;

	std::vector<vk::Image> m_swapChainImages;

	vk::Format m_swapChainImageFormat;

	vk::Extent2D m_swapChainExtent;

	std::vector<vk::ImageView> m_swapChainImageViews;

	vk::RenderPass m_renderPass;

	vk::DescriptorSetLayout m_descriptorSetLayout;

	vk::PipelineLayout m_pipelineLayout;

	vk::Pipeline m_graphicsPipeline;

	std::vector<vk::Framebuffer> m_swapChainFrameBuffers;

	vk::CommandPool m_commandPool;

	vk::Buffer m_vertexBuffer;

	vk::DeviceMemory m_vertexBufferMemory;

	vk::Buffer m_indexBuffer;

	vk::DeviceMemory m_indexBufferMemory;

	std::vector<vk::Buffer> m_uniformBuffers;

	std::vector<vk::DeviceMemory> m_uniformBuffersMemory;

	vk::DescriptorPool m_descriptorPool;

	std::vector<vk::DescriptorSet> m_descriptorSets;

	std::vector<vk::CommandBuffer> m_commandBuffers;

	std::vector<vk::Semaphore> m_imageAvailableSemaphores;

	std::vector<vk::Semaphore> m_renderFinishedSemaphores;

	std::vector<vk::Fence> m_inFlightFences;

	bool m_frameBufferResized = false;

	uint32_t m_currentFrame = 0;

	const std::vector<Vertex> m_vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> m_indices = {
		0,
		1,
		2,
		2,
		3,
		0
	};
};
