#include "Utils/ArgUtils.h"
#include "Utils/PrintUtils.h"
#include "Core.h"

#include <iostream>

std::string getFullUsageString(ArgUtils& argUtils, const ArgInfo* argInfo) {
	std::string usage = getUsageString(argUtils);
	if (argInfo) {
		if (argInfo->argName == "-h" || argInfo->argName == "-v") {
			std::string argSyntax = argInfo->argName;
			for (size_t i = 0; i < argInfo->argValues.size(); i++) {
				auto& argValue = argInfo->argValues[i];
				auto& argValueValues = argValue.second;
				if (argValueValues.empty()) {
					argSyntax += " {" + argValue.first + "}";
				} else {
					switch (argValueValues[0][0]) {
					case 'R': argSyntax += " <" + argValue.first + ">"; break;
					case 'O': argSyntax += " {" + argValue.first + "}"; break;
					case 'M': argSyntax += " {" + argValue.first + "...}"; break;
					}
				}
			}
			return argSyntax;
		} else {
			std::string argSyntax = argInfo->argName;
			for (size_t i = 0; i < argInfo->argValues.size(); i++) {
				auto& argValue = argInfo->argValues[i];
				auto& argValueValues = argValue.second;
				if (argValueValues.empty()) {
					argSyntax += " {" + argValue.first + "}";
				} else {
					switch (argValueValues[0][0]) {
					case 'R': argSyntax += " <" + argValue.first + ">"; break;
					case 'O': argSyntax += " {" + argValue.first + "}"; break;
					case 'M': argSyntax += " {" + argValue.first + "...}"; break;
					}
				}
			}
			return "{flags {value} ...} " + argSyntax + " " + usage;
		}
	} else {
		return "{flags {value} ...} " + usage;
	}
}

std::ostream& operator<<(std::ostream& ostream, const ArgInfo& argInfo) {
	ostream << PrintUtils::colorSchemeInfo << PrintUtils::appName << " Flag " << PrintUtils::colorSchemeArg << "'" << argInfo.argName;
	for (size_t i = 0; i < argInfo.argValues.size(); i++) {
		auto& argValue = argInfo.argValues[i];
		auto& argValueValues = argValue.second;
		if (argValueValues.empty()) {
			ostream << " {" << argValue.first << "}";
		} else {
			switch (argValueValues[0][0]) {
			case 'R': ostream << " <" << argValue.first << ">"; break;
			case 'O': ostream << " {" << argValue.first << "}"; break;
			case 'M': ostream << " {" << argValue.first << " ...}"; break;
			}
		}
	}
	ostream << "'" << PrintUtils::colorSchemeInfo;

	if (!argInfo.argDesc.empty()) ostream << ": " << argInfo.argDesc;

	if (!argInfo.argNote.empty())
		ostream << std::endl << "    " << PrintUtils::colorSchemeNote << "(" << PrintUtils::colorSchemeNoteLabel << "Note" << PrintUtils::colorSchemeNote << ": " << argInfo.argNote;

	ostream << PrintUtils::colorSchemeInfo;
	for (size_t i = 0; i < argInfo.argValues.size(); i++) {
		auto& argValue = argInfo.argValues[i];
		auto& argValueValues = argValue.second;
		ostream << std::endl << PrintUtils::appName << " Flag values for " << PrintUtils::colorSchemeArg << "'" << argValue.first << "'" << PrintUtils::colorSchemeInfo << ":";

		size_t numChars = 0;
		for (size_t j = 1; j < argValueValues.size(); j++) numChars += argValueValues[j].length();

		if (numChars > 50) {
			for (size_t j = 1; j < argValueValues.size(); j++)
				ostream << std::endl << "    " << PrintUtils::colorSchemeArg << "'" << argValueValues[j] << "'" << PrintUtils::colorSchemeInfo;
		} else {
			for (size_t j = 1; j < argValueValues.size(); j++) {
				if (j == 1)
					ostream << " ";
				else
					ostream << ", ";
				ostream << PrintUtils::colorSchemeArg << "'" << argValueValues[j] << "'" << PrintUtils::colorSchemeInfo;
			}
		}
	}

	ostream << PrintUtils::normal;
	return ostream;
}

