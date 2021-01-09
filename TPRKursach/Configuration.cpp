#include "Configuration.h"

void from_json(const nlohmann::json& j, ModelData& md)
{
	j.at("c").get_to(md.c);
	j.at("a").get_to(md.a);
	j.at("b").get_to(md.b);
	j.at("d").get_to(md.d);
	j.at("alpha").get_to(md.alpha);
}

void to_json(nlohmann::json& j, const ModelData& md)
{
	j = nlohmann::json{
		{ "c", md.c },
		{ "a", md.a },
		{ "b", md.b },
		{ "d", md.d },
		{ "alpha", md.alpha },
	};
}
