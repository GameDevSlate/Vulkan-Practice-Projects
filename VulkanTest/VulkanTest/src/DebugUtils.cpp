#define VULKAN_HPP_NO_CONSTRUCTORS
#include "DebugUtils.h"
#include "ValidationLayers.h"
#include<iostream>

void DebugUtils::PopulateMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
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

void DebugUtils::SetupDebugMessenger(vk::Instance instance, vk::DebugUtilsMessengerEXT& debugMessenger)
{

	if (!ValidationLayers::enableValidationLayers) return;

	vk::DebugUtilsMessengerCreateInfoEXT messenger_ci;
	PopulateMessengerCreateInfo(messenger_ci);

	if (CreateDebugUtilsMessengerEXT(instance, &messenger_ci, nullptr, &debugMessenger) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to set up debug messenger!");
}

vk::Result DebugUtils::CreateDebugUtilsMessengerEXT(vk::Instance instance,
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

void DebugUtils::DestroyDebugUtilsMessengerEXT(	vk::Instance instance,
												vk::DebugUtilsMessengerEXT debugMessenger,
												const vk::AllocationCallbacks* pAllocator)
{

	auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));

	if (func != nullptr)
		func(instance, debugMessenger, (VkAllocationCallbacks*)pAllocator);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugUtils::DebugCallback(	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
															vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
															const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
															void* pUserData)
{
	std::cerr << "Validation layer:" << pCallbackData->pMessage << std::endl;
	
	return VK_FALSE;
}
