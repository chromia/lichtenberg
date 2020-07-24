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
			ManualBreakModel(int width, int height, const std::vector<float>& source, float min_guarantee = 0.05);
			ManualBreakModel(const ManualBreakModel&) = delete;
			ManualBreakModel& operator =(const ManualBreakModel&) = delete;
			~ManualBreakModel();

			bool test(int x, int y);
		};
	}
}

