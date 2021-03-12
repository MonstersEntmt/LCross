#pragma once

#include "Core.h"

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <functional>

class ArgUtils;
struct ArgInfo;

extern void handleOutputName(ArgUtils& argUtils, std::string& outputName);
std::string getUsageString(ArgUtils& argUtils);
std::string getFullUsageString(ArgUtils& argUtils, const ArgInfo* argInfo);

struct ArgInfo {
	std::string argName;
	std::string argDesc;
	std::string argNote;
	std::vector<std::pair<std::string, std::vector<std::string>>> argValues;
};

std::ostream& operator<<(std::ostream& ostream, const ArgInfo& argInfo);

enum class ArgHandlerError {
	NO_VALUE,
	TOO_FEW_VALUES,
	INVALID_VALUE
};

class ArgUtils {
public:
	using ArgHandler = std::function<void(ArgUtils& argUtils, size_t& usedValueCount, bool& argFailed, std::string arg, std::vector<std::string> values)>;

	ArgUtils(int argc, char** argv);
	void setRequiresDefaultArgument(bool require);
	void addArgInfo(const std::string& argName, const std::string& argDesc, const std::string& argNote = "");
	void addArgValue(const std::string& argName, const std::string& argValue, const std::string& value);
	const ArgInfo* getArgInfo(const std::string& argName) const;
	const std::vector<std::pair<std::string, ArgInfo>>& getArgInfos() const;
	void addHandler(const std::string& argName, ArgHandler argHandler);
	std::string getCommand() const;
	const std::vector<std::string>& getArguments() const;
	bool hasFailed() const;
	void setVerbose();
	bool isVerbose() const;
	void setLinks(bool links);
	bool getLinks() const;
	void setHasNoLinkArg(bool hasNoLinkArg);
	bool getHasNoLinkArg() const;
	void setNoLink();
	bool getNoLink() const;
	void addInputName(const std::string& inputName);
	const std::vector<std::string>& getInputNames() const;
	void setOutputName(const std::string& outputName);
	const std::string& getOutputName() const;
	void setOutputArch(Arch outputArch);
	Arch getOutputArch() const;

	void handle();
	void handleArgs();

private:
	std::vector<std::string> arguments;
	std::vector<std::pair<std::string, ArgInfo>> argInfos;
	std::unordered_map<std::string, ArgHandler> argHandlers;
	bool requiresDefaultArguments = true;
	bool argFailed = false;
	bool verbose = false;
	bool links = true;
	bool hasNoLinkArg = true;
	bool noLink = false;

	std::vector<std::string> inputNames;
	std::string outputName;
	Arch outputArch = Arch::DEFAULT;
};