#pragma once
#include "dbmmodel.h"
#include <cassert>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include <set>


namespace lichtenberg {
	namespace model {
		DBMCell::DBMCell(double potential, bool lock)
			: potential(potential), lock(lock)
		{
		}

		DielectricBreakModel::DielectricBreakModel(int width, int height, const DBMGrid& initial_state, double min_guarantee, double eta)
			: init(false), width(width), height(height), min_guarantee(min_guarantee), eta(eta), denominator(0.0)
		{
			int grid_height = (int)initial_state.size();
			if (grid_height == 0) {
				throw std::runtime_error("the height of initial_state is invalid.");
			}
			int grid_width = (int)initial_state[0].size();
			if (grid_width != width + 2 || grid_height != height + 2) {
				throw std::runtime_error("the size of initial_state must be (width+2)*(height*2).");
			}
			grid.resize(grid_height);
			for (int y = 0; y < grid_height; y++) {
				grid[y].resize(grid_width);
				for (int x = 0; x < grid_width; x++) {
					grid[y][x] = initial_state[y][x];
				}
			}
		}

		bool DielectricBreakModel::test(int x, int y)
		{
			double ec = min_guarantee;
			double threshold = ec + ((double)rand() / RAND_MAX) * (1.0 - ec);
			double p = grid[y + 1][x + 1].potential;
			if (eta == 1.0) {
				double prob = p / denominator;
				return prob >= threshold;
			}
			else {
				double prob = std::pow(p, eta) / denominator;
				return prob >= threshold;
			}
		}

		void DielectricBreakModel::update(const CellList2D& cells, const std::vector<Point>& broken_list)
		{
			if (!init) {
				init_grid(cells);
				init = true;
			}
			update_grid(cells, broken_list);
			// solve laplace equation
			solve_laplace(cells);
			// calc denominator of probability
			denominator = calc_denominator(cells, broken_list);
		}

		void DielectricBreakModel::init_grid(const CellList2D& cells)
		{
			const int w = width;
			const int h = height;
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					auto c = grid[y + 1][x + 1];
					if (cells.get_broken(x, y)) {
						c.potential = DBM_POTENTIAL_VOID;
						c.lock = true;
					}
				}
			}
		}

		bool DielectricBreakModel::update_grid(const CellList2D& cells, const std::vector<Point>& broken_list)
		{
			bool changed = false;
			for (const Point& p : broken_list) {
				auto [x, y] = p;
				bool b = cells.get_broken(x, y);
				auto& c = grid[y + 1][x + 1];
				if (c.lock != b) {
					changed = true;
					c.lock = b;
					c.potential = DBM_POTENTIAL_VOID;
				}
			}
			return changed;
		}

		bool DielectricBreakModel::solve_laplace(const CellList2D& /*cells*/)
		{
			const int max_loop = 10000;
			const double eps = 1.0e-3;
			const int w = width;
			const int h = height;

			//Gauss-Seidel method
			for (int i = 0; i < max_loop; i++) {
				double err = 0.0;
				for (int y = 1; y <= h; y++) {
					auto& row = grid[y];
					for (int x = 1; x <= w; x++) {
						auto& c = row[x];
						if (!c.lock) {
							double old_potential = c.potential;
							const auto& ym = grid[y - 1];
							const auto& yc = grid[y];
							const auto& yp = grid[y + 1];
							double new_potential = (yc[x - 1].potential + yc[x + 1].potential + ym[x].potential + yp[x].potential) / 4;
							double diff = new_potential - old_potential;
							c.potential = old_potential + 1.9 * diff;  //SOR method
							err += fabs(diff);
						}
					}
				}
				if (err < eps) {
					return true;
				}
			}
			return false;
		}

		double DielectricBreakModel::calc_denominator(const CellList2D& cells, const std::vector<Point>& broken_list)
		{
			std::set<Point> target_set;
			for (const Point& p : broken_list) {
				auto [x, y] = p;
				if (x > 0 && !cells.get_broken(x - 1, y)) {
					target_set.insert(Point(x - 1, y));
				}
				if (x < width - 1 && !cells.get_broken(x + 1, y)) {
					target_set.insert(Point(x + 1, y));
				}
				if (y > 0 && !cells.get_broken(x, y - 1)) {
					target_set.insert(Point(x, y - 1));
				}
				if (y < height - 1 && !cells.get_broken(x, y + 1)) {
					target_set.insert(Point(x, y + 1));
				}
			}
			double denom = 0.0;
			if (eta == 1.0) {
				for (const auto& c : target_set) {
					auto [x, y] = c;
					double p = grid[y + 1][x + 1].potential;
					denom += p;
				}
			}
			else {
				for (const auto& c : target_set) {
					auto [x, y] = c;
					double p = grid[y + 1][x + 1].potential;
					denom += std::pow(p, eta);
				}
			}

			return denom;
		}

		double DielectricBreakModel::get_potential(int x, int y) const
		{
			return grid[y][x].potential;
		}
	}
}

