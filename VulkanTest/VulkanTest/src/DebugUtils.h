#pragma once

#include<vulkan/vulkan.hpp>

class DebugUtils
{
public:
	static void PopulateMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info);

	static void SetupDebugMessenger(vk::Instance instance, vk::DebugUtilsMessengerEXT& debug_messenger);

	static vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance,
	                                               const vk::DebugUtilsMessengerCreateInfoEXT* p_create_info,
	                                               const vk::AllocationCallbacks* p_allocator,
	                                               vk::DebugUtilsMessengerEXT* p_debug_messenger);

	static void DestroyDebugUtilsMessengerEXT(vk::Instance instance,
	                                          vk::DebugUtilsMessengerEXT debug_messenger,
	                                          const vk::AllocationCallbacks* p_allocator);

private:
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messages_severity,
	                                                      vk::DebugUtilsMessageTypeFlagsEXT message_types,
	                                                      const vk::DebugUtilsMessengerCallbackDataEXT* p_callback_data,
	                                                      void* p_user_data);
};
