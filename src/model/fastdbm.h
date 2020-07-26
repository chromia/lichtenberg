#pragma once
#include "model.h"
#include <vector>
#include <memory>
#include <set>
#include <optional>

/*
	An implementation of Fast DBM

	Fast Simulation of Laplacian Growth
	Theodore Kim, Jason Sewall, Avneesh Sud and Ming C. Lin
	University of North Carolina at Chapel Hill, USA
	http://gamma.cs.unc.edu/FRAC/
*/

namespace lichtenberg {
	namespace model {

		static const double FDBM_INITIAL_POTENTIAL = 0.0;

		class FastDBM : public BreakModel {
		public:
			FastDBM(int width, int height, double min_guarantee = 0.0, double eta = 1.0, const std::vector<float>& bias = {});
			FastDBM(const FastDBM&) = delete;
			FastDBM& operator =(const FastDBM&) = delete;

			bool test(int x, int y);
			void update(const CellList2D& cells, const std::vector<Point>& broken_list);

		private:
			int width;
			int height;
			bool init;
			double min_guarantee;
			double eta;
			std::set<Point> broken_set;
			double min_potential;
			double max_potential;
			double denominator;
			void initialize_potentials(const Point candidate_site);
			void update_potentials(const Point& broken_cell, const std::set<Point>& candidate_set);
			void update_minmax(const std::set<Point>& candidate_set);
			void update_denominator(const std::set<Point>& candidate_set);

			struct FDBMCell {
				double potential;
				FDBMCell(double potential = FDBM_INITIAL_POTENTIAL);
			};
			std::vector<std::vector<FDBMCell>> grid;

			typedef std::vector<std::vector<float>> BiasGrid;
			typedef std::shared_ptr<BiasGrid> BiasGridPtr;
			BiasGridPtr bias;

			double calc_potential(const Point& candidate_site, const Point& point_charge);
			double calc_bias_factor(const Point& position);
		};
	}
}

