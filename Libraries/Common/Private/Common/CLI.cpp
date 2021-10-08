#include "LCross/Common/CLI.hpp"

#include <cstdint>

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace CommonCLI {
	namespace KeyValue {
		namespace Help {
			void Help::invoke(HandlerKey& self, Handler& handler, const std::vector<std::string_view>& values, std::size_t& usedValueCount, std::size_t argc, const char** argv) {
				std::ostringstream helpStr;

				if (values.empty()) {
					if (!handler.getProgramDesc().empty()) helpStr << handler.getProgramDesc() << '\n';
					helpStr << handler.getProgramName() << " Syntax: " << argv[0] << " (--key (value ...) ...)\n";
					helpStr << handler.getProgramName() << " Keys:\n";

					auto& keys = handler.getKeys();
					std::vector<std::string> keyStrings;
					keyStrings.reserve(handler.getKeys().size());
					std::size_t largestStrlen = 0;
					for (auto& key : keys) {
						std::string keyString = "    - " + formatKey(key);
						std::size_t strlen    = keyString.size();
						keyStrings.push_back(std::move(keyString));
						if (strlen > largestStrlen) largestStrlen = strlen + 1;
					}

					for (std::size_t i = 0; i < keyStrings.size(); ++i) {
						auto& keyString = keyStrings[i];
						auto& key       = keys[i];
						helpStr << keyString;
						std::size_t spaces = largestStrlen - keyString.size();
						helpStr << std::string(spaces, ' ') << '|';
						auto& help = key.getHelp();
						if (!help.getDesc().empty()) helpStr << ' ' << help.getDesc() << '\n';
						if (!help.getNote().empty()) helpStr << std::string(largestStrlen, ' ') << "|     (Note: " << help.getNote() << ")\n";
					}
				} else {
					std::vector<HandlerKey*> keys;
					for (auto& value : values) {
						auto key = handler.getKey(value);
						if (key == nullptr) {
							helpStr << handler.getProgramName() << " Doesn't contain the key '" << value << "'\n";
							continue;
						}
						keys.push_back(key);
					}

					helpStr << handler.getProgramName() << " Keys:\n";
					std::vector<std::string> keyStrings;
					keyStrings.reserve(handler.getKeys().size());
					std::size_t largestStrlen = 0;
					for (auto& key : keys) {
						std::string keyString = "    - " + formatKey(*key);
						std::size_t strlen    = keyString.size();
						keyStrings.push_back(std::move(keyString));
						if (strlen > largestStrlen) largestStrlen = strlen + 1;
					}

					for (std::size_t i = 0; i < keyStrings.size(); ++i) {
						auto& keyString = keyStrings[i];
						auto& key       = keys[i];
						helpStr << keyString;
						std::size_t spaces = largestStrlen - keyString.size();
						helpStr << std::string(spaces, ' ') << '|';
						auto& help = key->getHelp();
						if (!help.getDesc().empty()) helpStr << ' ' << help.getDesc() << '\n';
						if (!help.getNote().empty()) helpStr << std::string(largestStrlen, ' ') << "|     (Note: " << help.getNote() << ")\n";
					}

					usedValueCount = values.size();
				}

				std::cout << helpStr.str();
			}

			std::string Help::formatKey(const HandlerKey& key) const {
				std::string str = key.getKey();

				auto& values = key.getValues();
				if (!values.empty()) {
					str += " ";
					bool space = false;
					for (auto& value : values) {
						if (space) str += " ";
						space = true;

						auto mode = value.getMode();
						switch (mode) {
						case EHandlerValueMode::Required:
							[[fallthrough]];
						case EHandlerValueMode::Multi_Required:
							str += "[";
							break;
						case EHandlerValueMode::Optional:
							[[fallthrough]];
						case EHandlerValueMode::Multi:
							str += "(";
							break;
						}

						str += value.getValue();

						switch (mode) {
						case EHandlerValueMode::Required:
							str += "]";
							break;
						case EHandlerValueMode::Optional:
							str += ")";
							break;
						case EHandlerValueMode::Multi_Required:
							str += " ...]";
							break;
						case EHandlerValueMode::Multi:
							str += " ...)";
							break;
						}
					}
				}
				return str;
			}
		} // namespace Help

		namespace Version {
			void Version::invoke(HandlerKey& self, Handler& handler, const std::vector<std::string_view>& values, std::size_t& usedValueCount, std::size_t argc, const char** argv) {
				std::ostringstream versionStr;

				if (values.size() > 0 && values[0] == "silent")
					versionStr << getVersionStr() << '\n';
				else
					versionStr << handler.getProgramName() << " version is " << getVersionStr() << '\n';

				std::cout << versionStr.str();
			}
		} // namespace Version

		ProcessedArguments Handler::handle(std::size_t argc, const char** argv) {
			std::vector<ProcessedArgument> processedArgs;
			processedArgs.reserve(m_Keys.size());
			for (auto& key : m_Keys) processedArgs.push_back({ std::string { key.getKey() } });

			std::vector<HandlerMessage> messages;
			std::vector<std::string> defaultValues;

			std::size_t keyIndex = 0;
			for (std::size_t i = 1; i < argc; ++i, ++keyIndex) {
				std::string_view currentKey = argv[i];
				if (currentKey.size() < 2 || currentKey[0] != '-' || currentKey[1] != '-') {
					defaultValues.push_back(std::string { currentKey });
					continue;
				}
				std::string_view currentKeyValue = currentKey.substr(2);

				auto keyInfo = getKey(currentKeyValue);
				if (keyInfo == nullptr) {
					messages.push_back({ "Unknown key '" + std::string { currentKeyValue } + "'", true });
					break;
				}

				auto& keyValues = keyInfo->getValues();
				std::vector<std::string_view> values;
				bool failed          = false;
				bool haveAllRequired = false;
				for (std::size_t j = 0; j < keyValues.size(); ++j) {
					auto& keyValue        = keyValues[j];
					std::size_t loopCount = i + 1;
					if (keyValue.getMode() == EHandlerValueMode::Multi_Required || keyValue.getMode() == EHandlerValueMode::Multi)
						loopCount = static_cast<std::size_t>(-1ULL);
					bool first = true;
					for (std::size_t k = 0; i < loopCount; ++i, ++k) {
						bool hasValue = (i + 1) < argc;
						if (hasValue) {
							std::string_view currentValue = hasValue ? argv[i + 1] : "";
							if (currentValue.size() >= 2 && currentValue[0] == '-' && currentValue[1] == '-') {
								if (!haveAllRequired && j < keyValues.size() - 1) {
									auto& nextKeyValue = keyValues[j + 1];
									if (nextKeyValue.getMode() == EHandlerValueMode::Required || (first && nextKeyValue.getMode() == EHandlerValueMode::Multi_Required)) {
										messages.push_back({ "Key '" + std::string { currentKeyValue } + "' at '" + std::to_string(keyIndex + 1) + "' is missing required parameters", true });
										failed = true;
									}
								}
								break;
							}

							if (!haveAllRequired && keyValue.getMode() != EHandlerValueMode::Required && (first && keyValue.getMode() != EHandlerValueMode::Multi_Required)) haveAllRequired = true;

							auto& keyPossibleValues = keyValue.getPossibleValues();
							if (!keyPossibleValues.empty()) {
								bool found = false;
								for (auto& posVal : keyPossibleValues) {
									if (posVal == currentValue) {
										found = true;
										break;
									}
								}

								if (keyValue.getMode() == EHandlerValueMode::Required || (first && keyValue.getMode() == EHandlerValueMode::Multi_Required)) {
									if (!found) {
										messages.push_back({ "Value '" + keyValue.getValue() + "' at '" + std::to_string(k + 1) + "' of key '" + std::string { currentKeyValue } + "' at '" + std::to_string(keyIndex + 1) + "' is not one of the possible values", true });
										failed = true;
										break;
									}
								} else if (!found) {
									break;
								}
							}

							values.push_back(currentValue);
							first = false;
						} else if (keyValue.getMode() == EHandlerValueMode::Required || (first && keyValue.getMode() == EHandlerValueMode::Multi_Required)) {
							messages.push_back({ "Value '" + keyValue.getValue() + "' at '" + std::to_string(k + 1) + "' of key '" + std::string { currentKeyValue } + "' at '" + std::to_string(keyIndex + 1) + "' is not one of the possible values", true });
							failed = true;
							break;
						} else {
							break;
						}
					}
				}

				if (failed) break;

				std::size_t usedValueCount = 0;
				keyInfo->invoke(*this, values, usedValueCount, argc, argv);

				for (auto& processedArg : processedArgs) {
					if (processedArg.getArg() == currentKeyValue) {
						processedArg.setPresent();
						std::vector<std::string> actualValues;
						actualValues.resize(std::min(usedValueCount, values.size()));
						for (std::size_t k = 0; k < actualValues.size(); ++k)
							actualValues[k] = values[k];
						processedArg.setValues(std::move(actualValues));
						break;
					}
				}
			}

			return { { argv[0] }, std::move(processedArgs), std::move(messages), std::move(defaultValues) };
		}
	} // namespace KeyValue
} // namespace CommonCLI
