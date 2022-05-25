#include<vector>

#pragma once

class ValidationLayers {
	public:

#ifndef _DEBUG
		static const bool enableValidationLayers = false;
#else
		static const bool enableValidationLayers = true;
#endif

		inline static const std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };

		static bool checkValidationLayerSupport();
};
