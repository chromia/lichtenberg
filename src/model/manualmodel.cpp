#pragma once
#include "manualmodel.h"
#include <cstdlib>

namespace lichtenberg {
	namespace model {
		ManualBreakModel::ManualBreakModel(int width, int height, const std::vector<float>& source, float min_guarantee)
			: width(width), height(height)
		{
			const float* p_source = &source[0];
			float max_scale = 1.0f - min_guarantee;
			map = new float[width * height];
			for (int y = 0; y < height; y++) {
				float* dst = map + y * width;
				const float* src = p_source + y * width;
				for (int x = 0; x < width; x++) {
					float f = *src++;
					float th = f * max_scale + min_guarantee;
					*dst++ = th;
				}
			}
		}
		ManualBreakModel::~ManualBreakModel()
		{
			delete[] map;
		}

		bool ManualBreakModel::test(int x, int y)
		{
			float th = map[y * width + x];
			float r = (float)(rand() % RAND_MAX) / RAND_MAX;
			return r < th;
		}
	}
}

