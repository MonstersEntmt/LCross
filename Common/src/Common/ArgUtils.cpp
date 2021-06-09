#include "Common/ArgUtils.h"

#include "Common/Logger.h"
#include "Common/PrintUtils.h"
#include "Core.h"

#include <sstream>

FlagValueInfo::FlagValueInfo(const std::string& name, FlagValueType type, const std::vector<std::string>& values)
    : name(name), type(type), values(values) { }

FlagInfo::FlagInfo(const std::string& name, const std::string& desc, const std::string& note, const std::vector<FlagValueInfo>& values)
    : name(name), desc(desc), note(note), values(values) { }

std::string FlagInfo::getSyntax() const {
	std::string syntax = this->name;
	for (size_t i = 0; i < this->values.size(); i++) {
		auto& flagValueInfo = this->values[i];
		switch (flagValueInfo.type) {
		case FlagValueType::REQUIRED: syntax += " <" + flagValueInfo.name + ">"; break;
		case FlagValueType::OPTIONAL: syntax += " {" + flagValueInfo.name + "}"; break;
		case FlagValueType::MULTI: syntax += " {" + flagValueInfo.name + " ...}"; break;
		}
	}
	return syntax;
}

std::string Format::Formatter<FlagInfo>::format(const FlagInfo& flagInfo, const std::string& options) {
	std::string str = Format::format("{0}{1} Flag {2}'{3}'{0}", LogColors::Info, PrintUtils::appName(), LogColors::Arg, flagInfo.getSyntax());

	if (!flagInfo.desc.empty()) str += ": " + flagInfo.desc;

	if (!flagInfo.note.empty()) str += Format::format("\n    {0}({1}Note{0}: {2})", LogColors::Note, LogColors::NoteLabel, flagInfo.note);

	for (size_t i = 0; i < flagInfo.values.size(); i++) {
		auto& flagValueInfo   = flagInfo.values[i];
		auto& flagValueValues = flagValueInfo.values;
		if (flagValueValues.size() > 0) {
			str += Format::format("\n{} Flag values for {}'{}'{}:", PrintUtils::appName(), LogColors::Arg, flagValueInfo.name, LogColors::Info);

			size_t numChars = 0;
			for (size_t j = 0; j < flagValueValues.size(); j++)
				numChars += flagValueValues[j].length();

			if (numChars > 50) {
				str += Format::format("{}", LogColors::Arg);
				for (size_t j = 0; j < flagValueValues.size(); j++)
					str += Format::format("\n    '{}'", flagValueValues[j]);
				str += Format::format("{}", LogColors::Info);
			} else {
				for (size_t j = 0; j < flagValueValues.size(); j++) {
					if (j == 0)
						str += " ";
					else
						str += ", ";
					str += Format::format("{}'{}'{}", LogColors::Arg, flagValueValues[j], LogColors::Info);
				}
			}
		}
	}
	str += Format::format("{0:r}{0:br}", LogColors::Black);
	return str;
}

