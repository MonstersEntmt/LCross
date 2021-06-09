#pragma once

#include <string>
#include <vector>

class ByteBuffer {
public:
	size_t getIndex() const { return this->index; }

	uint8_t getUInt8(size_t offset) const;
	uint16_t getUInt16(size_t offset) const;
	uint32_t getUInt32(size_t offset) const;
	uint64_t getUInt64(size_t offset) const;
	std::string getString(size_t offset) const;
	std::string getStringNNT(size_t offset, size_t length) const;
	void getBytes(size_t offset, size_t length, ByteBuffer& bytes) const;
	int8_t getInt8(size_t offset) const {
		uint8_t res = getUInt8(offset);
		return *reinterpret_cast<int8_t*>(&res);
	}
	int16_t getInt16(size_t offset) const {
		uint16_t res = getUInt16(offset);
		return *reinterpret_cast<int16_t*>(&res);
	}
	int32_t getInt32(size_t offset) const {
		uint32_t res = getUInt32(offset);
		return *reinterpret_cast<int32_t*>(&res);
	}
	int64_t getInt64(size_t offset) const {
		uint64_t res = getUInt64(offset);
		return *reinterpret_cast<int64_t*>(&res);
	}

	uint8_t getUInt8();
	uint16_t getUInt16();
	uint32_t getUInt32();
	uint64_t getUInt64();
	std::string getString();
	std::string getStringNNT(size_t length);
	void getBytes(size_t length, ByteBuffer& bytes);
	int8_t getInt8() {
		uint8_t res = getUInt8();
		return *reinterpret_cast<int8_t*>(&res);
	}
	int16_t getInt16() {
		uint16_t res = getUInt16();
		return *reinterpret_cast<int16_t*>(&res);
	}
	int32_t getInt32() {
		uint32_t res = getUInt32();
		return *reinterpret_cast<int32_t*>(&res);
	}
	int64_t getInt64() {
		uint64_t res = getUInt64();
		return *reinterpret_cast<int64_t*>(&res);
	}

	void addUInt8(uint8_t value, size_t position);
	void addUInt16(uint16_t value, size_t position);
	void addUInt32(uint32_t value, size_t position);
	void addUInt64(uint64_t value, size_t position);
	void addUInt8(uint8_t value) { addUInt8(value, this->bytes.size()); }
	void addUInt16(uint16_t value) { addUInt16(value, this->bytes.size()); }
	void addUInt32(uint32_t value) { addUInt32(value, this->bytes.size()); }
	void addUInt64(uint64_t value) { addUInt64(value, this->bytes.size()); }
	void addString(const std::string& value);
	void addStringNNT(const std::string& value);
	void addBytes(const std::vector<uint8_t>& bytes);
	void addBytes(const ByteBuffer& byteBuffer);
	void addBytes(const std::vector<ByteBuffer>& byteBuffers);
	void addInt8(int8_t value) { addUInt8(*reinterpret_cast<uint8_t*>(&value)); }
	void addInt16(int16_t value) { addUInt16(*reinterpret_cast<uint16_t*>(&value)); }
	void addInt32(int32_t value) { addUInt32(*reinterpret_cast<uint32_t*>(&value)); }
	void addInt64(int64_t value) { addUInt64(*reinterpret_cast<uint64_t*>(&value)); }
	void addInt8(int8_t value, size_t position) { addUInt8(*reinterpret_cast<uint8_t*>(&value), position); }
	void addInt16(int16_t value, size_t position) { addUInt16(*reinterpret_cast<uint16_t*>(&value), position); }
	void addInt32(int32_t value, size_t position) { addUInt32(*reinterpret_cast<uint32_t*>(&value), position); }
	void addInt64(int64_t value, size_t position) { addUInt64(*reinterpret_cast<uint64_t*>(&value), position); }

	void replaceUInt8(uint8_t value, size_t position);
	void replaceUInt16(uint16_t value, size_t position);
	void replaceUInt32(uint32_t value, size_t position);
	void replaceUInt64(uint64_t value, size_t position);
	void replaceInt8(int8_t value, size_t position) { replaceUInt8(*reinterpret_cast<uint8_t*>(&value), position); }
	void replaceInt16(int16_t value, size_t position) { replaceUInt16(*reinterpret_cast<uint16_t*>(&value), position); }
	void replaceInt32(int32_t value, size_t position) { replaceUInt32(*reinterpret_cast<uint32_t*>(&value), position); }
	void replaceInt64(int64_t value, size_t position) { replaceUInt64(*reinterpret_cast<uint64_t*>(&value), position); }

	void resize(size_t newSize) { this->bytes.resize(newSize); }
	size_t size() const { return this->bytes.size(); }
	uint8_t* data() { return this->bytes.data(); }
	const uint8_t* data() const { return this->bytes.data(); }
	void clear() { this->bytes.clear(); }
	uint8_t operator[](size_t index) const { return this->bytes[index]; }

private:
	std::vector<uint8_t> bytes;
	size_t index = 0;
};