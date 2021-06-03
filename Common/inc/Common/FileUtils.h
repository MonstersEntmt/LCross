#pragma once

#include "ByteBuffer.h"
#include "LCO.h"

#include <string>
#include <vector>

namespace FileUtils {
	size_t readFile(const std::string& filename, std::string& fileContent);
	size_t readFileBinary(const std::string& filename, ByteBuffer& bytes);
	void writeFile(const std::string& filename, const std::string& fileContent);
	void appendFile(const std::string& filename, const std::string& fileContent);
	void writeFileBinary(const std::string& filename, const ByteBuffer& bytes);
	void appendFileBinary(const std::string& filename, const ByteBuffer& bytes);
	void deleteFile(const std::string& filename);

	void writeLCO(const std::string& filename, const LCO& lco);
	bool readLCO(const std::string& filename, LCO& lco);
} // namespace FileUtils