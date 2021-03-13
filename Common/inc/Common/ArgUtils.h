#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

enum class FlagValueType {
	REQUIRED,
	OPTIONAL,
	MULTI
};

struct FlagValueInfo {
	FlagValueInfo(const std::string& name, FlagValueType type, const std::vector<std::string>& values);

	std::string name;
	FlagValueType type;
	std::vector<std::string> values;
};

struct FlagInfo {
	FlagInfo(const std::string& name, const std::string& desc, const std::string& note, const std::vector<FlagValueInfo>& values);

	std::string getSyntax() const;
	friend std::ostream& operator<<(std::ostream& ostream, const FlagInfo& flagInfo);

	std::string name;
	std::string desc;
	std::string note;
	std::vector<FlagValueInfo> values;
};

class ArgUtils {
public:
	using HandlerFunc = std::function<void(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed)>;

	ArgUtils(int argc, char** argv);

	void registerFlagInfo(const FlagInfo& argInfo);
	void registerHandlerFunc(const std::string& argName, HandlerFunc handlerFunc);

	const FlagInfo* getFlagInfo(const std::string& argName) const;
	inline const std::vector<FlagInfo>& getFlagInfos() const { return argInfos; }
	inline const char* getProcessCommand() const { return argv[0]; }
	inline int getProcessArgCount() const { return argc - 1; }
	inline char** getProcessArgs() const { return argv + 1; }

	virtual void handleHelpFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed);
	virtual void handleVersionFlag(ArgUtils& argUtils, const std::string& arg, const std::vector<std::string>& argValues, size_t& usedValueCount, bool& argFailed);
	virtual std::string getFullUsageString(const FlagInfo* currentFlag);
	virtual void handleOutputName();
	void handle();

protected:
	virtual void handleVirt() {}

public: // Passed Arg Data
	std::vector<std::string> inputNames;
	std::string outputName;
	bool verbose = false;

private:
	int argc;
	char** argv;
	std::vector<FlagInfo> argInfos;
	std::unordered_map<std::string, HandlerFunc> handlerFuncs;
};
