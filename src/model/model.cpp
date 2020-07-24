#include "model.h"
#include <cstdlib>
#include <exception>

namespace lichtenberg {
	namespace model {
		//BreakModel
		BreakModel::~BreakModel()
		{
		}

		bool BreakModel::test(int /*x*/, int /*y*/)
		{
			throw std::exception("not implemented");
		}

		void BreakModel::update(const CellList2D& /*cells*/, const std::vector<Point>& /*broken_list*/) {
		}

		void BreakModel::onbreak(int /*x*/, int /*y*/)
		{
		}


		//DefaultBreakModel
		DefaultBreakModel::DefaultBreakModel()
		{
		}

		bool DefaultBreakModel::test(int /*x*/, int /*y*/)
		{
			//uses simple probability model
			//fifty - fifty
			int rnd = rand() % RAND_MAX;
			return rnd < (RAND_MAX >> 1);
		}
	}
}