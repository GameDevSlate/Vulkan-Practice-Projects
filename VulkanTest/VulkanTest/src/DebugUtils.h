#pragma once

#include<vulkan/vulkan.hpp>

class DebugUtils
{
public:
	static void PopulateMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);
	static void SetupDebugMessenger(vk::Instance instance, vk::DebugUtilsMessengerEXT& debugMessenger);
	static vk::Result CreateDebugUtilsMessengerEXT(	vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo,
												const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger);
	
	static void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator);
	
private:
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
															vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
															const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
															void* pUserData);
};

