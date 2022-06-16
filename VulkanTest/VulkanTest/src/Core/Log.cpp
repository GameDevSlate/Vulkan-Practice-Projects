#include "Log.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::m_sCoreLogger;

std::shared_ptr<spdlog::logger> Log::m_sClientLogger;

void Log::Init()
{
	std::vector<spdlog::sink_ptr> log_sinks;
	log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Practice.log", true));

	log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
	log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

	m_sCoreLogger = std::make_shared<spdlog::logger>("PRACTICE-RENDERER", begin(log_sinks), end(log_sinks));
	register_logger(m_sCoreLogger);
	m_sCoreLogger->set_level(spdlog::level::trace);
	m_sCoreLogger->flush_on(spdlog::level::trace);

	m_sClientLogger = std::make_shared<spdlog::logger>("APP", begin(log_sinks), end(log_sinks));
	register_logger(m_sClientLogger);
	m_sClientLogger->set_level(spdlog::level::trace);
	m_sClientLogger->flush_on(spdlog::level::trace);
}
