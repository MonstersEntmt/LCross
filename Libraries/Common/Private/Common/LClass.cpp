#include "LCross/Common/LClass.hpp"
#include "LCross/Common/ByteBuffer.hpp"
#include "LCross/Common/FileHelper.hpp"

#include <unordered_map>

namespace LCross::Common::LClass {
	namespace V1 {
		bool read(ByteBuffer& inBytes, LClass& lclass);
	}

	void writeLClass(const std::filesystem::path& filepath, const LClass& lclass) {
		std::unordered_map<std::string, std::uint16_t> stringToIndex;
		std::unordered_map<std::string, std::uint16_t> classToIndex;
		ByteBuffer bytes;

		bytes.addUI4(0x484F544C);
		bytes.addUI2(1);
		stringToIndex.insert({ lclass.m_ClassName, 0 });
		classToIndex.insert({ lclass.m_ClassName, 0 });
		for (auto& super : lclass.m_Supers) {
			stringToIndex.insert({ super, 0 });
			classToIndex.insert({ super, 0 });
		}

		for (auto& field : lclass.m_Fields) {
			stringToIndex.insert({ field.m_Id, 0 });
			for (auto& attribute : field.m_Attributes)
				stringToIndex.insert({ attribute.m_Name, 0 });
		}

		for (auto& method : lclass.m_Methods) {
			stringToIndex.insert({ method.m_Id, 0 });
			for (auto& attribute : method.m_Attributes)
				stringToIndex.insert({ attribute.m_Name, 0 });
		}

		for (auto& attribute : lclass.m_Attributes)
			stringToIndex.insert({ attribute.m_Name, 0 });

		bytes.addUI2(stringToIndex.size() + classToIndex.size() + 1);
		std::uint16_t currentIndex = 0;
		for (auto& str : stringToIndex) {
			bytes.addUI1(2); // UTF8 tag = 2
			bytes.addUI4(static_cast<std::uint32_t>(str.first.size()));
			bytes.addString(str.first);
			str.second = currentIndex;
			++currentIndex;
		}
		for (auto& cls : classToIndex) {
			auto itr = stringToIndex.find(cls.first);
			if (itr == stringToIndex.end())
				throw std::runtime_error("Unexpeced");
			bytes.addUI1(1);
			bytes.addUI2(itr->second);
			++currentIndex;
		}

		bytes.addUI2(static_cast<std::uint16_t>(lclass.m_ClassFlags));
		{
			auto itr = classToIndex.find(lclass.m_ClassName);
			if (itr == classToIndex.end())
				throw std::runtime_error("Unexpected");
			bytes.addUI2(itr->second);
		}

		bytes.addUI2(static_cast<std::uint16_t>(lclass.m_Supers.size()));
		for (auto& super : lclass.m_Supers) {
			auto itr = classToIndex.find(super);
			if (itr == classToIndex.end())
				throw std::runtime_error("Unexpected");
			bytes.addUI2(itr->second);
		}

		bytes.addUI2(static_cast<std::uint16_t>(lclass.m_Fields.size()));
		for (auto& field : lclass.m_Fields) {
			bytes.addUI2(static_cast<std::uint16_t>(field.m_Flags));
			auto itr = stringToIndex.find(field.m_Id);
			if (itr == stringToIndex.end())
				throw std::runtime_error("Unexpected");
			bytes.addUI2(itr->second);
			bytes.addUI2(static_cast<std::uint16_t>(field.m_Attributes.size()));
			for (auto& attribute : field.m_Attributes) {
				auto attrId = stringToIndex.find(attribute.m_Name);
				if (attrId == stringToIndex.end())
					throw std::runtime_error("Unexpected");
				bytes.addUI2(attrId->second);
				bytes.addUI4(static_cast<std::uint32_t>(attribute.m_Data.size()));
				bytes.addUI1s(attribute.m_Data);
			}
		}

		bytes.addUI2(static_cast<std::uint16_t>(lclass.m_Methods.size()));
		for (auto& method : lclass.m_Methods) {
			bytes.addUI2(static_cast<std::uint16_t>(method.m_Flags));
			auto itr = stringToIndex.find(method.m_Id);
			if (itr == stringToIndex.end())
				throw std::runtime_error("Unexpected");
			bytes.addUI2(itr->second);
			bytes.addUI2(static_cast<std::uint16_t>(method.m_Attributes.size()));
			for (auto& attribute : method.m_Attributes) {
				auto attrId = stringToIndex.find(attribute.m_Name);
				if (attrId == stringToIndex.end())
					throw std::runtime_error("Unexpected");
				bytes.addUI2(attrId->second);
				bytes.addUI4(static_cast<std::uint32_t>(attribute.m_Data.size()));
				bytes.addUI1s(attribute.m_Data);
			}
		}

		bytes.addUI2(static_cast<std::uint16_t>(lclass.m_Attributes.size()));
		for (auto& attribute : lclass.m_Attributes) {
			auto attrId = stringToIndex.find(attribute.m_Name);
			if (attrId == stringToIndex.end())
				throw std::runtime_error("Unexpected");
			bytes.addUI2(attrId->second);
			bytes.addUI4(static_cast<std::uint32_t>(attribute.m_Data.size()));
			bytes.addUI1s(attribute.m_Data);
		}

		writeBinaryFile(filepath, bytes);
	}

	bool readLClass(const std::filesystem::path& filepath, LClass& lclass) {
		ByteBuffer inBytes;

		readBinaryFile(filepath, inBytes);

		if (inBytes.getUI4() != 0x484F544C)
			return false;

		std::uint16_t version = inBytes.getUI2();
		switch (version) {
		case 1:
			return V1::read(inBytes, lclass);
		default:
			return false;
		}
	}

	namespace V1 {
		bool read(ByteBuffer& inBytes, LClass& lclass) {
			return true;
		}
	} // namespace V1
} // namespace LCross::Common::LClass
