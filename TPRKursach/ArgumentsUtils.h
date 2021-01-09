#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <processenv.h>
#include <unordered_map>
#include <optional>

namespace argumetsUtils
{
	class ArgumentUtil
	{
	public:
		ArgumentUtil();
		~ArgumentUtil();

		ArgumentUtil& read();

		bool has_argument(std::string&&);
		std::optional<std::string> argument_value(std::string&&);

	private:
		std::unordered_map<std::string, std::optional<std::string>> args_map_;

		int commandLineArgs(char**& argv);
	};
}