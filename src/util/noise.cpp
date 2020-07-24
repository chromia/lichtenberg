/*
	Value Noise Implementation

	Original is by Hugo Elias.
	https://web.archive.org/web/20160530124230/http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
*/

#include "noise.h"
#include <math.h>

namespace lichtenberg {
	namespace util {
		static int seed = 0;

		void noise_init(int seed_)
		{
			seed = seed_;
		}

		double rand(int x, int y)
		{
			int n = x + y * 57 + seed;
			n = (n << 13) ^ n;
			return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
		}

		double cosine_interpolate(double a, double b, double x)
		{
			double ft = x * 3.141592653589793;
			double f = (1 - cos(ft)) * 0.5;
			return a * (1 - f) + b * f;
		}

		double smooth(int x, int y)
		{
			double corners = (rand(x - 1, y - 1) + rand(x + 1, y - 1) + rand(x - 1, y + 1) + rand(x + 1, y + 1)) / 16;
			double sides = (rand(x - 1, y) + rand(x + 1, y) + rand(x, y - 1) + rand(x, y + 1)) / 8;
			double center = rand(x, y) / 4;
			return corners + sides + center;
		}

		double interpolate(double x, double y)
		{
			int xi = int(x);
			double xf = xi - x;
			int yi = int(y);
			double yf = yi - y;
			double v1 = smooth(xi, yi);
			double v2 = smooth(xi + 1, yi);
			double v3 = smooth(xi, yi + 1);
			double v4 = smooth(xi + 1, yi + 1);

			double u1 = cosine_interpolate(v1, v2, xf);
			double u2 = cosine_interpolate(v3, v4, xf);
			return cosine_interpolate(u1, u2, yf);
		}

		double noise(double x, double y, double persistence, int octaves)
		{
			double total = 0.0;

			for (int i = 0; i < octaves; i++) {
				int frequency = 1 << i;
				double amplitude = pow(persistence, i);
				total = total + interpolate(x * frequency, y * frequency) * amplitude;
			}

			return total;
		}

	}
}