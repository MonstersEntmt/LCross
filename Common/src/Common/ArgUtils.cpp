#include "Common/ArgUtils.h"

#include "Core.h"
#include "Common/PrintUtils.h"

FlagValueInfo::FlagValueInfo(const std::string& name, FlagValueType type, const std::vector<std::string>& values)
	: name(name), type(type), values(values) {}

FlagInfo::FlagInfo(const std::string& name, const std::string& desc, const std::string& note, const std::vector<FlagValueInfo>& values)
	: name(name), desc(desc), note(note), values(values) {}

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

std::ostream& operator<<(std::ostream& ostream, const FlagInfo& flagInfo) {
	ostream << PrintUtils::colorSchemeInfo << PrintUtils::appName << " Flag " << PrintUtils::colorSchemeArg << "'" << flagInfo.getSyntax() << "'" << PrintUtils::colorSchemeInfo;

	if (!flagInfo.desc.empty()) ostream << ": " << flagInfo.desc << PrintUtils::colorSchemeInfo;

	if (!flagInfo.note.empty()) ostream << std::endl << "    " << PrintUtils::colorSchemeNote << "(" << PrintUtils::colorSchemeNoteLabel << "Note" << PrintUtils::colorSchemeNote << ": " << flagInfo.note << ")" << PrintUtils::colorSchemeInfo;

	for (size_t i = 0; i < flagInfo.values.size(); i++) {
		auto& flagValueInfo = flagInfo.values[i];
		auto& flagValueValues = flagValueInfo.values;
		if (flagValueValues.size() > 0) {
			ostream << std::endl << PrintUtils::appName << " Flag values for " << PrintUtils::colorSchemeArg << "'" << flagValueInfo.name << "'" << PrintUtils::colorSchemeInfo << ":";

			size_t numChars = 0;
			for (size_t j = 0; j < flagValueValues.size(); j++)
				numChars += flagValueValues[j].length();

			if (numChars > 50) {
				for (size_t j = 0; j < flagValueValues.size(); j++)
					ostream << std::endl << "    " << PrintUtils::colorSchemeArg << "'" << flagValueValues[j] << "'" << PrintUtils::colorSchemeInfo;
			} else {
				for (size_t j = 0; j < flagValueValues.size(); j++) {
					if (j == 0)
						ostream << " ";
					else
						ostream << ", ";
					ostream << PrintUtils::colorSchemeArg << "'" << flagValueValues[j] << "'" << PrintUtils::colorSchemeInfo;
				}
			}
		}
	}
	ostream << PrintUtils::normal;
	return ostream;
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
		usedValueCount = 1;
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
			const FlagInfo* flagInfo = getFlagInfo("-" + argValue);
			if (flagInfo) {
				std::cout << PrintUtils::appHelp << "Help for flag " << PrintUtils::colorSchemeArg << "'" << flagInfo->name << "'" << PrintUtils::colorSchemeHelp << std::endl;
				std::cout << PrintUtils::appUsage << "'\"" << getProcessCommand() << "\" " << getFullUsageString(flagInfo) << "'" << std::endl;
				std::cout << *flagInfo << PrintUtils::normal << std::endl;
			} else {
				std::cout << PrintUtils::appError << "Flag " << PrintUtils::colorSchemeArg << "'-" << argValue << "'" << PrintUtils::colorSchemeError << " is not a valid flag for this app!" << PrintUtils::normal << std::endl;
				argFailed = true;
			}
		}
	} else {
		std::cout << PrintUtils::appUsage << "'\"" << getProcessCommand() << "\" " << getFullUsageString(nullptr) << "'" << std::endl;
		std::cout << PrintUtils::colorSchemeInfo << PrintUtils::appName << " Flags:";

		size_t maxFlagSyntaxLength = 0;
		for (size_t i = 0; i < argInfos.size(); i++) {
			auto& flagInfo = argInfos[i];
			size_t len = flagInfo.getSyntax().length();
			if (len > maxFlagSyntaxLength)
				maxFlagSyntaxLength = len;
		}

		for (size_t i = 0; i < argInfos.size(); i++) {
			auto& flagInfo = argInfos[i];
			std::string flagSyntax = flagInfo.getSyntax();
			std::cout << std::endl << "    " << PrintUtils::colorSchemeArg << "'" << flagSyntax << "'" << PrintUtils::colorSchemeInfo;
			size_t requiredSpaces = (maxFlagSyntaxLength + 1) - flagSyntax.length();
			std::cout << std::string(requiredSpaces, ' ');
			if (!flagInfo.desc.empty()) std::cout << flagInfo.desc;
			if (!flagInfo.note.empty()) std::cout << std::endl << "           " << std::string(maxFlagSyntaxLength, ' ') << PrintUtils::colorSchemeNote << "(" << PrintUtils::colorSchemeNoteLabel << "Note" << PrintUtils::colorSchemeNote << ": " << flagInfo.note << ")";
		}
		std::cout << PrintUtils::normal << std::endl;
	}
}

