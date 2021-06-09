#pragma once

#include <charconv>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace Format {
	namespace Detail {
		struct DefaultFormatData {
			struct FillAndAlign {
				char fill = ' ';
				enum class Align {
					LEFT,
					CENTER,
					RIGHT
				} align           = Align::LEFT;
				bool alignPresent = false;
			} fillAndAlign = {};

			enum class Sign {
				PLUSS,
				MINUS,
				SPACE
			} sign = Sign::MINUS;

			bool alternate = false;
			bool pad       = false;

			uint32_t width     = 0;
			uint32_t precision = 0;

			char type = '\0';

			std::string toString() {
				std::string str;
				if (fillAndAlign.alignPresent) {
					str += fillAndAlign.fill;
					switch (fillAndAlign.align) {
					case FillAndAlign::Align::LEFT:
						str += "<";
						break;
					case FillAndAlign::Align::CENTER:
						str += "^";
						break;
					case FillAndAlign::Align::RIGHT:
						str += ">";
						break;
					}
				}

				switch (sign) {
				case Sign::PLUSS:
					str += "+";
					break;
				case Sign::MINUS:
					str += "-";
					break;
				case Sign::SPACE:
					str += " ";
					break;
				}

				if (alternate)
					str += "#";

				if (pad)
					str += "0";

				str += std::to_string(width) + "." + std::to_string(precision) + type;
				return str;
			}

			void parse(const std::string& options) {
				size_t o = 0;

				size_t fillAndAlignOffset = options.find_first_of("<^>", o);
				if (fillAndAlignOffset < options.size()) {
					o = fillAndAlignOffset + 1;
					if (fillAndAlignOffset > 0)
						fillAndAlign.fill = options[fillAndAlignOffset - 1];
					char a = options[fillAndAlignOffset];
					switch (a) {
					case '<':
						fillAndAlign.align = FillAndAlign::Align::LEFT;
						break;
					case '^':
						fillAndAlign.align = FillAndAlign::Align::CENTER;
						break;
					case '>':
						fillAndAlign.align = FillAndAlign::Align::RIGHT;
						break;
					}
					fillAndAlign.alignPresent = true;
				}

				size_t signOffset = options.find_first_of("+- ", o);
				if (signOffset < options.size()) {
					o = signOffset + 1;
					switch (options[signOffset]) {
					case '+':
						sign = Sign::PLUSS;
						break;
					case '-':
						sign = Sign::MINUS;
						break;
					case ' ':
						sign = Sign::SPACE;
						break;
					}
				}

				size_t alternateOffset = options.find_first_of('#', o);
				if (alternateOffset < options.size()) {
					o         = alternateOffset + 1;
					alternate = true;
				}

				size_t end = options.size();
				size_t to  = options.find_last_of("sbBcdoxXaAeEfFgGp", end);
				if (to < options.size()) {
					end  = to - 1;
					type = options[to];
				}

				size_t precisionOffset = options.find_last_of('.', end);

				size_t padOffset = options.find_first_of('0', o);
				if (padOffset < options.size() && padOffset == o && padOffset < precisionOffset) {
					o   = padOffset + 1;
					pad = true;
				}

				if (precisionOffset < options.size() && precisionOffset >= o) {
					std::istringstream iss { options.substr(precisionOffset + 1, end - precisionOffset) };
					iss >> precision;
					end = precisionOffset - 1;
				}

				if (end >= o) {
					std::istringstream iss { options.substr(o, (end + 1) - o) };
					iss >> width;
				}
			}
		};
	} // namespace Detail

	template <typename T>
	struct Formatter;

	template <>
	struct Formatter<std::string_view> {
		std::string format(std::string_view value, const std::string& option) {
			Detail::DefaultFormatData data;
			data.parse(option);
			std::string str = std::string(value);
			if (data.width > str.size()) {
				switch (data.fillAndAlign.align) {
				case Detail::DefaultFormatData::FillAndAlign::Align::LEFT:
					str += std::string(data.width - str.size(), data.fillAndAlign.fill);
					break;
				case Detail::DefaultFormatData::FillAndAlign::Align::CENTER: {
					float len = (float) (data.width - str.size()) / 2;
					size_t ll = (size_t) std::floor(len);
					size_t rl = (size_t) std::ceil(len);
					str       = std::string(ll, data.fillAndAlign.fill) + str + std::string(rl, data.fillAndAlign.fill);
					break;
				}
				case Detail::DefaultFormatData::FillAndAlign::Align::RIGHT:
					str = std::string(data.width - str.size(), data.fillAndAlign.fill) + str;
					break;
				}
			}
			return str;
		}
	};

	template <>
	struct Formatter<std::string> {
		std::string format(const std::string& value, const std::string& option) {
			return Formatter<std::string_view>().format(value, option);
		}
	};

	template <>
	struct Formatter<const char*> {
		std::string format(const char* value, const std::string& option) {
			return Formatter<std::string_view>().format(value, option);
		}
	};

	template <>
	struct Formatter<char*> {
		std::string format(const char* value, const std::string& option) {
			return Formatter<std::string_view>().format(value, option);
		}
	};

	template <>
	struct Formatter<uint64_t> {
		std::string format(uint64_t value, const std::string& option) {
			Detail::DefaultFormatData tempData;
			Detail::DefaultFormatData data;
			tempData.parse(option);
			if (tempData.type == 'c')
				data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::LEFT;
			else
				data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::RIGHT;
			data.parse(option);

			auto padWithLeadingZeros = [](std::string& str, size_t num) {
				str = std::string(num, '0') + str;
			};

			std::string str;
			char buffer[64] { '\0' };
			char* end      = buffer + sizeof(buffer);
			bool upperCase = false;
			switch (data.type) {
			case 'B':
				upperCase = true;
				[[fallthrough]];
			case 'b':
				std::to_chars(buffer, end, value, 2);
				str = buffer;
				if (!data.fillAndAlign.alignPresent && data.pad && (str.size() + (data.alternate ? 2 : 0)) < data.width)
					padWithLeadingZeros(str, data.width - str.size() - (data.alternate ? 2 : 0));
				if (data.alternate) {
					if (upperCase)
						str = "0B" + str;
					else
						str = "0b" + str;
				}
				break;
			case 'c':
				str += static_cast<char>(value);
				break;
			case '\0':
				[[fallthrough]];
			case 'd':
				std::to_chars(buffer, end, value);
				str = buffer;
				if (!data.fillAndAlign.alignPresent && data.pad && (str.size() + (data.sign == Detail::DefaultFormatData::Sign::PLUSS || data.sign == Detail::DefaultFormatData::Sign::SPACE ? 1 : 0)) < data.width)
					padWithLeadingZeros(str, data.width - str.size() - (data.sign == Detail::DefaultFormatData::Sign::PLUSS || data.sign == Detail::DefaultFormatData::Sign::SPACE ? 1 : 0));
				switch (data.sign) {
				case Detail::DefaultFormatData::Sign::PLUSS:
					str = "+" + str;
					break;
				case Detail::DefaultFormatData::Sign::SPACE:
					str = " " + str;
					break;
				}
				break;
			case 'o':
				std::to_chars(buffer, end, value, 8);
				str = buffer;
				if (!data.fillAndAlign.alignPresent && data.pad && (str.size() + (data.alternate ? 1 : 0)) < data.width)
					padWithLeadingZeros(str, data.width - str.size() - (data.alternate ? 1 : 0));
				if (data.alternate) {
					if (value != 0)
						str = "0" + str;
				}
				break;
			case 'X':
				upperCase = true;
				[[fallthrough]];
			case 'x':
				std::to_chars(buffer, end, value, 16);
				str = buffer;
				if (upperCase)
					for (size_t i = 0; i < str.size(); i++)
						str[i] = (char) std::toupper(str[i]);
				if (!data.fillAndAlign.alignPresent && data.pad && (str.size() + (data.alternate ? 2 : 0)) < data.width)
					padWithLeadingZeros(str, data.width - str.size() - (data.alternate ? 2 : 0));
				if (data.alternate) {
					if (upperCase)
						str = "0X" + str;
					else
						str = "0x" + str;
				}
				break;
			}

			if (!(!data.fillAndAlign.alignPresent && data.pad) && data.width > str.size()) {
				switch (data.fillAndAlign.align) {
				case Detail::DefaultFormatData::FillAndAlign::Align::LEFT:
					str += std::string(data.width - str.size(), data.fillAndAlign.fill);
					break;
				case Detail::DefaultFormatData::FillAndAlign::Align::CENTER: {
					float len = (float) (data.width - str.size()) / 2;
					size_t ll = (size_t) std::floor(len);
					size_t rl = (size_t) std::ceil(len);
					str       = std::string(ll, data.fillAndAlign.fill) + str + std::string(rl, data.fillAndAlign.fill);
					break;
				}
				case Detail::DefaultFormatData::FillAndAlign::Align::RIGHT:
					str = std::string(data.width - str.size(), data.fillAndAlign.fill) + str;
					break;
				}
			}
			return str;
		}
	};

	template <>
	struct Formatter<uint32_t> {
		std::string format(uint32_t value, const std::string& option) {
			return Formatter<uint64_t>().format(value, option);
		}
	};

	template <>
	struct Formatter<uint16_t> {
		std::string format(uint16_t value, const std::string& option) {
			return Formatter<uint64_t>().format(value, option);
		}
	};

	template <>
	struct Formatter<uint8_t> {
		std::string format(uint8_t value, const std::string& option) {
			return Formatter<uint64_t>().format(value, option);
		}
	};

	template <>
	struct Formatter<int64_t> {
		std::string format(int64_t value, const std::string& option) {
			Detail::DefaultFormatData tempData;
			Detail::DefaultFormatData data;
			tempData.parse(option);
			if (tempData.type == 'c')
				data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::LEFT;
			else
				data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::RIGHT;
			data.parse(option);
			bool negative = false;
			if (value < 0) {
				data.sign = Detail::DefaultFormatData::Sign::PLUSS;
				value     = -value;
				negative  = true;
			}
			std::string uoptions = data.toString();

			std::string str = Formatter<uint64_t>().format(static_cast<uint64_t>(value), uoptions);
			if ((data.type == '\0' || data.type == 'd') && negative)
				str[0] = '-';
			return str;
		}
	};

	template <>
	struct Formatter<int32_t> {
		std::string format(int32_t value, const std::string& option) {
			return Formatter<int64_t>().format(value, option);
		}
	};

	template <>
	struct Formatter<int16_t> {
		std::string format(int16_t value, const std::string& option) {
			return Formatter<int64_t>().format(value, option);
		}
	};

	template <>
	struct Formatter<int8_t> {
		std::string format(int8_t value, const std::string& option) {
			return Formatter<int64_t>().format(value, option);
		}
	};

	template <>
	struct Formatter<char> {
		std::string format(char value, const std::string& option) {
			Detail::DefaultFormatData tempData;
			Detail::DefaultFormatData data;
			tempData.parse(option);
			if (tempData.type == '\0' || tempData.type == 'c')
				data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::LEFT;
			else
				data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::RIGHT;
			data.parse(option);
			if (data.type == '\0')
				data.type = 'c';
			std::string uoptions = data.toString();
			return Formatter<int8_t>().format(static_cast<int8_t>(value), uoptions);
		}
	};

	template <>
	struct Formatter<bool> {
		std::string format(bool value, const std::string& option) {
			Detail::DefaultFormatData data;
			data.parse(option);
			switch (data.type) {
			case '\0':
				[[fallthrough]];
			case 's':
				return Formatter<const char*>().format(value ? "true" : "false", option);
			case 'b':
			case 'B':
			case 'c':
			case 'd':
			case 'o':
			case 'x':
			case 'X':
				return Formatter<uint8_t>().format(static_cast<uint8_t>(value), option);
			default:
				return "";
			}
		}
	};

	template <>
	struct Formatter<long double> {
		std::string format(long double value, const std::string& option) {
			Detail::DefaultFormatData data;
			data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::RIGHT;
			data.parse(option);

			auto padWithLeadingZeros = [](std::string& str, size_t num) {
				str = std::string(num, '0') + str;
			};

			std::string str;
			char buffer[128] { '\0' };
			char* end      = buffer + sizeof(buffer);
			bool upperCase = false;
			bool negative  = value < 0;
			if (negative)
				value = -value;
			switch (data.type) {
			case 'A':
				upperCase = true;
				[[fallthrough]];
			case 'a':
				if (data.precision != 0)
					std::to_chars(buffer, end, value, std::chars_format::hex, data.precision);
				else
					std::to_chars(buffer, end, value, std::chars_format::hex);
				str = buffer;
				if (data.alternate && str != "inf" && str != "nan") {
					size_t dot = str.find_first_of('.');
					if (dot >= str.size()) {
						size_t p = str.find_first_of('p');
						str      = str.substr(0, p) + ".0" + str.substr(p);
					}
				}
				if (upperCase)
					for (size_t i = 0; i < str.size(); i++)
						str[i] = (char) std::toupper(str[i]);
				break;
			case 'E':
				upperCase = true;
				[[fallthrough]];
			case 'e':
				std::to_chars(buffer, end, value, std::chars_format::scientific, data.precision != 0 ? data.precision : 6);
				str = buffer;
				if (data.alternate && str != "inf" && str != "nan") {
					size_t dot = str.find_first_of('.');
					if (dot >= str.size()) {
						size_t e = str.find_first_of('e');
						str      = str.substr(0, e) + ".0" + str.substr(e);
					}
				}
				if (upperCase)
					for (size_t i = 0; i < str.size(); i++)
						str[i] = (char) std::toupper(str[i]);
				break;
			case 'F':
				upperCase = true;
				[[fallthrough]];
			case 'f':
				std::to_chars(buffer, end, value, std::chars_format::fixed, data.precision != 0 ? data.precision : 6);
				str = buffer;
				break;
			case 'G':
				upperCase = true;
				[[fallthrough]];
			case 'g':
				std::to_chars(buffer, end, value, std::chars_format::general, data.precision != 0 ? data.precision : 6);
				str = buffer;
				if (data.alternate && str != "inf" && str != "nan") {
					size_t dot           = str.find_first_of('.');
					size_t trailingZeros = 0;
					if (dot < str.size()) {
						size_t decimals = (str.size() - dot);
						if (decimals < data.precision)
							trailingZeros = data.precision - decimals;
					} else {
						trailingZeros = data.precision;
						str += ".";
					}
					str += std::string(trailingZeros, 0);
				}

				if (upperCase)
					for (size_t i = 0; i < str.size(); i++)
						str[i] = (char) std::toupper(str[i]);
				break;
			case '\0':
				if (data.precision != 0)
					std::to_chars(buffer, end, value, std::chars_format::general, data.precision);
				else
					std::to_chars(buffer, end, value);
				str = buffer;
				if (data.alternate && str != "inf" && str != "nan") {
					size_t dot = str.find_first_of('.');
					if (dot >= str.size())
						str += ".0";
				}
				break;
			}

			if (!data.fillAndAlign.alignPresent && data.pad && (str.size() + (!negative && data.sign == Detail::DefaultFormatData::Sign::MINUS ? 0 : 1)) < data.width)
				padWithLeadingZeros(str, data.width - str.size() - (!negative && data.sign == Detail::DefaultFormatData::Sign::MINUS ? 0 : 1));

			switch (data.sign) {
			case Detail::DefaultFormatData::Sign::PLUSS:
				if (negative)
					str = "-" + str;
				else
					str = "+" + str;
				break;
			case Detail::DefaultFormatData::Sign::MINUS:
				if (negative)
					str = "-" + str;
				break;
			case Detail::DefaultFormatData::Sign::SPACE:
				if (negative)
					str = "-" + str;
				else
					str = " " + str;
				break;
			}

			if (!(!data.fillAndAlign.alignPresent && data.pad) && data.width > str.size()) {
				switch (data.fillAndAlign.align) {
				case Detail::DefaultFormatData::FillAndAlign::Align::LEFT:
					str += std::string(data.width - str.size(), data.fillAndAlign.fill);
					break;
				case Detail::DefaultFormatData::FillAndAlign::Align::CENTER: {
					float len = (float) (data.width - str.size()) / 2;
					size_t ll = (size_t) std::floor(len);
					size_t rl = (size_t) std::ceil(len);
					str       = std::string(ll, data.fillAndAlign.fill) + str + std::string(rl, data.fillAndAlign.fill);
					break;
				}
				case Detail::DefaultFormatData::FillAndAlign::Align::RIGHT:
					str = std::string(data.width - str.size(), data.fillAndAlign.fill) + str;
					break;
				}
			}
			return str;
		}
	};

	template <>
	struct Formatter<double> {
		std::string format(double value, const std::string& option) {
			return Formatter<long double>().format(value, option);
		}
	};

	template <>
	struct Formatter<float> {
		std::string format(float value, const std::string& option) {
			return Formatter<long double>().format(value, option);
		}
	};

	template <typename T>
	struct Formatter<T*> {
		std::string format(T* value, const std::string& option) {
			Detail::DefaultFormatData data;
			data.fillAndAlign.align = Detail::DefaultFormatData::FillAndAlign::Align::RIGHT;
			data.parse(option);
			switch (data.type) {
			case '\0':
				[[fallthrough]];
			case 'p':
				data.type = 'x';
				break;
			case 'P':
				data.type = 'X';
				break;
			}
			std::string uoptions = data.toString();
			return Formatter<uintptr_t>().format(static_cast<uintptr_t>(value), uoptions);
		}
	};

	namespace Detail {
		struct FormattedArg {
			std::string str;
			bool formattable = true;
		};

		template <typename T>
		struct RemoveArray {
			using Type = T;
		};

		template <typename T, size_t N>
		struct RemoveArray<T[N]> {
			using Type = T*;
		};

		template <typename T, size_t N>
		struct RemoveArray<const T[N]> {
			using Type = const T*;
		};

		template <typename Param>
		void formatArg(size_t index, std::vector<FormattedArg>& formattedArgs, const Param& arg) {
			for (FormattedArg& formattedArg : formattedArgs) {
				if (!formattedArg.formattable)
					continue;

				std::istringstream currentFormatStream { formattedArg.str };
				size_t argIndex;
				if (currentFormatStream >> argIndex && index == argIndex) {
					size_t start = formattedArg.str.find_first_of(':');
					std::string option;
					if (start < formattedArg.str.size())
						option = formattedArg.str.substr(start + 1);

					formattedArg.str         = Formatter<typename RemoveArray<Param>::Type>().format(arg, option);
					formattedArg.formattable = false;
				}
			}
		}
	} // namespace Detail

	template <typename... Params>
	std::string format(std::string_view fmt, const Params&... args) {
		std::string formatStr = std::string(fmt);
		size_t offset         = 0;
		size_t currentIndex   = 0;
		while ((offset = formatStr.find_first_of('{', offset)) < formatStr.size()) {
			if (offset < formatStr.size() - 1 && formatStr[offset + 1] == '{') {
				offset += 2;
				continue;
			}

			size_t end = offset + 1;
			for (size_t depth = 1; end < formatStr.size(); end++) {
				switch (formatStr[end]) {
				case '{':
					depth++;
					break;
				case '}':
					depth--;
					break;
				}
				if (depth == 0)
					break;
			}

			offset++;
			size_t argIndex = formatStr.find_first_of(':', offset);
			std::istringstream currentFormatStream { formatStr.substr(offset, (argIndex < end ? argIndex : end) - offset) };
			size_t argVal;
			if (!(currentFormatStream >> argVal)) {
				std::string currentIndexStr = std::to_string(currentIndex++);
				formatStr                   = formatStr.substr(0, offset) + currentIndexStr + formatStr.substr(offset);
				offset += currentIndexStr.size();
			}
		}

		std::vector<Detail::FormattedArg> formattedArgs;
		offset = 0;
		while ((offset = formatStr.find_first_of('{', offset)) < formatStr.size()) {
			if (offset < formatStr.size() - 1 && formatStr[offset + 1] == '{') {
				offset += 2;
				continue;
			}

			size_t end = offset + 1;
			for (size_t depth = 1; end < formatStr.size(); end++) {
				switch (formatStr[end]) {
				case '{':
					depth++;
					break;
				case '}':
					depth--;
					break;
				}
				if (depth == 0)
					break;
			}

			if (end < formatStr.size())
				formattedArgs.push_back({ formatStr.substr(offset + 1, end - 1 - offset) });
			offset = end + 1;
		}

		for (Detail::FormattedArg& formattedArg : formattedArgs) {
			size_t begin = formattedArg.str.find_first_of('{');
			if (begin >= formattedArg.str.size() && !std::isdigit(formattedArg.str[begin + 1]))
				continue;
			size_t end = formattedArg.str.find_first_of('}');
			if (end >= formattedArg.str.size())
				continue;

			formattedArg.str = format<Params...>(formattedArg.str, args...);
		}

		size_t index = 0;
		(Detail::formatArg<Params>(index++, formattedArgs, args), ...);

		currentIndex = 0;
		for (size_t i = 0; i < formatStr.size(); i++) {
			switch (formatStr[i]) {
			case '{':
				if (i + 1 < formatStr.size() && formatStr[i + 1] == '{') {
					formatStr.replace(i, 2, 1, '{');
				} else {
					size_t end = i + 1;
					for (size_t depth = 1; end < formatStr.size(); end++) {
						switch (formatStr[end]) {
						case '{':
							depth++;
							break;
						case '}':
							depth--;
							break;
						}
						if (depth == 0)
							break;
					}

					i++;
					size_t argIndex = formatStr.find_first_of(':', i);
					std::istringstream currentFormatStream { formatStr.substr(i, (argIndex < end ? argIndex : end) - i) };
					size_t argVal;
					if (currentFormatStream >> argVal) {
						Detail::FormattedArg& arg = formattedArgs[currentIndex++];
						formatStr.replace(i - 1, end - (i - 2), arg.str);
						i = i - 2 + arg.str.size();
					} else {
						i = end + 1;
					}
				}
				break;
			case '}':
				if (i + 1 < formatStr.size() && formatStr[i + 1] == '}')
					formatStr.replace(i, 2, 1, '}');
				break;
			}
		}
		return formatStr;
	}
} // namespace Format