static void handleHelpArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	std::cout << PrintUtils::appHelp << PrintUtils::normal << std::endl;
	if (values.size() > 0) {
		auto* argInfo = argUtils.getArgInfo("-" + values[0]);
		std::cout << PrintUtils::appUsage << "'\"" << argUtils.getCommand() << "\" " << getFullUsageString(argUtils, argInfo) << "'" << PrintUtils::normal << std::endl;

		if (argInfo)
			std::cout << *argInfo << std::endl;
		else
			std::cout << PrintUtils::appError << "Argument " << PrintUtils::colorSchemeArg << "'-" << values[0] << "'" << PrintUtils::colorSchemeError << " is not a valid argument!" << PrintUtils::normal << std::endl;
	} else {
		auto& argInfos = argUtils.getArgInfos();
		std::cout << PrintUtils::appUsage << "'\"" << argUtils.getCommand() << "\" " << getFullUsageString(argUtils, nullptr) << "'" << PrintUtils::normal << std::endl;
		std::cout << PrintUtils::colorSchemeInfo << PrintUtils::appName << " Flags:" << std::endl;

		size_t maxArgSyntaxLength = 0;
		for (size_t i = 0; i < argInfos.size(); i++) {
			auto& argInfo = argInfos[i].second;
			std::string argSyntax = argInfo.argName;
			for (size_t i = 0; i < argInfo.argValues.size(); i++) {
				auto& argValue = argInfo.argValues[i];
				auto& argValueValues = argValue.second;
				if (argValueValues.empty()) {
					argSyntax += " {" + argValue.first + "}";
				} else {
					switch (argValueValues[0][0]) {
					case 'R': argSyntax += " <" + argValue.first + ">"; break;
					case 'O': argSyntax += " {" + argValue.first + "}"; break;
					case 'M': argSyntax += " {" + argValue.first + "...}"; break;
					}
				}
			}
			if (argSyntax.length() > maxArgSyntaxLength) maxArgSyntaxLength = argSyntax.length();
		}

		for (size_t i = 0; i < argInfos.size(); i++) {
			auto& argInfo = argInfos[i].second;
			std::string argSyntax = argInfo.argName;
			for (size_t i = 0; i < argInfo.argValues.size(); i++) {
				auto& argValue = argInfo.argValues[i];
				auto& argValueValues = argValue.second;
				if (argValueValues.empty()) {
					argSyntax += " {" + argValue.first + "}";
				} else {
					switch (argValueValues[0][0]) {
					case 'R': argSyntax += " <" + argValue.first + ">"; break;
					case 'O': argSyntax += " {" + argValue.first + "}"; break;
					case 'M': argSyntax += " {" + argValue.first + "...}"; break;
					}
				}
			}
			std::cout << "    " << PrintUtils::colorSchemeArg << "'" << argSyntax << "'" << PrintUtils::colorSchemeInfo;
			size_t requiredSpaces = (maxArgSyntaxLength + 1) - argSyntax.length();
			std::cout << std::string(requiredSpaces, ' ');
			if (!argInfo.argDesc.empty()) std::cout << argInfo.argDesc;
			if (!argInfo.argNote.empty())
				std::cout << std::endl << "           " << std::string(maxArgSyntaxLength, ' ') << PrintUtils::colorSchemeNote << "(" << PrintUtils::colorSchemeNoteLabel << "Note" << PrintUtils::colorSchemeNote << ": " << argInfo.argNote;
			std::cout << PrintUtils::normal << std::endl;
		}
		std::cout << PrintUtils::normal << std::endl;
	}
	PrintUtils::restoreAnsi();
	exit(EXIT_SUCCESS);
}

static void handleVersionArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	HostInfo hostInfo = getHostInfo();
	std::cout << PrintUtils::colorSchemeInfo << PrintUtils::appName << " version " << PrintUtils::colorSchemeArg << PrintUtils::appVersion << PrintUtils::colorSchemeInfo << std::endl << "Built for " << PrintUtils::colorSchemeArg << hostInfo.platform << " " << hostInfo.arch << PrintUtils::colorSchemeInfo << std::endl << "License " << PrintUtils::colorSchemeArg << "GPLv3+" << PrintUtils::colorSchemeInfo << ": GNU GPL Version 3 or later " << PrintUtils::colorSchemeArg << "<http://gnu.org/licenses/gpl.html>" << PrintUtils::colorSchemeInfo << std::endl << "This is free software: You are free to change and redistribute it." << std::endl << "There is " << PrintUtils::colorSchemeArg << "NO WARRANTY" << PrintUtils::colorSchemeInfo << ", to the extent permitted by law." << std::endl << "This build of " << PrintUtils::appName << " targets these formats:" << std::endl;