void ArgUtils::handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed) {
	HostInfo hostInfo = getHostInfo();
	std::cout << PrintUtils::colorSchemeInfo << PrintUtils::appName << " version " << PrintUtils::colorSchemeArg << PrintUtils::appVersion << PrintUtils::colorSchemeInfo << std::endl;
	std::cout << "Built for " << PrintUtils::colorSchemeArg << hostInfo.platform << " " << hostInfo.arch << PrintUtils::colorSchemeInfo << std::endl;
	std::cout << "License " << PrintUtils::colorSchemeArg << "GPLv3+" << PrintUtils::colorSchemeInfo << ": GNU GPL Version 3 or later " << PrintUtils::colorSchemeArg << "<https://gnu.org/licenses/gpl.html>" << PrintUtils::colorSchemeInfo << std::endl;
	std::cout << "This is free software: You are free to change and redistribute it." << std::endl;
	std::cout << "There is " << PrintUtils::colorSchemeArg << "NO WARRANTY" << PrintUtils::colorSchemeInfo << ", to the extent permitted by law." << PrintUtils::normal << std::endl;
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
						std::cout << PrintUtils::appError << "Missing value " << PrintUtils::colorSchemeArg << "'<" << flagInfo->values[0].name << ">'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
					} else {
						// TODO: Print too few values error!
						std::cout << PrintUtils::appError << "Too few values" << PrintUtils::normal << std::endl;
					}
					argFailed = true;
				}

				if (!argFailed) {
					for (size_t i = 0; i < std::min(flagInfo->values.size(), argValues.size()); i++) {
						bool done = false;
						auto& value = argValues[i];
						auto& flagValueInfo = flagInfo->values[i];
						switch (flagValueInfo.type) {
						case FlagValueType::MULTI:
							done = true;
							break;
						case FlagValueType::OPTIONAL:
							done = true;
							[[fallthrough]];
						case FlagValueType::REQUIRED:
						{
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
									std::cout << PrintUtils::appError << "Value " << PrintUtils::colorSchemeArg << "'" << value << "'" << PrintUtils::colorSchemeError << " is not a valid value of " << PrintUtils::colorSchemeArg << "'" << flagValueInfo.name << ">'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
									argFailed = true;
									done = true;
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
								if (argFailed) exit(EXIT_FAILURE);
								else exit(EXIT_SUCCESS);
							}
						}
					}
				}
			} else {
				std::cout << PrintUtils::appWarn << "Flag " << PrintUtils::colorSchemeArg << "'" << arg << "'" << PrintUtils::colorSchemeWarn << " is not a valid flag for this app!" << PrintUtils::normal << std::endl;
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
			std::cout << PrintUtils::appHelp << PrintUtils::normal << std::endl;
			const FlagInfo* flagInfo = getFlagInfo(arg);
			std::cout << PrintUtils::appUsage << "'\"" << getProcessCommand() << "\" " << getFullUsageString(flagInfo) << "'" << PrintUtils::normal << std::endl;
			if (flagInfo) std::cout << *flagInfo << std::endl;
			throw std::exception();
		}
	}

	if (this->inputNames.empty()) {
		std::cout << PrintUtils::appError << "Missing input filename!" << PrintUtils::normal << std::endl;
		std::cout << PrintUtils::appUsage << "'\"" << getProcessCommand() << "\" " << getFullUsageString(nullptr) << "'" << PrintUtils::normal << std::endl;
		throw std::exception();
	}

	handleVirt();

	if (this->outputName.empty()) {
		this->outputName = this->inputNames[0];
		handleOutputName();
	}
}