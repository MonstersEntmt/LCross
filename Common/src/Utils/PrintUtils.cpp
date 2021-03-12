#include "Utils/PrintUtils.h"

namespace PrintUtils {
	std::string ansiFGColor(ANSIColor color, bool bright) {
		return "\033[" + std::to_string((uint8_t) color + (bright ? 90 : 30)) + "m";
	}

	std::string ansiBGColor(ANSIColor color, bool bright) {
		return "\033[" + std::to_string((uint8_t) color + (bright ? 100 : 40)) + "m";
	}

	std::string ansiFGBColor(ANSIColor color, uint8_t brightness) {
		if (color == ANSIColor::WHITE) return "\033[38;5" + std::to_string(232 + brightness) + "m";
		else return "\033[38;5;" + std::to_string((uint8_t) color * (brightness + 1));
	}

	std::string ansiBGBColor(ANSIColor color, uint8_t brightness) {
		if (color == ANSIColor::WHITE) return "\033[48;5" + std::to_string(232 + brightness) + "m";
		else return "\033[48;5;" + std::to_string((uint8_t) color * (brightness + 1));
	}

	std::string ansiFGColorRGB(uint8_t r, uint8_t g, uint8_t b) {
		return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
	}

	std::string ansiBGColorRGB(uint8_t r, uint8_t g, uint8_t b) {
		return "\033[48;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
	}

	std::string ansiReset() { return "\033[m"; }

	std::ostream& colorSchemeInfo(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::CYAN, true); }
	std::ostream& colorSchemeWarn(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::YELLOW); }
	std::ostream& colorSchemeError(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::RED); }
	std::ostream& colorSchemeUsage(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::GREEN, true); }
	std::ostream& colorSchemeHelp(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::CYAN, true); }
	std::ostream& colorSchemeArg(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::YELLOW, true); }
	std::ostream& colorSchemeNote(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::YELLOW); }
	std::ostream& colorSchemeNoteLabel(std::ostream& ostream) { return ostream << ansiFGColor(ANSIColor::RED, true); }

	std::ostream& appInfo(std::ostream& ostream) { return ostream << colorSchemeInfo << appName << " Info: "; }
	std::ostream& appWarn(std::ostream& ostream) { return ostream << colorSchemeWarn << appName << " Warning: "; }
	std::ostream& appError(std::ostream& ostream) { return ostream << colorSchemeError << appName << " Error: "; }
	std::ostream& appUsage(std::ostream& ostream) { return ostream << colorSchemeUsage << appName << " Usage: "; }
	std::ostream& appHelp(std::ostream& ostream) { return ostream << colorSchemeHelp << appName << " Help: "; }
	std::ostream& normal(std::ostream& ostream) { return ostream << ansiReset(); }
}