#pragma once
#include "model.h"


namespace lichtenberg {
	namespace model {
		class ValueNoiseBreakModel : public BreakModel
		{
		private:
			int width;
			int height;
			float* map;
		public:
			ValueNoiseBreakModel(int width, int height, float min_guarantee = 0.05,
				int seed = 0, float scale = 10.0, float persistence = 0.5, int octaves = 5);
			ValueNoiseBreakModel(const ValueNoiseBreakModel&) = delete;
			ValueNoiseBreakModel& operator =(const ValueNoiseBreakModel&) = delete;
			~ValueNoiseBreakModel();

			bool test(int x, int y);
			float get(int x, int y) const;
		};
	}
}

