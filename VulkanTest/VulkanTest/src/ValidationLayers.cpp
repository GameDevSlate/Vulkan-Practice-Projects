#include "ValidationLayers.h"
#include<vulkan/vulkan.hpp>

const std::vector<const char*> ValidationLayers::validation_layers = { "VK_LAYER_KHRONOS_validation" };

bool ValidationLayers::checkValidationLayerSupport()
{
    // Get the number of available layers
    uint32_t layerCount;
    vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
    
    // Enumerate the available layers
    std::vector<vk::LayerProperties> availableLayers(layerCount);
    vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());


    // Check if all the layers exist
    for (const char* layerName : ValidationLayers::validation_layers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}
