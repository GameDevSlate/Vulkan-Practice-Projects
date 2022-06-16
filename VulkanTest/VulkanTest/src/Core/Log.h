#pragma once
#include <memory>

#include <glm/gtx/string_cast.hpp>

// Ignore all warnings from the inside of external headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_sCoreLogger; }

	static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_sClientLogger; }

private:
	static std::shared_ptr<spdlog::logger> m_sCoreLogger;

	static std::shared_ptr<spdlog::logger> m_sClientLogger;
};

template < typename OStream, glm::length_t L, typename T, glm::qualifier Q >
OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template < typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q >
OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template < typename OStream, typename T, glm::qualifier Q >
OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core log macros
#define VK_CORE_TRACE(...)		::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VK_CORE_INFO(...)		::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VK_CORE_WARN(...)		::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define VK_CORE_ERROR(...)		::Log::GetCoreLogger()->error(__VA_ARGS__);
#define VK_CORE_CRITICAL(...)	::Log::GetCoreLogger()->critical(__VA_ARGS__);

// Client log macros
#define VK_TRACE(...)		::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VK_INFO(...)		::Log::GetClientLogger()->info(__VA_ARGS__)
#define VK_WARN(...)		::Log::GetClientLogger()->warn(__VA_ARGS__);
#define VK_ERROR(...)		::Log::GetClientLogger()->error(__VA_ARGS__);
#define VK_CRITICAL(...)	::Log::GetClientLogger()->critical(__VA_ARGS__);
