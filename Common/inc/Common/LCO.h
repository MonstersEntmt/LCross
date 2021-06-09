#pragma once

#include "Common/ByteBuffer.h"
#include "Common/Format/Format.h"
#include "Core.h"

#include <string>
#include <vector>

enum class SymbolType : uint32_t {
	LABEL = 0,
	FUNCTION
};

enum class SymbolState : uint8_t {
	HIDDEN = 0,
	GLOBAL,
	EXTERNAL
};

template <>
struct Format::Formatter<SymbolType> {
	std::string format(SymbolType type, const std::string& options) {
		switch (type) {
		case SymbolType::LABEL: return "Label";
		case SymbolType::FUNCTION: return "Function";
		default: return "Unknown";
		}
	}
};

template <>
struct Format::Formatter<SymbolState> {
	std::string format(SymbolState state, const std::string& options) {
		switch (state) {
		case SymbolState::HIDDEN: return "Hidden";
		case SymbolState::GLOBAL: return "Global";
		case SymbolState::EXTERNAL: return "External";
		default: return "Unknown";
		}
	}
};

struct Symbol {
public:
	Symbol();
	Symbol(const std::string& name, SymbolType type);

public:
	std::string name;
	SymbolType type            = SymbolType::LABEL;
	SymbolState state          = SymbolState::HIDDEN;
	uint64_t sectionIndex      = 0;
	uint64_t sectionByteOffset = 0;
	uint64_t sectionByteLength = 0; // Not used for Label types
	uint64_t originAddress     = 0;
};

struct SymbolAddr {
public:
	uint16_t bits              = 64;
	uint64_t symbolIndex       = 0;
	uint64_t sectionIndex      = 0;
	uint64_t sectionByteOffset = 0;
};

struct Section {
public:
	Section();
	Section(const std::string& name);

public:
	std::string name;
	ByteBuffer bytes;
};

struct LCO {
public:
	Symbol* getSymbol(const std::string& name) const;
	Section* getSection(const std::string& name) const;

public:
	OutputArch outputArch = OutputArch::DEFAULT;
	std::vector<Symbol> symbols;
	std::vector<SymbolAddr> symbolAddrs;
	std::vector<Section> sections;
};