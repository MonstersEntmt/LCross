#include "Common/ByteBuffer.h"

uint8_t ByteBuffer::getUInt8(size_t offset) const {
	if (offset >= bytes.size()) return 0;
	return bytes[offset];
}

uint16_t ByteBuffer::getUInt16(size_t offset) const {
	return getUInt8(offset) | getUInt8(offset + 1) << 8;
}

uint32_t ByteBuffer::getUInt32(size_t offset) const {
	return getUInt8(offset) | getUInt8(offset + 1) << 8 | getUInt8(offset + 2) << 16 | getUInt8(offset + 3) << 24;
}

uint64_t ByteBuffer::getUInt64(size_t offset) const {
	return getUInt8(offset) | getUInt8(offset + 1) << 8 | getUInt8(offset + 2) << 16 | getUInt8(offset + 3) << 24 | (uint64_t) getUInt8(offset + 4) << 32 | (uint64_t) getUInt8(offset + 5) << 40 | (uint64_t) getUInt8(offset + 6) << 48 | (uint64_t) getUInt8(offset + 7) << 56;
}

std::string ByteBuffer::getString(size_t offset) const {
	return std::string(reinterpret_cast<const char*>(this->data() + offset));
}

std::string ByteBuffer::getStringNNT(size_t offset, size_t length) const {
	return std::string(reinterpret_cast<const char*>(this->data() + offset), length);
}

void ByteBuffer::getBytes(size_t offset, size_t length, ByteBuffer& bytes) const {
	auto begin = this->bytes.begin() + offset;
	bytes.addBytes(std::vector<uint8_t>(begin, begin + length));
}

uint8_t ByteBuffer::getUInt8() {
	if (this->index >= bytes.size()) return 0;
	return bytes[this->index++];
}

uint16_t ByteBuffer::getUInt16() {
	return getUInt8() | getUInt8() << 8;
}

uint32_t ByteBuffer::getUInt32() {
	return getUInt8() | getUInt8() << 8 | getUInt8() << 16 | getUInt8() << 24;
}

uint64_t ByteBuffer::getUInt64() {
	return getUInt8() | getUInt8() << 8 | getUInt8() << 16 | getUInt8() << 24 | (uint64_t) getUInt8() << 32 | (uint64_t) getUInt8() << 40 | (uint64_t) getUInt8() << 48 | (uint64_t) getUInt8() << 56;
}

std::string ByteBuffer::getString() {
	std::string str(reinterpret_cast<char*>(this->data()) + this->index);
	this->index += str.length() + 1;
	return str;
}

std::string ByteBuffer::getStringNNT(size_t length) {
	std::string str(reinterpret_cast<char*>(this->data()) + this->index, length);
	this->index += str.length();
	return str;
}

void ByteBuffer::getBytes(size_t length, ByteBuffer& bytes) {
	auto begin = this->bytes.begin() + index;
	bytes.addBytes(std::vector<uint8_t>(begin, begin + length));
	this->index += length;
}

void ByteBuffer::addUInt8(uint8_t value) {
	bytes.push_back(value);
}

void ByteBuffer::addUInt16(uint16_t value) {
	bytes.reserve(bytes.capacity() + 2);
	bytes.push_back(value & 0xFF);
	bytes.push_back((value >> 8) & 0xFF);
}

void ByteBuffer::addUInt32(uint32_t value) {
	bytes.reserve(bytes.capacity() + 4);
	bytes.push_back(value & 0xFF);
	bytes.push_back((value >> 8) & 0xFF);
	bytes.push_back((value >> 16) & 0xFF);
	bytes.push_back((value >> 24) & 0xFF);
}

void ByteBuffer::addUInt64(uint64_t value) {
	bytes.reserve(bytes.capacity() + 8);
	bytes.push_back(value & 0xFF);
	bytes.push_back((value >> 8) & 0xFF);
	bytes.push_back((value >> 16) & 0xFF);
	bytes.push_back((value >> 24) & 0xFF);
	bytes.push_back((value >> 32) & 0xFF);
	bytes.push_back((value >> 40) & 0xFF);
	bytes.push_back((value >> 48) & 0xFF);
	bytes.push_back((value >> 56) & 0xFF);
}

void ByteBuffer::addString(const std::string& value) {
	bytes.reserve(bytes.capacity() + value.length() + 1);
	for (size_t i = 0; i < value.length(); i++)
		bytes.push_back(value[i]);
	bytes.push_back(0x00);
}

void ByteBuffer::addStringNNT(const std::string& value) {
	bytes.reserve(bytes.capacity() + value.length());
	for (size_t i = 0; i < value.length(); i++)
		bytes.push_back(value[i]);
}

void ByteBuffer::addBytes(const std::vector<uint8_t>& bytes) {
	this->bytes.reserve(this->bytes.capacity() + bytes.size());
	for (size_t i = 0; i < bytes.size(); i++)
		this->bytes.push_back(bytes[i]);
}

void ByteBuffer::addBytes(const ByteBuffer& byteBuffer) {
	this->bytes.reserve(this->bytes.capacity() + byteBuffer.size());
	for (size_t i = 0; i < byteBuffer.size(); i++)
		this->bytes.push_back(byteBuffer[i]);
}

void ByteBuffer::addBytes(const std::vector<ByteBuffer>& byteBuffers) {
	for (const ByteBuffer& byteBuffer : byteBuffers)
		addBytes(byteBuffer);
}