#if _TARGETS_PE_
	std::cout << PrintUtils::colorSchemeArg << "    'pe'" << PrintUtils::colorSchemeInfo << std::endl;
#endif
#if _TARGETS_ELF_
	std::cout << PrintUtils::colorSchemeArg << "    'elf'" << PrintUtils::colorSchemeInfo << std::endl;
#endif
#if _TARGETS_BIN_
	std::cout << PrintUtils::colorSchemeArg << "    'bin'" << PrintUtils::colorSchemeInfo << std::endl;
#endif
	std::cout << "And these architectures:" << std::endl;
#if _TARGETS_X86_
	std::cout << PrintUtils::colorSchemeArg << "    'x86'" << PrintUtils::colorSchemeInfo << std::endl;
#endif
#if _TARGETS_X86_64_
	std::cout << PrintUtils::colorSchemeArg << "    'x86_64'" << PrintUtils::colorSchemeInfo << std::endl;
#endif
#if _TARGETS_ARM32_
	std::cout << PrintUtils::colorSchemeArg << "    'arm32'" << PrintUtils::colorSchemeInfo << std::endl;
#endif
#if _TARGETS_ARM64_
	std::cout << PrintUtils::colorSchemeArg << "    'arm64'" << PrintUtils::colorSchemeInfo << std::endl;
#endif
	std::cout << PrintUtils::normal << PrintUtils::appVersionInfo;
	PrintUtils::restoreAnsi();
	exit(EXIT_SUCCESS);
}

static void handleVerboseArg(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values) {
	argUtils.setVerbose();
}

ArgUtils::ArgUtils(int argc, char** argv) {
	addArgInfo("-h", "Show this help information or if given a flag this will show info about that flag", "Flag should not contain '-' e.g. for flag '-f' give 'f' as value");
	addArgValue("-h", "flag", "O");
	addArgInfo("-v", "Show the version of this app");
	addArgInfo("-verbose", "Print debug information");
	this->argHandlers.insert({ "-h", &handleHelpArg });
	this->argHandlers.insert({ "-v", &handleVersionArg });
	this->argHandlers.insert({ "-verbose", &handleVerboseArg });
	this->arguments.clear();
	this->arguments.resize(argc);
	for (int i = 0; i < argc; i++)
		this->arguments[i] = argv[i];
}

void ArgUtils::setRequiresDefaultArgument(bool require) { this->requiresDefaultArguments = require; }

void ArgUtils::addArgInfo(const std::string& argName, const std::string& argDesc, const std::string& argNote) {
	for (auto itr = this->argInfos.begin(); itr != this->argInfos.end(); itr++)
		if (itr->first == argName)
			return;
	this->argInfos.push_back({ argName, { argName, argDesc, argNote, {} } });
}

void ArgUtils::addArgValue(const std::string& argName, const std::string& argValue, const std::string& value) {
	auto itr = this->argInfos.begin();
	for (itr; itr != this->argInfos.end(); itr++)
		if (itr->first == argName)
			break;
	if (itr != this->argInfos.end()) {
		auto argValuesItr = itr->second.argValues.begin();
		for (argValuesItr; argValuesItr != itr->second.argValues.end(); argValuesItr++)
			if (argValuesItr->first == argValue)
				break;
		if (argValuesItr != itr->second.argValues.end()) {
			argValuesItr->second.push_back(value);
		} else {
			std::vector<std::string> argValueValues;
			argValueValues.push_back(value);
			itr->second.argValues.push_back({ argValue, argValueValues });
		}
	}
}

const ArgInfo* ArgUtils::getArgInfo(const std::string& argName) const {
	auto itr = this->argInfos.begin();
	for (itr; itr != this->argInfos.end(); itr++)
		if (itr->first == argName)
			break;
	if (itr != this->argInfos.end()) return &itr->second;
	else return nullptr;
}

const std::vector<std::pair<std::string, ArgInfo>>& ArgUtils::getArgInfos() const {
	return this->argInfos;
}

void ArgUtils::addHandler(const std::string& argName, ArgHandler argHandler) {
	if (argName != "-h") this->argHandlers.insert_or_assign(argName, argHandler);
}

