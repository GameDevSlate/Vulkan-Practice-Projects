// Adding the Vulkan C++ header, and ignoring all warnings
// the are only "use enum class instead" warning.
#pragma once
#pragma warning(push, 0)
#include<vulkan/vulkan.hpp>
#pragma warning(pop)

#include<vector>
#include<GLFW/glfw3.h>

constexpr uint32_t WIDTH = 800;

constexpr uint32_t HEIGHT = 600;

#pragma once
class HelloTriangleApplication
{
public:
	void Run();

private:
	void InitWindow();

	void InitVulkan();

	void CreateInstance();

	void CreateSurface();

	void CreateImageViews();

	void CreateFrameBuffers();

	void CreateCommandBuffers();

	void RecordCommandBuffer(vk::CommandBuffer command_buffer, uint32_t image_index);

	void CreateSyncObjects();

	void MainLoop();

	void DrawFrame();

	static std::vector<const char*> GetRequiredExtensions();

	void CleanUp() const;

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

	vk::PipelineLayout m_pipelineLayout;

	vk::Pipeline m_graphicsPipeline;

	std::vector<vk::Framebuffer> m_swapChainFrameBuffers;

	vk::CommandPool m_commandPool;

	vk::CommandBuffer m_commandBuffer;

	vk::Semaphore m_imageAvailableSemaphore;

	vk::Semaphore m_renderFinishedSemaphore;

	vk::Fence m_inFlightFence;
};
