#include <iostream>
#define OPTIMIZER_CMD_DEBUG_OUTPUT
#include "Optimizer.hpp"
#include "ArgumentsUtils.h"
#include <fstream>

int main()
{
	ModelData md;
	Eigen::Matrix<double, 9, 1> opt_arg;
	Optimizer<9> opt(0.0001, 1000);

	nlohmann::json j;
	argumetsUtils::ArgumentUtil cl_reader;
	cl_reader.read();
	std::string path = cl_reader.argument_value("data")
		?
		*cl_reader.argument_value("data")
		:
		"data.json"
		;

	std::ifstream ifile(path);

	if(ifile.is_open())
	{
		ifile >> j;

		if(j.contains("data"))
		{
			j.at("data").get_to(md);
		}
		else
		{
			md.a =
			{
				{
					{ { 1, 2 }, { 0.5, 1 }, { 2, 3 } },
				{ { 1, 3 }, { 1, 0.5 }, { 3, 5 } },
				{ { 1, 4 }, { 1, 3 }, { 1, 3 } }
				},
			{
				{ { 2, 4 }, { 2, 4 }, { 2, 4 } },
				{ { 3, 7 }, { 1, 2 }, { 4, 6 } },
				{ { 2, 6 }, { 3, 5 }, { 6, 8 } }
			}
			};

			md.c =
			{
				{ 2, 8, 5 },
				{ 3, 6, 6 },
				{ 3, 9, 5 }
			};

			md.b = { { 250, 150, 100 }, { 200, 240, 300 } };

			md.d = { 300, 200, 150 };

			md.alpha = 0.9;

			j["data"] = nlohmann::json(md);
		}
	}
	else
	{
		md.a =
		{
			{
				{ { 1, 2 }, { 0.5, 1 }, { 2, 3 } },
			{ { 1, 3 }, { 1, 0.5 }, { 3, 5 } },
			{ { 1, 4 }, { 1, 3 }, { 1, 3 } }
			},
			{
				{ { 2, 4 }, { 2, 4 }, { 2, 4 } },
			{ { 3, 7 }, { 1, 2 }, { 4, 6 } },
			{ { 2, 6 }, { 3, 5 }, { 6, 8 } }
			}
		};

		md.c =
		{
			{ 2, 8, 5 },
			{ 3, 6, 6 },
			{ 3, 9, 5 }
		};

		md.b = { { 250, 150, 100 }, { 200, 240, 300 } };

		md.d = { 300, 200, 150 };

		md.alpha = 0.9;

		j["data"] = nlohmann::json(md);

		std::ofstream ofile(path, std::ofstream::out | std::ofstream::trunc);
		ofile << j;
		ofile.close();
	}


	opt.optimize(md, opt_arg);
	std::cout << "alpha = " << md.alpha << std::endl;
	std::cout << "x11: " << opt_arg[0] << std::endl;
	std::cout << "x12: " << opt_arg[1] << std::endl;
	std::cout << "x13: " << opt_arg[2] << std::endl;
	std::cout << "x21: " << opt_arg[3] << std::endl;
	std::cout << "x22: " << opt_arg[4] << std::endl;
	std::cout << "x23: " << opt_arg[5] << std::endl;
	std::cout << "x31: " << opt_arg[6] << std::endl;
	std::cout << "x32: " << opt_arg[7] << std::endl;
	std::cout << "x33: " << opt_arg[8] << std::endl;


	md.alpha = 0.7;
	opt.optimize(md, opt_arg);
	std::cout << "alpha = " << md.alpha << std::endl;
	std::cout << "x11: " << opt_arg[0] << std::endl;
	std::cout << "x12: " << opt_arg[1] << std::endl;
	std::cout << "x13: " << opt_arg[2] << std::endl;
	std::cout << "x21: " << opt_arg[3] << std::endl;
	std::cout << "x22: " << opt_arg[4] << std::endl;
	std::cout << "x23: " << opt_arg[5] << std::endl;
	std::cout << "x31: " << opt_arg[6] << std::endl;
	std::cout << "x32: " << opt_arg[7] << std::endl;
	std::cout << "x33: " << opt_arg[8] << std::endl;


	md.alpha = 0.01;
	opt.optimize(md, opt_arg);
	std::cout << "alpha = " << md.alpha << std::endl;
	std::cout << "x11: " << opt_arg[0] << std::endl;
	std::cout << "x12: " << opt_arg[1] << std::endl;
	std::cout << "x13: " << opt_arg[2] << std::endl;
	std::cout << "x21: " << opt_arg[3] << std::endl;
	std::cout << "x22: " << opt_arg[4] << std::endl;
	std::cout << "x23: " << opt_arg[5] << std::endl;
	std::cout << "x31: " << opt_arg[6] << std::endl;
	std::cout << "x32: " << opt_arg[7] << std::endl;
	std::cout << "x33: " << opt_arg[8] << std::endl;
	return 0;
}