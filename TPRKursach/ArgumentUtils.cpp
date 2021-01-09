#include "ArgumentsUtils.h"

argumetsUtils::ArgumentUtil::ArgumentUtil()
{
}

argumetsUtils::ArgumentUtil::~ArgumentUtil()
{
}

argumetsUtils::ArgumentUtil& argumetsUtils::ArgumentUtil::read()
{
	char** argv = nullptr;
	size_t argc = commandLineArgs(argv);

	std::string arg_str;
	for (size_t i = 0; i < argc; ++i)
	{
		arg_str = std::string(argv[i]);
		if (arg_str.at(0) == '-')
		{
			arg_str.erase(0, 1);
		}
		size_t ix = arg_str.find('=');
		if (ix != std::string::npos)
		{
			args_map_.insert(std::make_pair(arg_str.substr(0, ix - 1), arg_str.substr(++ix)));
		}
		else
		{
			args_map_.insert(std::make_pair(arg_str.substr(0, ix), std::nullopt));
		}
	}

	for (auto i = 0; i < argc; ++i)
	{
		free(argv[i]);
	}
	free(argv);

	return *this;
}

bool argumetsUtils::ArgumentUtil::has_argument(std::string&& key)
{
	return args_map_.find(key) != args_map_.end();
}

std::optional<std::string> argumetsUtils::ArgumentUtil::argument_value(std::string&& key)
{
	return
		!has_argument(std::move(key))
		?
		std::nullopt
		:
		args_map_.find(key)->second
		;
}

int argumetsUtils::ArgumentUtil::commandLineArgs(char**& argv)
{
	int argc = 0;
	LPWSTR clw = GetCommandLineW();
	LPWSTR* wargv = CommandLineToArgvW(clw, &argc);
	if (wargv) {
		argv = new char* [argc];
		for (int i = 0; i < argc; i++) {
			LPWSTR warg = wargv[i];
			DWORD size = WideCharToMultiByte(CP_UTF8, 0, warg, -1, NULL, 0, NULL, NULL);
			if (size == 0)
				return -1;
			argv[i] = new char[size];
			DWORD result = WideCharToMultiByte(CP_UTF8, 0, warg, -1, argv[i], size, NULL, NULL);
			if (result == 0)
				return -1;
		}
	}
	return argc;
}
