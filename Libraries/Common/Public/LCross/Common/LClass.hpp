#pragma once

#include <cstdint>

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace LCross::Common::LClass {
	enum class EClassFlags : std::uint16_t {
		Default = 0
	};

	enum class EFieldFlags : std::uint16_t {
		Default = 0
	};

	struct Attribute {
	public:
		std::string m_Name;
		std::vector<std::uint8_t> m_Data;
	};

	struct Field {
	public:
		EFieldFlags m_Flags;
		std::string m_Id;
		std::vector<Attribute> m_Attributes;
	};

	struct Method {
	public:
		EFieldFlags m_Flags;
		std::string m_Id;
		std::vector<Attribute> m_Attributes;
	};

	struct LClass {
	public:
		EClassFlags m_ClassFlags;
		std::string m_ClassName;
		std::vector<std::string> m_Supers;
		std::vector<Field> m_Fields;
		std::vector<Method> m_Methods;
		std::vector<Attribute> m_Attributes;

		// UI4 = 0x484F544C        : Magic Number
		// UI2                     : Version
		// UI2                     : Constant pool size + 1
		//  UI1                    : Constant pool entry tag
		//  []                     : Constant pool entry data
		// 0..Constant pool size
		// UI2                     : Class flags
		// UI2                     : This class
		// UI2                     : Super count
		// UI2[Super count]        : Supers
		// UI2                     : Field count
		//  UI2                    : Field flags
		//  UI2                    : Field Id entry
		//  UI2                    : Field Attribute count
		//   UI2                   : Attribute name entry
		//   UI4                   : Attribute length
		//   UI1[Attribute length] : Attribute data
		//  0..Field Attribute count
		// 0..Field count
		// UI2                     : Method count
		//  UI2                    : Method flags
		//  UI2                    : Method Id entry
		//  UI2                    : Method Attribute count
		//   UI2                   : Attribute name entry
		//   UI4                   : Attribute length
		//   UI1[Attribute length] : Attribute data
		//  0..Method Attribute count
		// 0..Method count
		// UI2                     : Attribute count
		//  UI2                    : Attribute name entry
		//  UI4                    : Attribute length
		//  UI1[Attribute length]  : Attribute data
		// 0..Attribute count
	};

	void writeLClass(const std::filesystem::path& filepath, const LClass& lclass);
	bool readLClass(const std::filesystem::path& filepath, LClass& lclass);
} // namespace LCross::Common::LClass
