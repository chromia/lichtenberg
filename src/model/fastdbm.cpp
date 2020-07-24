#pragma once
#include "fastdbm.h"
#include <vector>
#include <memory>
#include <cmath>


namespace lichtenberg {
	namespace model {

		FastDBM::FDBMCell::FDBMCell(double potential)
			: potential(potential)
		{
		}

		FastDBM::FastDBM(int width, int height, double min_guarantee, double eta)
			: init(false), width(width), height(height), min_guarantee(min_guarantee), eta(eta),
			min_potential(0.0), max_potential(0.0), denominator(0.0)
		{
			grid.resize(height);
			for (int y = 0; y < height; y++) {
				grid[y].resize(width);
			}
		}
		bool FastDBM::test(int x, int y)
		{
			double ec = min_guarantee;
			double threshold = ec + ((double)rand() / RAND_MAX) * (1.0 - ec);
			double p = grid[y][x].potential;
			double numerator = p;
			if (max_potential != min_potential) {
				numerator = (p - min_potential) / (max_potential - min_potential);
			}
			double prob = std::pow(numerator, eta) / denominator;
			return prob >= threshold;
		}
		void FastDBM::update(const CellList2D& cells, const std::vector<Point>& broken_list)
		{
			//update broken cells
			std::vector<Point> new_broken_cells;
			for (const Point& p : broken_list) {
				if (broken_set.count(p) == 0) {
					//add new broken cell to the set
					broken_set.insert(p);
					new_broken_cells.push_back(p);
				}
			}

			//get candidate cells
			std::set<Point> old_candidates;
			std::set<Point> new_candidates;
			auto dispatch = [&](int ix, int iy) {
				if (!cells.get_broken(ix, iy)) {
					if (grid[iy][ix].potential == FDBM_INITIAL_POTENTIAL) {
						new_candidates.insert(Point(ix, iy));
					}
					else {
						old_candidates.insert(Point(ix, iy));
					}
				}
			};
			for (const Point& p : broken_list) {
				auto [x, y] = p;
				if (x > 0) dispatch(x - 1, y);
				if (x < width - 1) dispatch(x + 1, y);
				if (y > 0) dispatch(x, y - 1);
				if (y < height - 1) dispatch(x, y + 1);
			}

			//update potential at all old candidate sites
			for (const Point& p : new_broken_cells) {
				update_potentials(p, old_candidates);
			}
			//initialize potential at all new candidate sites
			for (const Point& p : new_candidates) {
				initialize_potentials(p);
			}
			//merge cancidiate sites
			auto& candidates = old_candidates;
			candidates.insert(new_candidates.begin(), new_candidates.end());

			//calculate some parts of probability expression
			update_minmax(candidates);
			update_denominator(candidates);
		}

		static double distance(const Point& p1, const Point& p2)
		{
			auto [x1, y1] = p1;
			auto [x2, y2] = p2;
			double dx = (double)(x2 - x1);
			double dy = (double)(y2 - y1);
			return std::sqrt(dx * dx + dy * dy);
		}

		static double calc_potential(const Point& candidate_site, const Point& point_charge)
		{
			//expression (9)
			const double R1 = 0.5;
			double d = distance(candidate_site, point_charge);
			return (1.0 - R1 / d);
		}

		void FastDBM::initialize_potentials(const Point candidate_site)
		{
			//expression (10)
			auto [x, y] = candidate_site;
			for (const Point& p : broken_set) {
				grid[y][x].potential += calc_potential(candidate_site, p);
			}
		}

		void FastDBM::update_potentials(const Point& broken_cell, const std::set<Point>& candidate_set)
		{
			//expression (11)
			const double R1 = 0.5;
			for (const Point& p : candidate_set) {
				auto [x, y] = p;
				grid[y][x].potential += calc_potential(p, broken_cell);
			}
		}
		void FastDBM::update_minmax(const std::set<Point>& candidate_set)
		{
			//get maximum/minimum potential
			double max_p = 0.0;
			double min_p = 1.0e10;
			for (const Point& c : candidate_set) {
				auto [x, y] = c;
				double p = grid[y][x].potential;
				if (p > max_p) max_p = p;
				if (p < min_p) min_p = p;
			}
			min_potential = min_p;
			max_potential = max_p;
		}
		void FastDBM::update_denominator(const std::set<Point>& candidate_set)
		{
			//expression (12),(13)
			double denom = 0.0;
			double min_p = min_potential;
			double D = max_potential - min_potential;
			if (D == 0.0) {
				//first time(min==max)
				D = 1.0;
				min_p = 0.0;
			}
			for (const auto& c : candidate_set) {
				auto [x, y] = c;
				double p = grid[y][x].potential;
				double t = (p - min_p) / D;
				denom += std::pow(t, eta);
			}
			denominator = denom;
		}

	}
}

