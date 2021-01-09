#pragma once
#include <nlohmann/json.hpp>

struct ModelData
{
	std::vector<std::vector<double>> c;
	std::vector<std::vector<std::vector<std::pair<double, double>>>> a;
	std::vector<std::vector<double>> b;
	std::vector<double> d;
	double alpha;
};

void from_json(const nlohmann::json& j, ModelData& md);
void to_json(nlohmann::json& j, const ModelData& md);