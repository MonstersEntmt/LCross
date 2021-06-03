#pragma once

#include "Common/ByteBuffer.h"
#include "Core.h"

#include <string>
#include <vector>

enum class SymbolType : uint32_t {
	LABEL = 0,
	FUNCTION
};

struct Symbol {
public:
	Symbol();
	Symbol(const std::string& name, SymbolType type);

public:
	std::string name;
	SymbolType type          = SymbolType::LABEL;
	size_t sectionIndex      = 0;
	size_t sectionByteOffset = 0;
	size_t sectionByteLength = 0; // Not used for Label types
};

struct SymbolAddr {
public:
	uint16_t bits            = 64;
	size_t symbolIndex       = 0;
	size_t sectionIndex      = 0;
	size_t sectionByteOffset = 0;
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
	Arch outputArch;
	std::vector<Symbol> symbols;
	std::vector<SymbolAddr> symbolAddrs;
	std::vector<Section> sections;
};