#include<vector>

#pragma once

class ValidationLayers
{
public:
	static constexpr bool enable_validation_layers =
#ifndef _DEBUG
		false;
#else
		true;
#endif

	inline static const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};

	static bool CheckValidationLayerSupport();
};
