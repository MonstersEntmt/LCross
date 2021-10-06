#include "LCross/Common/FileHelper.hpp"

#include <fstream>

namespace LCross::Common {
	std::size_t readFile(const std::filesystem::path& filepath, std::string& fileContent) {
		std::ifstream file(filepath, std::ios::ate);
		if (file.is_open()) {
			std::size_t len = file.tellg();
			file.seekg(0);
			std::size_t offset = fileContent.size();
			fileContent.resize(offset + len);
			file.read(fileContent.data() + offset, len);
			file.close();
			fileContent.shrink_to_fit();
			return len;
		}
		return 0;
	}

	std::size_t readBinaryFile(const std::filesystem::path& filepath, ByteBuffer& bytes) {
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);
		if (file.is_open()) {
			std::size_t len = file.tellg();
			file.seekg(0);
			std::size_t offset = bytes.size();
			bytes.resize(offset + len);
			file.read(reinterpret_cast<char*>(bytes.data()) + offset, len);
			file.close();
			return len;
		}
		return 0;
	}

	void writeFile(const std::filesystem::path& filepath, std::string_view fileContent) {
		std::ofstream file(filepath);
		if (file.is_open()) {
			file.write(fileContent.data(), fileContent.size());
			file.close();
		}
	}

	void writeBinaryFile(const std::filesystem::path& filepath, const ByteBuffer& bytes) {
		std::ofstream file(filepath, std::ios::binary);
		if (file.is_open()) {
			file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
			file.close();
		}
	}

	void appendFile(const std::filesystem::path& filepath, std::string_view fileContent) {
		std::ofstream file(filepath, std::ios::app);
		if (file.is_open()) {
			file.write(fileContent.data(), fileContent.size());
			file.close();
		}
	}

	void appendBinaryFile(const std::filesystem::path& filepath, const ByteBuffer& bytes) {
		std::ofstream file(filepath, std::ios::binary | std::ios::app);
		if (file.is_open()) {
			file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
			file.close();
		}
	}

	void deleteFile(const std::filesystem::path& filepath) {
		std::filesystem::remove(filepath);
	}
} // namespace LCross::Common
