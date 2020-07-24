#pragma once
#include "model.h"
#include <vector>
#include <memory>

/*
	An implementation of DBM(Dielectric Break Model)

	Fractal Dimension of Dielectric Breakdown
	L. Niemeyer, L. Pietronero, and H. J. Wiesmann
	Phys. Rev. Lett. 52, 1033 – Published 19 March 1984
	https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.52.1033
*/

namespace lichtenberg {
	namespace model {

		static const double DBM_POTENTIAL_VOID = 0.0;
		static const double DBM_POTENTIAL_ATTRACT = 1.0;

		struct DBMCell {
			double potential;
			bool lock;
			DBMCell(double potential = DBM_POTENTIAL_VOID, bool lock = false);
		};
		typedef std::vector<std::vector<DBMCell>> DBMGrid;

		class DielectricBreakModel : public BreakModel {
		public:
			/* the size of initial_state must be (width+2)*(height+2) */
			DielectricBreakModel(int width, int height, const DBMGrid& initial_state, double min_guarantee = 0.0, double eta = 1.0);
			DielectricBreakModel(const DielectricBreakModel&) = delete;
			DielectricBreakModel& operator =(const DielectricBreakModel&) = delete;

			bool test(int x, int y);
			void update(const CellList2D& cells, const std::vector<Point>& broken_list);
			double get_potential(int x, int y) const;

		private:
			int width;
			int height;
			bool init;
			double min_guarantee;
			double eta;
			DBMGrid grid;
			double denominator;
			void init_grid(const CellList2D& cells);
			bool update_grid(const CellList2D& cells, const std::vector<Point>& broken_list);
			bool solve_laplace(const CellList2D& cells);
			double calc_denominator(const CellList2D& cells, const std::vector<Point>& broken_list);
		};
	}
}