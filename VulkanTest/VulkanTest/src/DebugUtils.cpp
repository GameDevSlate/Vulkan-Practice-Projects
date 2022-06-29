#define VULKAN_HPP_NO_CONSTRUCTORS
#include "DebugUtils.h"
#include<iostream>
#include "ValidationLayers.h"

void DebugUtils::PopulateMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& create_info)
{
	create_info = {
		.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		                   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		               vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(DebugCallback),
		.pUserData = nullptr
	};
}

void DebugUtils::SetupDebugMessenger(const vk::Instance instance, vk::DebugUtilsMessengerEXT& debug_messenger)
{
	if constexpr (!ValidationLayers::enable_validation_layers) return;

	vk::DebugUtilsMessengerCreateInfoEXT messenger_ci;
	PopulateMessengerCreateInfo(messenger_ci);

	if (CreateDebugUtilsMessengerEXT(instance, &messenger_ci, nullptr, &debug_messenger) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to set up debug messenger!");
}

vk::Result DebugUtils::CreateDebugUtilsMessengerEXT(const vk::Instance instance,
                                                    const vk::DebugUtilsMessengerCreateInfoEXT* p_create_info,
                                                    const vk::AllocationCallbacks* p_allocator,
                                                    vk::DebugUtilsMessengerEXT* p_debug_messenger)
{
	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.
		getProcAddr("vkCreateDebugUtilsMessengerEXT"));

	if (func != nullptr)
		return static_cast<vk::Result>(func(instance,
		                                    (VkDebugUtilsMessengerCreateInfoEXT*)p_create_info,
		                                    (VkAllocationCallbacks*)p_allocator,
		                                    (VkDebugUtilsMessengerEXT*)p_debug_messenger));
	return vk::Result::eErrorExtensionNotPresent;
}

void DebugUtils::DestroyDebugUtilsMessengerEXT(const vk::Instance instance,
                                               const vk::DebugUtilsMessengerEXT debug_messenger,
                                               const vk::AllocationCallbacks* p_allocator)
{
	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.
		getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

	if (func != nullptr)
		func(instance, debug_messenger, (VkAllocationCallbacks*)p_allocator);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugUtils::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messages_severity,
                                                           vk::DebugUtilsMessageTypeFlagsEXT message_types,
                                                           const vk::DebugUtilsMessengerCallbackDataEXT*
                                                           p_callback_data,
                                                           void* p_user_data)
{
	std::cerr << std::endl << "Validation layer:" << p_callback_data->pMessage << std::endl;

	return VK_FALSE;
}
