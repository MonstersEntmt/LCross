#pragma once

#include "../Core.hpp"
#include "ByteBuffer.hpp"

#include <cstdint>

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace LCross::Common::LCO {
	enum class SymbolType : std::uint32_t {
		Label = 0,
		Function
	};

	enum class SymbolState : std::uint8_t {
		Hidden = 0,
		Global,
		External
	};

	struct Symbol {
	public:
		Symbol();
		Symbol(std::string_view name, SymbolType type);

	public:
		std::string name;
		SymbolType type                 = SymbolType::Label;
		SymbolState state               = SymbolState::Hidden;
		std::uint64_t sectionIndex      = 0;
		std::uint64_t sectionByteOffset = 0;
		std::uint64_t sectionByteLength = 0; // Not used for Label types
		std::uint64_t originAddress     = 0;
	};

	struct SymbolAddr {
	public:
		std::uint16_t bits              = 64;
		std::uint64_t symbolIndex       = 0;
		std::uint64_t sectionIndex      = 0;
		std::uint64_t sectionByteOffset = 0;
	};

	struct Section {
	public:
		Section();
		Section(std::string_view name);

	public:
		std::string name;
		ByteBuffer bytes;
	};

	struct LCO {
	public:
		Symbol* getSymbol(std::string_view name) const;
		Section* getSection(std::string_view name) const;

	public:
		Architecture outputArch = Architecture::Default;
		std::vector<Symbol> symbols;
		std::vector<SymbolAddr> symbolAddrs;
		std::vector<Section> sections;
	};

	void writeLCO(const std::filesystem::path& filepath, const LCO& lco);
	bool readLCO(const std::filesystem::path& filepath, LCO& lco);
} // namespace LCross::Common::LCO
