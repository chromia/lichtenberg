#pragma once
#include <vector>
#include <memory>
#include "../core.h"

namespace lichtenberg {
	namespace model {

		class BreakModel {
		public:
			virtual ~BreakModel();
			virtual bool test(int x, int y);
			virtual void update(const CellList2D& cells, const std::vector<Point>& broken_list);
			virtual void onbreak(int x, int y);
		};
		typedef std::shared_ptr<BreakModel> BreakModelPtr;


		/*
			An uniformed probalibity model of dielectric breakdown
			by Haraguchi+Tsukasa
			http://lazylifeeasyexit.blog67.fc2.com/blog-entry-53.html
		*/
		class DefaultBreakModel : public BreakModel
		{
		public:
			DefaultBreakModel();
			bool test(int x, int y);
		};
	}
}
