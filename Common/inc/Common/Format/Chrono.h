#pragma once

#include "Format.h"

#include <chrono>
#include <ctime>

namespace Format {
	template <typename Clock, typename Duration>
	struct Formatter<std::chrono::time_point<Clock, Duration>> {
		std::string format(const std::chrono::time_point<Clock, Duration>& value, const std::string& option) {
			std::time_t time = Clock::to_time_t(value);
			std::string timeFormat;
			if (option.size() > 0)
				timeFormat = option;
			else
				timeFormat = "%c";
			char buf[256];
			if (std::strftime(buf, 256, timeFormat.c_str(), std::localtime(&time)))
				return std::string(buf);
			return "";
		}
	};
} // namespace Format