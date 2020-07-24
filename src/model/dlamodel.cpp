#include "dlamodel.h"
#include <utility>
#include <cmath>


namespace lichtenberg {
	namespace model {
		DLABreakModel::DLAParticle::DLAParticle(double x, double y)
			: next(nullptr), prev(nullptr), position(x, y), live(true)
		{
		}

		static double random()
		{
			return (double)rand() / RAND_MAX;
		}

		DLABreakModel::DLABreakModel(int width, int height, int num_particle)
			: width(width), height(height)
		{
			grid.resize(height);
			for (int y = 0; y < height; y++) {
				grid[y].resize(width, nullptr);
			}
			particles.reserve(num_particle);
			for (int i = 0; i < num_particle; i++) {
				double x = random() * (width - 1);
				double y = random() * (height - 1);
				//create particles
				DLAParticle particle(x, y);
				particles.push_back(std::move(particle));
			}
			for (DLAParticle& p : particles) {
				//create reference grid
				auto [x, y] = p.position;
				int ix = (int)(x + 0.5);
				int iy = (int)(y + 0.5);
				p.next = grid[iy][ix];
				if (grid[iy][ix]) grid[iy][ix]->prev = &p;
				grid[iy][ix] = &p;
			}
		}

		bool DLABreakModel::test(int x, int y)
		{
			DLAParticle* list = grid[y][x];
			if (!list) return false;
			for (DLAParticle* p = list; p; p = p->next) {
				p->live = false;
			}
			grid[y][x] = nullptr;
			return true;
		}

		void DLABreakModel::update(const CellList2D& /*cells*/, const std::vector<Point>& /*broken_list*/)
		{
			const double pi = 3.141592653589793;
			for (DLAParticle& p : particles) {
				if (!p.live) continue;
				auto& [x, y] = p.position;
				int old_ix = int(x + 0.5);
				int old_iy = int(y + 0.5);
				double r = 1.0 * random();
				double t = random() * 2 * pi;
				double dx = r * std::cos(t);
				double dy = r * std::sin(t);
				double new_x = x + dx;
				double new_y = y + dy;
				int new_ix = int(new_x + 0.5);
				int new_iy = int(new_y + 0.5);
				if (0 <= new_ix && 0 <= new_iy && new_ix < width && new_iy < height) {
					//update position
					x = new_x;
					y = new_y;
					//re-connect list nodes
					if (old_ix != new_ix || old_iy != new_iy) {
						//remove this particle from current list
						if (p.prev) p.prev->next = p.next;
						if (p.next) p.next->prev = p.prev;
						if (grid[old_iy][old_ix] == &p) grid[old_iy][old_ix] = p.next;
						//append this to another list
						p.prev = nullptr;
						p.next = grid[new_iy][new_ix];
						if (grid[new_iy][new_ix]) grid[new_iy][new_ix]->prev = &p;
						grid[new_iy][new_ix] = &p;
					}
				}
			}
		}
	}
}