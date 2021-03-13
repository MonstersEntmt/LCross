#pragma once

#include <string>
#include <vector>

namespace FileUtils {
	size_t readFile(const std::string& filename, std::string& fileContent);
	size_t readFileBinary(const std::string& filename, std::vector<uint8_t>& fileContent);
	size_t writeFile(const std::string& filename, const std::string& fileContent);
	size_t appendFile(const std::string& filename, const std::string& fileContent);
	size_t writeFileBinary(const std::string& filename, const std::vector<uint8_t>& fileContent);
	size_t appendFileBinary(const std::string& filename, const std::vector<uint8_t>& fileContent);
	void deleteFile(const std::string& filename);
}