#include "Common/LCO.h"

Symbol::Symbol() { }

Symbol::Symbol(const std::string& name, SymbolType type)
    : name(name), type(type) { }

Section::Section() { }

Section::Section(const std::string& name)
    : name(name) { }

Symbol* LCO::getSymbol(const std::string& name) const {
	for (const Symbol& sym : this->symbols)
		if (sym.name == name)
			return const_cast<Symbol*>(&sym);
	return nullptr;
}

Section* LCO::getSection(const std::string& name) const {
	for (const Section& sect : this->sections)
		if (sect.name == name)
			return const_cast<Section*>(&sect);
	return nullptr;
}