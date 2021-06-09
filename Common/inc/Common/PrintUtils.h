#pragma once

#include <iostream>
#include <stdint.h>
#include <string>

namespace PrintUtils {
	enum class ANSIColor : uint8_t {
		BLACK   = 0,
		RED     = 1,
		GREEN   = 2,
		YELLOW  = 3,
		BLUE    = 4,
		MAGENTA = 5,
		CYAN    = 6,
		WHITE   = 7
	};

	void setupAnsi();
	void restoreAnsi();

	std::string ansiFGColor(ANSIColor color, bool bright = false);
	std::string ansiBGColor(ANSIColor color, bool bright = false);
	std::string ansiFGBColor(ANSIColor color, uint8_t brightness);
	std::string ansiBGBColor(ANSIColor color, uint8_t brightness);
	std::string ansiFGColorRGB(uint8_t r, uint8_t g, uint8_t b);
	std::string ansiBGColorRGB(uint8_t r, uint8_t g, uint8_t b);
	std::string ansiReset();

	std::ostream& colorSchemeInfo(std::ostream& ostream);
	std::ostream& colorSchemeWarn(std::ostream& ostream);
	std::ostream& colorSchemeError(std::ostream& ostream);
	std::ostream& colorSchemeUsage(std::ostream& ostream);
	std::ostream& colorSchemeHelp(std::ostream& ostream);
	std::ostream& colorSchemeArg(std::ostream& ostream);
	std::ostream& colorSchemeNote(std::ostream& ostream);
	std::ostream& colorSchemeNoteLabel(std::ostream& ostream);

	std::ostream& appInfo(std::ostream& ostream);
	std::ostream& appWarn(std::ostream& ostream);
	std::ostream& appError(std::ostream& ostream);
	std::ostream& appUsage(std::ostream& ostream);
	std::ostream& appHelp(std::ostream& ostream);
	std::ostream& normal(std::ostream& ostream);
	extern std::string appName();
	extern std::string appVersion();
} // namespace PrintUtils