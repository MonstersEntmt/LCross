#include "Common/FileUtils.h"

#include <filesystem>

namespace FileUtils {
	size_t readFile(const std::string& filename, std::string& fileContent) {
		FILE* file = fopen(filename.c_str(), "r");
		if (file) {
			fseek(file, 0, SEEK_END);
			size_t len = ftell(file);
			fseek(file, 0, SEEK_SET);
			size_t off = fileContent.size();
			fileContent.resize(off + len);
			len = fread(fileContent.data() + off, sizeof(char), len, file);
			fclose(file);
			return len;
		}
		return 0;
	}

	size_t readFileBinary(const std::string& filename, std::vector<uint8_t>& fileContent) {
		FILE* file = fopen(filename.c_str(), "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			size_t len = ftell(file);
			fseek(file, 0, SEEK_SET);
			size_t off = fileContent.size();
			fileContent.resize(off + len);
			len = fread(fileContent.data() + off, sizeof(uint8_t), len, file);
			fclose(file);
			return len;
		}
		return 0;
	}

	size_t writeFile(const std::string& filename, const std::string& fileContent) {
		FILE* file = fopen(filename.c_str(), "w");
		if (file) {
			size_t len = fwrite(fileContent.data(), sizeof(char), fileContent.length(), file);
			fclose(file);
			return len;
		}
		return 0;
	}

	size_t appendFile(const std::string& filename, const std::string& fileContent) {
		FILE* file = fopen(filename.c_str(), "a");
		if (file) {
			size_t len = fwrite(fileContent.data(), sizeof(char), fileContent.length(), file);
			fclose(file);
			return len;
		}
		return 0;
	}

	size_t writeFileBinary(const std::string& filename, const std::vector<uint8_t>& fileContent) {
		FILE* file = fopen(filename.c_str(), "wb");
		if (file) {
			size_t len = fwrite(fileContent.data(), sizeof(uint8_t), fileContent.size(), file);
			fclose(file);
			return len;
		}
		return 0;
	}

	size_t appendFileBinary(const std::string& filename, const std::vector<uint8_t>& fileContent) {
		FILE* file = fopen(filename.c_str(), "ab");
		if (file) {
			size_t len = fwrite(fileContent.data(), sizeof(uint8_t), fileContent.size(), file);
			fclose(file);
			return len;
		}
		return 0;
	}

	void deleteFile(const std::string& filename) {
		std::filesystem::remove(filename);
	}
}