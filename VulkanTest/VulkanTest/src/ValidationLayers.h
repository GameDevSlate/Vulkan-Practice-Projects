#include<vector>

#pragma once

class ValidationLayers {
	public:

#ifndef _DEBUG
		static const bool enableValidationLayers = false;
#else
		static const bool enableValidationLayers = true;
#endif

		static const std::vector<const char*> validation_layers;

		static bool checkValidationLayerSupport();
};
