#pragma once

#include "Format/Format.h"
#include "PrintUtils.h"

#include <string>
#include <string_view>

struct LogColor {
	constexpr LogColor(uint8_t r, uint8_t g, uint8_t b)
	    : r(r), g(g), b(b) { }

	uint8_t r, g, b;
};

enum class LogSeverity {
	Info,
	Warn,
	Error,
	Usage,
	Help
};

namespace LogColors {
	static constexpr LogColor White  = { 255, 255, 255 };
	static constexpr LogColor Red    = { 255, 0, 0 };
	static constexpr LogColor Yellow = { 255, 255, 0 };
	static constexpr LogColor Green  = { 0, 255, 0 };
	static constexpr LogColor Cyan   = { 0, 255, 255 };
	static constexpr LogColor Blue   = { 0, 0, 255 };
	static constexpr LogColor Purple = { 255, 0, 255 };
	static constexpr LogColor Black  = { 0, 0, 0 };

	static constexpr LogColor Info      = { 127, 255, 255 };
	static constexpr LogColor Warn      = { 255, 127, 0 };
	static constexpr LogColor Error     = { 255, 30, 30 };
	static constexpr LogColor Usage     = { 127, 255, 63 };
	static constexpr LogColor Help      = { 127, 255, 255 };
	static constexpr LogColor Arg       = { 255, 255, 66 };
	static constexpr LogColor Note      = { 255, 255, 0 };
	static constexpr LogColor NoteLabel = { 255, 63, 63 };
} // namespace LogColors

template <>
struct Format::Formatter<LogColor> {
	std::string format(LogColor color, const std::string& options) {
		if (options.find_first_of('b') < options.size()) {
			if (options.find_first_of('r') < options.size())
				return "\033[49m";
			else
				return "\033[48;2;" + std::to_string(color.r) + ";" + std::to_string(color.g) + ";" + std::to_string(color.b) + "m";
		} else {
			if (options.find_first_of('r') < options.size())
				return "\033[39m";
			else
				return "\033[38;2;" + std::to_string(color.r) + ";" + std::to_string(color.g) + ";" + std::to_string(color.b) + "m";
		}
	}
};

template <>
struct Format::Formatter<LogSeverity> {
	std::string format(LogSeverity severity, const std::string& option) {
		switch (severity) {
		case LogSeverity::Info: return Format::format("{}{} Info", LogColors::Info, PrintUtils::appName());
		case LogSeverity::Warn: return Format::format("{}{} Warn", LogColors::Warn, PrintUtils::appName());
		case LogSeverity::Error: return Format::format("{}{} Error", LogColors::Error, PrintUtils::appName());
		case LogSeverity::Usage: return Format::format("{}{} Usage", LogColors::Usage, PrintUtils::appName());
		case LogSeverity::Help: return Format::format("{}{} Help", LogColors::Help, PrintUtils::appName());
		default: return Format::format("{0:r}{0:br}{1}", LogColors::Black, PrintUtils::appName());
		}
	}
};

namespace Logger {
	void logMessage(std::string_view message);

	template <class... Params>
	void log(LogSeverity severity, std::string_view format, const Params&... args) {
		std::string message = Format::format(format, args...);
		logMessage(Format::format("{} {}", severity, message));
	}

	template <class... Params>
	void print(std::string_view format, const Params&... args) {
		logMessage(Format::format(format, args...));
	}
}; // namespace Logger