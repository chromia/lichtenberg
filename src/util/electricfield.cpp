#include "electricfield.h"
#include <set>
#include <tuple>


std::vector<std::vector<float>> lichtenberg::util::make_electric_field(
	int width, int height, const std::vector<std::tuple<int, int, float>>& points,
	int max_loop, float eps, float sor_coef)
{
	std::vector<std::vector<float>> cells;
	std::set<std::tuple<int, int>> locked;
	for (int y = 0; y < height; y++) {
		std::vector<float> row(width, 0.0f);
		cells.push_back(row);
	}
	for (auto& p : points) {
		auto [x, y, potential] = p;
		std::tuple<int, int> xy = { x, y };
		locked.insert(xy);
		cells[y][x] = potential;
	}
	for (int i = 0; i < max_loop; i++) {
		float err = 0.0f;
		for (int y = 1; y < height - 1; y++) {
			auto& yc = cells[y];
			const auto& yp = cells[y + 1];
			const auto& ym = cells[y - 1];
			for (int x = 1; x < width - 1; x++) {
				std::tuple<int, int> xy = { x, y };
				if (locked.count(xy) == 1) continue;
				float old_pot = yc[x];
				float new_pot = (yc[x - 1] + yc[x + 1] + ym[x] + yp[x]) / 4.0f;
				float diff = new_pot - old_pot;
				yc[x] = old_pot + diff * sor_coef;
				err += diff;
			}
		}
		if (err < eps) break;
	}
	return cells;
}
