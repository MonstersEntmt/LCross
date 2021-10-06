#pragma once

#include "ByteBuffer.hpp"

#include <cstddef>

#include <filesystem>
#include <string>
#include <string_view>

namespace LCross::Common {
	std::size_t readFile(const std::filesystem::path& filepath, std::string& fileContent);
	std::size_t readBinaryFile(const std::filesystem::path& filepath, ByteBuffer& bytes);

	void writeFile(const std::filesystem::path& filepath, std::string_view fileContent);
	void writeBinaryFile(const std::filesystem::path& filepath, const ByteBuffer& bytes);

	void appendFile(const std::filesystem::path& filepath, std::string_view fileContent);
	void appendBinaryFile(const std::filesystem::path& filepath, const ByteBuffer& bytes);

	void deleteFile(const std::filesystem::path& filepath);
} // namespace LCross::Common
