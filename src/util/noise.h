/*
	Value Noise Implementation

	Original is by Hugo Elias.
	https://web.archive.org/web/20160530124230/http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
*/

#pragma once

namespace lichtenberg {
	namespace util {
		void noise_init(int seed);
		extern double noise(double x, double y, double persistence, int octaves);
	}
}
