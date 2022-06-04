#include<vector>

#pragma once

class ValidationLayers
{
public:
#ifndef _DEBUG
		static constexpr bool enable_validation_layers = false;
#else
	static constexpr bool enable_validation_layers = true;
#endif

	inline static const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};

	static bool CheckValidationLayerSupport();
};