ArgUtils::ArgUtils(int argc, char** argv)
    : argc(argc), argv(argv) {
	registerFlagInfo(FlagInfo("-h", "Show this help information or if given a flag this will show info about that flag", "Flag should not contain '-' e.g. for flag '-f' give 'f' as value", { FlagValueInfo("flags", FlagValueType::MULTI, {}) }));
	registerFlagInfo(FlagInfo("-v", "Show the version of this app", "", {}));
	registerFlagInfo(FlagInfo("-verbose", "Print debug information", "", {}));
	registerFlagInfo(FlagInfo("-o", "Set output filename", "", { FlagValueInfo("output filename", FlagValueType::REQUIRED, {}) }));

	registerHandlerFunc("-h", std::bind(&ArgUtils::handleHelpFlag, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
	registerHandlerFunc("-v", std::bind(&ArgUtils::handleVersionFlag, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));
	registerHandlerFunc("", [](ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
		argUtils.inputNames.push_back(arg);
	});
	registerHandlerFunc("-verbose", [](ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
		argUtils.verbose = true;
	});
	registerHandlerFunc("-o", [](ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
		argUtils.outputName = argValues[0];
		usedValueCount      = 1;
	});
}

void ArgUtils::registerFlagInfo(const FlagInfo& argInfo) {
	for (auto& ai : this->argInfos)
		if (ai.name == argInfo.name)
			return;
	this->argInfos.push_back(argInfo);
}

void ArgUtils::registerHandlerFunc(const std::string& argName, HandlerFunc handlerFunc) {
	this->handlerFuncs.insert({ argName, handlerFunc });
}

const FlagInfo* ArgUtils::getFlagInfo(const std::string& argName) const {
	for (auto& ai : this->argInfos)
		if (ai.name == argName)
			return &ai;
	return nullptr;
}

void ArgUtils::handleHelpFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	if (argValues.size() > 0) {
		for (size_t i = 0; i < argValues.size(); i++) {
			const std::string& argValue = argValues[i];
			const FlagInfo* flagInfo    = getFlagInfo("-" + argValue);
			if (flagInfo) {
				Logger::log(LogSeverity::Help, "Help for flag {}'{}'{}", LogColors::Arg, flagInfo->name, LogColors::Help);
				Logger::log(LogSeverity::Usage, "'\"{}\" {}'", getProcessCommand(), getFullUsageString(flagInfo));
				Logger::print("{}", *flagInfo);
			} else {
				Logger::log(LogSeverity::Error, "Flag {}'-{}'{} is not a valid flag for this app!", LogColors::Arg, argValue, LogColors::Error);
				argFailed = true;
			}
		}
	} else {
		Logger::log(LogSeverity::Usage, "'\"{}\" {}'", getProcessCommand(), getFullUsageString(nullptr));
		Logger::print("{}{} Flags:", LogColors::Info, PrintUtils::appName());

		size_t maxFlagSyntaxLength = 0;
		for (size_t i = 0; i < argInfos.size(); i++) {
			auto& flagInfo = argInfos[i];
			size_t len     = flagInfo.getSyntax().length();
			if (len > maxFlagSyntaxLength)
				maxFlagSyntaxLength = len;
		}

		for (size_t i = 0; i < argInfos.size(); i++) {
			auto& flagInfo         = argInfos[i];
			std::string flagSyntax = flagInfo.getSyntax();
			Logger::print("    {}'{}'{} {}{}", LogColors::Arg, flagSyntax, LogColors::Info, flagInfo.desc.empty() ? "" : flagInfo.desc, std::string((maxFlagSyntaxLength + 1) - flagSyntax.length(), ' '));
			if (!flagInfo.note.empty())
				Logger::print("           {0}{1}({2}Note{1}: {3})", std::string(maxFlagSyntaxLength, ' '), LogColors::Note, LogColors::NoteLabel, flagInfo.note);
		}
	}
}

void ArgUtils::handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	HostInfo hostInfo = getHostInfo();
	Logger::print("{0}{1} version {2}'{3}'{0}", LogColors::Info, PrintUtils::appName(), LogColors::Arg, PrintUtils::appVersion());
	Logger::print("{}Built for {} {}", LogColors::Info, hostInfo.platform, hostInfo.arch);
	Logger::print("{0}License {1}GPLv3+{0}: GNU GPL Version 3 or later {1}<https://gnu.org/licenses/gpl.html>{0}", LogColors::Info, LogColors::Arg);
	Logger::print("{}This is free software: You are free to change and redistribute it.", LogColors::Info);
	Logger::print("{0}There is {1}NO WARRANTY{0}, to the extent permitted by law.", LogColors::Info, LogColors::Arg);
}

std::string ArgUtils::getFullUsageString(const FlagInfo* currentFlag) {
	if (currentFlag) {
		if (currentFlag->name == "-h" || currentFlag->name == "-v") {
			return currentFlag->getSyntax();
		} else {
			return "{flags {value} ...} " + currentFlag->getSyntax() + " <input filenames ...>";
		}
	} else {
		return "{flags {value} ...} <input filenames ...>";
	}
}

void ArgUtils::handleOutputName() {
	this->outputName += ".o";
}

