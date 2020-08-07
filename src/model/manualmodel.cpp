#pragma once
#include "manualmodel.h"
#include <cstdlib>
#include <stdexcept>

namespace lichtenberg {
	namespace model {
		ManualBreakModel::ManualBreakModel(int width, int height, const Grid& source, float min_guarantee)
			: width(width), height(height), map(nullptr)
		{
			if (source.size() != height) {
				throw std::invalid_argument("The height of source grid must be equal to `height` argument.");
			}
			for (int y = 0; y < height; y++) {
				if (source[y].size() != width) {
					throw std::invalid_argument("The width of source grid must be equal to `width` argument.");
				}
			}

			float max_scale = 1.0f - min_guarantee;
			map = new float[width * height];
			for (int y = 0; y < height; y++) {
				float* dst = map + y * width;
				const std::vector<float>& line = source[y];
				for (int x = 0; x < width; x++) {
					float f = line[x];
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

