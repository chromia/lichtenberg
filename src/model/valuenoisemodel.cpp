#include "valuenoisemodel.h"
#include "../util/noise.h"
#include <stdexcept>

namespace lichtenberg {
	namespace model {
		ValueNoiseBreakModel::ValueNoiseBreakModel(int width, int height, float min_guarantee,
			int seed, float scale, float persistence, int octaves)
			: width(width), height(height), map(nullptr)
		{
			if (width <= 0 || height <= 0) {
				throw std::invalid_argument("width and height must be positive.");
			}

			map = new float[width * height];
			double max_rate = 1.0 - min_guarantee;
			util::noise_init(seed);
			for (int y = 0; y < height; y++) {
				float* ptr = map + y * width;
				for (int x = 0; x < width; x++) {
					double f = util::noise(x / scale, y / scale, persistence, octaves);
					double n = (f + 1.0) / 2.0; //[-1,+1] -> [0,1]
					n = n * max_rate + min_guarantee; //[0, 1] -> [min_guarantee, 1.0]
					*ptr++ = (float)n;
				}
			}
		}
		ValueNoiseBreakModel::~ValueNoiseBreakModel()
		{
			delete[] map;
		}

		bool ValueNoiseBreakModel::test(int x, int y)
		{
			float th = map[y * width + x];
			float r = (float)(rand() % RAND_MAX) / RAND_MAX;
			return r < th;
		}
		float ValueNoiseBreakModel::get(int x, int y) const
		{
			return map[y * width + x];
		}
	}
}
