#pragma once
#include <vector>


namespace lichtenberg
{
	namespace util
	{
		std::vector<std::vector<float>> make_electric_field(
			int width, int height, const std::vector<std::tuple<int, int, float>>& points,
			int max_loop = 1000, float eps = 1.0e-6, float sor_coef = 1.5);
	}
}