void ArgUtils::handle() {
	for (size_t index = 1; index < this->argc; index++) {
		std::string arg = this->argv[index];
		std::vector<std::string> argValues;
		for (size_t i = index + 1; i < this->argc; i++) {
			if (this->argv[i][0] == '-')
				break;
			argValues.push_back(this->argv[i]);
		}

		bool argFailed = false;
		if (arg[0] == '-') {
			const FlagInfo* flagInfo = getFlagInfo(arg);
			if (flagInfo) {
				size_t requiredValueCount = flagInfo->values.size();
				for (auto& flagValueInfo : flagInfo->values) {
					switch (flagValueInfo.type) {
					case FlagValueType::OPTIONAL:
					case FlagValueType::MULTI:
						requiredValueCount--;
						break;
					default:
						break;
					}
				}

				if (argValues.size() < requiredValueCount) {
					if (requiredValueCount == 1) {
						Logger::log(LogSeverity::Error, "Missing value {}'<{}>'{}!", LogColors::Arg, flagInfo->values[0].name, LogColors::Error);
					} else {
						Logger::log(LogSeverity::Error, "Too few values");
					}
					argFailed = true;
				}

				if (!argFailed) {
					for (size_t i = 0; i < std::min(flagInfo->values.size(), argValues.size()); i++) {
						bool done           = false;
						auto& value         = argValues[i];
						auto& flagValueInfo = flagInfo->values[i];
						switch (flagValueInfo.type) {
						case FlagValueType::MULTI:
							done = true;
							break;
						case FlagValueType::OPTIONAL:
							done = true;
							[[fallthrough]];
						case FlagValueType::REQUIRED: {
							auto& flagValueValues = flagValueInfo.values;
							if (flagValueValues.size() > 0) {
								bool found = false;
								for (size_t j = 0; j < flagValueValues.size(); j++) {
									if (flagValueValues[j] == value) {
										found = true;
										break;
									}
								}
								if (!found) {
									Logger::log(LogSeverity::Error, "Value {0}'{1}'{2} is not a valid value of {0}'{3}'{0}", LogColors::Arg, value, LogColors::Error, flagValueInfo.name);
									argFailed = true;
									done      = true;
									break;
								}
							}
							break;
						}
						}
						if (done) break;
					}

					if (!argFailed) {
						auto handler = this->handlerFuncs.find(arg);
						if (handler != this->handlerFuncs.end()) {
							size_t indexOffset = 0;
							handler->second(*this, arg, argValues, indexOffset, argFailed);
							index += indexOffset;

							if (arg == "-h" || arg == "-v") {
								PrintUtils::restoreAnsi();
								if (argFailed)
									exit(EXIT_FAILURE);
								else
									exit(EXIT_SUCCESS);
							}
						}
					}
				}
			} else {
				Logger::log(LogSeverity::Warn, "Flag {}'{}'{} is not a valid flag for this app!", LogColors::Arg, arg, LogColors::Warn);
			}
		} else {
			auto handler = this->handlerFuncs.find("");
			if (handler != this->handlerFuncs.end()) {
				size_t indexOffset = 0;
				handler->second(*this, arg, argValues, indexOffset, argFailed);
				index += indexOffset;
			}
		}

		if (argFailed) {
			const FlagInfo* flagInfo = getFlagInfo(arg);
			Logger::log(LogSeverity::Help, "");
			Logger::log(LogSeverity::Usage, "'\"{}\" {}'", getProcessCommand(), getFullUsageString(flagInfo));
			if (flagInfo)
				Logger::print("{}", *flagInfo);

			PrintUtils::restoreAnsi();
			exit(EXIT_FAILURE);
		}
	}

	if (this->inputNames.empty()) {
		Logger::log(LogSeverity::Error, "Missing input filename!");
		Logger::log(LogSeverity::Usage, "'\"{}\" {}'", getProcessCommand(), getFullUsageString(nullptr));

		PrintUtils::restoreAnsi();
		exit(EXIT_FAILURE);
	}

	handleVirt();

	if (this->outputName.empty()) {
		this->outputName = this->inputNames[0];
		handleOutputName();
	}
}