void ArgUtils::handle() {
	bool hasDefaultArguments = false;
	for (size_t index = 1; index < this->arguments.size(); index++) {
		std::string& arg = this->arguments[index];
		std::vector<std::string> argValues;
		for (size_t i = index + 1; i < this->arguments.size(); i++) {
			if (this->arguments[i][0] == '-')
				break;
			argValues.push_back(this->arguments[i]);
		}

		if (arg[0] == '-') {
			const ArgInfo* argInfo = getArgInfo(arg);
			if (argInfo) {
				size_t requiredArgCount = argInfo->argValues.size();
				for (auto& argValue : argInfo->argValues) {
					auto& argValueValues = argValue.second;
					if (argValueValues.size() > 0) {
						auto& argValueSyntax = argValueValues[0];
						if (argValueSyntax[0] == 'O')
							requiredArgCount--;
					}
				}

				if (argValues.size() < requiredArgCount) {
					if (requiredArgCount == 1) {
						std::cout << PrintUtils::appError << "Missing value " << PrintUtils::colorSchemeArg << "'<" << argInfo->argValues[0].first << ">'" << PrintUtils::colorSchemeError << "!" << PrintUtils::normal << std::endl;
					} else {
						// TODO: Print too few values error!
						std::cout << PrintUtils::appError << "Too few values" << PrintUtils::normal << std::endl;
					}
					this->argFailed = true;
				}

				for (size_t i = 0; i < std::min(argInfo->argValues.size(), argValues.size()); i++) {
					bool done = false;
					auto& value = argValues[i];
					auto& argValue = argInfo->argValues[i];
					auto& argValueValues = argValue.second;
					if (argValueValues.size() > 0) {
						auto& argValueSyntax = argValueValues[0];
						switch (argValueSyntax[0]) {
						case 'M':
							done = true;
							break;
						case 'O':
							done = true;
							[[fallthrough]];
						case 'R':
						{
							if (argValueValues.size() > 1) {
								bool found = false;
								for (size_t j = 1; j < argValueValues.size(); j++)
									if (argValueValues[j] == value)
										found = true;
								if (!found) {
									std::cout << PrintUtils::appError << "Value " << PrintUtils::colorSchemeArg << "'" << value << "'" << PrintUtils::colorSchemeError << " is not a valid value of " << PrintUtils::colorSchemeArg << "'<" << argInfo->argValues[0].first << ">'" << "!" << PrintUtils::normal << std::endl;
									this->argFailed = true;
									break;
								}
							}
							break;
						}
						}
					}
					if (done) break;
				}

				if (!this->argFailed) {
					auto handler = this->argHandlers.find(arg);
					if (handler != this->argHandlers.end()) {
						size_t indexOffset = 0;
						handler->second(*this, indexOffset, this->argFailed, arg, argValues);
						index += indexOffset;
					} else {
						std::cout << PrintUtils::appWarn << "Arg " << PrintUtils::colorSchemeArg << "'" << arg << "'" << PrintUtils::colorSchemeWarn << " is not a valid arg for this app!" << PrintUtils::normal << std::endl;
					}
				}
			}
		} else {
			hasDefaultArguments = true;
			auto handler = this->argHandlers.find("");
			if (handler != this->argHandlers.end()) {
				size_t indexOffset = 0;
				handler->second(*this, indexOffset, this->argFailed, arg, argValues);
				index += indexOffset;
			}
		}

		if (this->argFailed) {
			// TODO: Print arg info!
			std::cout << PrintUtils::appHelp << PrintUtils::normal << std::endl;
			const ArgInfo* argInfo = getArgInfo(arg);
			std::cout << PrintUtils::appUsage << "'\"" << getCommand() << "\" " << getFullUsageString(*this, argInfo) << "'" << PrintUtils::normal << std::endl;
			if (argInfo) std::cout << *argInfo << std::endl;
			break;
		}
	}

	if (!this->argFailed && !hasDefaultArguments) {
		std::cout << PrintUtils::appError << "Missing default argument!" << PrintUtils::normal << std::endl;
		std::cout << PrintUtils::appUsage << "'\"" << getCommand() << "\" " << getFullUsageString(*this, nullptr) << "'" << PrintUtils::normal << std::endl;
		this->argFailed = true;
	}
}

std::string ArgUtils::getCommand() const { return this->arguments[0]; }
const std::vector<std::string>& ArgUtils::getArguments() const { return this->arguments; }
bool ArgUtils::hasFailed() const { return this->argFailed; }
void ArgUtils::setVerbose() { this->verbose = true; }
bool ArgUtils::isVerbose() const { return this->verbose; }