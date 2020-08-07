#pragma once
#include "model.h"
#include <vector>


namespace lichtenberg {
	namespace model {
		class ManualBreakModel : public BreakModel
		{
		private:
			int width;
			int height;
			float* map;
		public:
			typedef std::vector<std::vector<float>> Grid;
			ManualBreakModel(int width, int height, const Grid& source, float min_guarantee = 0.05);
			ManualBreakModel(const ManualBreakModel&) = delete;
			ManualBreakModel& operator =(const ManualBreakModel&) = delete;
			~ManualBreakModel();

			bool test(int x, int y);
		};
	}
}

