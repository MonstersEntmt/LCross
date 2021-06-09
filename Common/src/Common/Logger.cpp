#include "Common/Logger.h"

#include <iostream>

namespace Logger {
	void logMessage(std::string_view message) {
		std::cout << Format::format("{1}{0:r}{0:br}\n", LogColors::Black, message);
	}
} // namespace Logger