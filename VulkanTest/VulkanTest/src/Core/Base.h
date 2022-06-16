#pragma once

#include "PlatformDetection.h"

#ifdef _DEBUG

#if defined(VK_PLATFORM_WINDOWS)

#define VK_DEBUGBREAK() __debugbreak()

#elif defined(VK_PLATFORM_LINUX)

#include <signal.h>
#define VK_DEBUGBREAK() raise(SIGTRAP)

#else

#error "Platform doesn't support debugbreak yet!"

#endif

#define VK_ENABLE_ASSERTS

#else

#define VK_DEBUGBREAK()

#endif

#define VK_EXPAND_MACRO(x) x
#define VK_STRINGIFY_MACRO(x) #x
