// Base needed libraries
#include <cstdlib>
#include <iostream>

#include "Core/Log.h"

// Custom Vulkan Application class
#include"HelloTriangleApplication.h"

int main()
{
	Log::Init();

	HelloTriangleApplication app;

	try {
		app.Run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
