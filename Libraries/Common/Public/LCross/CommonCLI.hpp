#pragma once

#include <cstdint>

#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace CommonCLI {
	class HandlerMessage;

	namespace KeyValue {
		namespace Help {
			class HelpInfo;
			class Help;
		} // namespace Help
		namespace Version {
			class Version;
		}
		class ProcessedArgument;
		class ProcessedArguments;
		enum class EHandlerValueMode;
		class HandlerValue;
		class HandlerKey;
		class Handler;
	} // namespace KeyValue

	class HandlerMessage {
	public:
		HandlerMessage(std::string&& str, bool error = false) : m_Str(std::move(str)), b_Error(error) { }

		auto& getStr() const { return m_Str; }
		bool isError() const { return b_Error; }

	private:
		std::string m_Str;
		bool b_Error;
	};

	namespace KeyValue {
		namespace Help {
			class HelpInfo {
			public:
				HelpInfo(std::string&& desc = {}, std::string&& note = {}) : m_Desc(std::move(desc)), m_Note(std::move(note)) { }

				auto& getDesc() const { return m_Desc; }
				auto& getNote() const { return m_Note; }

			private:
				std::string m_Desc;
				std::string m_Note;
			};

			class Help {
			public:
				void invoke(HandlerKey& self, Handler& handler, const std::vector<std::string_view>& values, std::size_t& usedValueCount, std::size_t argc, const char** argv);

				std::string formatKey(const HandlerKey& key) const;
			};
		} // namespace Help

		namespace Version {
			class Version {
			public:
				void invoke(HandlerKey& self, Handler& handler, const std::vector<std::string_view>& values, std::size_t& usedValueCount, std::size_t argc, const char** argv);

				std::string getVersionStr() const {
					std::string str = m_Prefix;
					if (!str.empty()) str += "-";
					str += std::to_string(m_Major) + "." + std::to_string(m_Minor) + "." + std::to_string(m_Patch);
					if (!m_Suffix.empty()) str += "-" + m_Suffix;
					return str;
				}

				void setVersion(std::size_t major, std::size_t minor, std::size_t patch, std::string&& prefix = {}, std::string&& suffix = {}) {
					m_Major  = major;
					m_Minor  = minor;
					m_Patch  = patch;
					m_Prefix = prefix;
					m_Suffix = suffix;
				}

			private:
				std::size_t m_Major, m_Minor, m_Patch;
				std::string m_Prefix;
				std::string m_Suffix;
			};
		} // namespace Version

		class ProcessedArgument {
		public:
			ProcessedArgument(std::string&& arg) : m_Arg(std::move(arg)), b_Present(false) { }

			auto& getArg() const { return m_Arg; }
			bool isPresent() const { return b_Present; }
			auto& getValues() const { return m_Values; }

			void setPresent() { b_Present = true; }
			void setValues(std::vector<std::string>&& values) { m_Values = std::move(values); }

		private:
			std::string m_Arg;
			bool b_Present;
			std::vector<std::string> m_Values;
		};

		class ProcessedArguments {
		public:
			ProcessedArguments(std::string&& procCall, std::vector<ProcessedArgument>&& processedArgs, std::vector<HandlerMessage>&& messages, std::vector<std::string>&& defaultValues) : m_ProcCall(std::move(procCall)), m_ProcessedArgs(std::move(processedArgs)), m_Messages(std::move(messages)), m_DefaultValues(std::move(defaultValues)) { }

			auto& getProcCall() const { return m_ProcCall; }
			auto& getProcessedArguments() const { return m_ProcessedArgs; }
			auto& getMessages() const { return m_Messages; }
			auto& getDefaultValues() const { return m_DefaultValues; }

			ProcessedArgument* getProcessedArgument(std::string_view key) const {
				for (auto& arg : m_ProcessedArgs)
					if (arg.getArg() == key)
						return const_cast<ProcessedArgument*>(&arg);
				return nullptr;
			}

		private:
			std::string m_ProcCall;
			std::vector<ProcessedArgument> m_ProcessedArgs;
			std::vector<HandlerMessage> m_Messages;
			std::vector<std::string> m_DefaultValues;
		};

		enum class EHandlerValueMode {
			Required,       // Always 1
			Optional,       // ?
			Multi_Required, // +
			Multi           // *
		};

		class HandlerValue {
		public:
			HandlerValue(std::string&& value, EHandlerValueMode mode = EHandlerValueMode::Required, std::initializer_list<std::string>&& possibleValues = {}, Help::HelpInfo&& help = {}) : m_Value(std::move(value)), m_Mode(mode), m_PossibleValues(std::move(possibleValues)), m_Help(std::move(help)) { }

			auto& getValue() const { return m_Value; }
			auto getMode() const { return m_Mode; }
			auto& getPossibleValues() const { return m_PossibleValues; }
			auto& getHelp() const { return m_Help; }

		private:
			std::string m_Value;
			EHandlerValueMode m_Mode;
			std::vector<std::string> m_PossibleValues;
			Help::HelpInfo m_Help;
		};

		class HandlerKey {
		public:
			using Callback = std::function<void(HandlerKey& self, Handler& handler, const std::vector<std::string_view>& values, std::size_t& usedValueCount, std::size_t argc, const char** argv)>;

		public:
			HandlerKey(std::string&& key, Callback&& callback = nullptr, std::initializer_list<HandlerValue>&& values = {}, Help::HelpInfo&& help = {}) : m_Key(std::move(key)), m_Callback(callback), m_Values(std::move(values)), m_Help(std::move(help)) { }

			auto& getKey() const { return m_Key; }
			auto& getValues() const { return m_Values; }
			auto& getHelp() const { return m_Help; }

			void invoke(Handler& handler, const std::vector<std::string_view>& values, std::size_t& usedValueCount, std::size_t argc, const char** argv) { m_Callback(*this, handler, values, usedValueCount, argc, argv); }

		private:
			std::string m_Key;
			std::vector<HandlerValue> m_Values;
			Help::HelpInfo m_Help;
			Callback m_Callback;
		};

		class Handler {
		public:
			Handler(std::string&& programName, std::string&& programDesc = {})
			    : m_ProgramName(std::move(programName)), m_ProgramDesc(std::move(programDesc)) {
				addKey(HandlerKey { "help", std::bind(&Help::Help::invoke, &m_Help, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6), { HandlerValue { "key", EHandlerValueMode::Multi, {} } }, Help::HelpInfo { "Shows this help page", "You can give this argument the same key multiple times" } });
				addKey(HandlerKey { "version", std::bind(&Version::Version::invoke, &m_Version, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6), { HandlerValue { "silent", EHandlerValueMode::Optional, { "silent" } } }, Help::HelpInfo { "Shows the version of this application", "Giving 'silent' as a parameter will only print the version number" } });
			}

			ProcessedArguments handle(std::size_t argc, const char** argv);

			virtual void addKey(HandlerKey&& key) { m_Keys.push_back(std::move(key)); }

			auto& getProgramName() const { return m_ProgramName; }
			auto& getProgramDesc() const { return m_ProgramDesc; }
			auto& getKeys() const { return m_Keys; }
			auto& getHelp() { return m_Help; }
			auto& getHelp() const { return m_Help; }
			auto& getVersion() { return m_Version; }
			auto& getVersion() const { return m_Version; }

			HandlerKey* getKey(std::string_view key) const {
				for (auto& k : m_Keys)
					if (k.getKey() == key)
						return const_cast<HandlerKey*>(&k);
				return nullptr;
			}

		private:
			std::string m_ProgramName;
			std::string m_ProgramDesc;
			std::vector<HandlerKey> m_Keys;
			Help::Help m_Help;
			Version::Version m_Version;
		};
	} // namespace KeyValue
} // namespace CommonCLI
