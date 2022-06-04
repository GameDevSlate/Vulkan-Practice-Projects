#include "ValidationLayers.h"
#include<vulkan/vulkan.hpp>

bool ValidationLayers::CheckValidationLayerSupport()
{
	// Get the number of available layers
	uint32_t layer_count;
	vk::enumerateInstanceLayerProperties(&layer_count, nullptr);

	// Enumerate the available layers
	std::vector<vk::LayerProperties> available_layers(layer_count);
	enumerateInstanceLayerProperties(&layer_count, available_layers.data());

	// Check if all the layers exist
	for (const char* layer_name : validation_layers) {
		bool layer_found = false;

		for (const auto& layer_properties : available_layers) {
			if (strcmp(layer_name, layer_properties.layerName) == 0) {
				layer_found = true;
				break;
			}
		}

		if (!layer_found)
			return false;
	}

	return true;
}
