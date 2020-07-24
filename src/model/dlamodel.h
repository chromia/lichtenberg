#pragma once
#include "model.h"
#include <tuple>

/*
	An implementation of DLA(Diffusion-limited aggregation)

	Diffusion-Limited Aggregation, a Kinetic Critical Phenomenon
	T. A. Witten, Jr. and L. M. Sander
	Phys. Rev. Lett. 47, 1400 – Published 9 November 1981
	https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.47.1400
*/

namespace lichtenberg {
	namespace model {
		class DLABreakModel : public BreakModel {
		public:
			DLABreakModel(int width, int height, int num_particle);
			DLABreakModel(const DLABreakModel&) = delete;
			DLABreakModel& operator =(const DLABreakModel&) = delete;

			bool test(int x, int y);
			void update(const CellList2D& cells, const std::vector<Point>& broken_list);

		private:
			int width;
			int height;

			typedef std::tuple<double, double> PointF;

			struct DLAParticle {
				DLAParticle* prev;
				DLAParticle* next;
				PointF position;
				bool live;
				DLAParticle(double x, double y);
			};
			typedef std::vector<std::vector<DLAParticle*>> DLAGrid;

			DLAGrid grid;
			std::vector<DLAParticle> particles;
		};
	}
}