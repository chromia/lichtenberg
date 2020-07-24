#include "simulator.h"

namespace lichtenberg {

	Simulator::Simulator(int width, int height, model::BreakModelPtr model,
		bool up, bool down, bool left, bool right)
		: cells(width, height), model(model),
		def_up(up), def_down(down), def_left(left), def_right(right)
	{
	}

	//check whether the cell receive electrons from surrounding cells
	bool Simulator::check_dir(int x, int y)
	{
		if (x != 0) {
			Direction dir_left = cells.get_dir(x - 1, y);
			if (dir_left == Direction::Right) return true;
		}
		if (x != cells.width - 1) {
			Direction dir_right = cells.get_dir(x + 1, y);
			if (dir_right == Direction::Left) return true;
		}
		if (y != 0) {
			Direction dir_up = cells.get_dir(x, y - 1);
			if (dir_up == Direction::Down) return true;
		}
		if (y != cells.height - 1) {
			Direction dir_down = cells.get_dir(x, y + 1);
			if (dir_down == Direction::Up) return true;
		}
		return false;
	}

	//enlarges the breaking area
	//points : list of target cells
	//return true if four adjoining cells are broken already
	std::tuple<bool, bool> Simulator::breaking(int x, int y, std::vector<Point>& points, BreakCallBack callback)
	{
		bool cancel = false;
		bool complete = false;

		if (!model) {
			model = model::BreakModelPtr(new model::DefaultBreakModel());
		}
		model::BreakModel* m = model.get();

		//Common procedures
		auto process_broken_cell = [&](int ix, int iy, Direction dir, bool& flag) {
			//Test breakdown in the cell
			if (m->test(ix, iy)) {
				cells.set_broken(ix, iy);	//set 'broken' flag
				cells.set_dir(ix, iy, dir);	//record direction from me
				points.push_back(Point(ix, iy));
				flag = true;
				m->onbreak(ix, iy); // callback to model
				if (callback) cancel |= callback(ix, iy); //callback to user
			}
		};

		if (cells.get_broken(x, y)) {
			//This cell has been already broken
			//Enlarges the area

			bool up, down, left, right;
			up = down = left = right = true;

			//the upper side
			if (def_up && y != 0) {
				up = cells.get_broken(x, y - 1);
				if (!up) {
					process_broken_cell(x, y - 1, Direction::Down, up);
				}
			}
			//the lower side
			if (def_down && y != cells.height - 1) {
				down = cells.get_broken(x, y + 1);
				if (!down) {
					process_broken_cell(x, y + 1, Direction::Up, down);
				}
			}
			//the left side
			if (def_left && x != 0) {
				left = cells.get_broken(x - 1, y);
				if (!left) {
					process_broken_cell(x - 1, y, Direction::Right, left);
				}
			}
			//the right side
			if (def_right && x != cells.width - 1) {
				right = cells.get_broken(x + 1, y);
				if (!right) {
					process_broken_cell(x + 1, y, Direction::Left, right);
				}
			}

			//Mission complete
			complete = left && right && up && down;
		}
		return { complete, cancel };
	}

	void Simulator::breakdown(int x, int y)
	{
		if (x < 0 || x >= cells.width || y < 0 || y >= cells.height) return;
		cells.set_broken(x, y);
		cells.set_dir(x, y, Direction::None);
	}

	void Simulator::insulate(int x, int y)
	{
		if (x < 0 || x >= cells.width || y < 0 || y >= cells.height) return;
		cells.set_insulated(x, y);
		cells.set_broken(x, y);
		cells.set_dir(x, y, Direction::None);
	}

	void Simulator::insulate_square(int x1, int y1, int x2, int y2, bool fill)
	{
		if (x1 > x2) {
			int tmp = x1;
			x1 = x1;
			x2 = tmp;
		}
		if (y1 > y2) {
			int tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		if (x1 < 0) x1 = 0;
		if (y1 < 0) y1 = 0;
		if (x2 > cells.width) x2 = cells.width;
		if (y2 > cells.height) y2 = cells.height;

		if (fill) {
			for (int iy = y1; iy < y2; iy++) {
				for (int ix = x1; ix < x2; ix++) {
					insulate(ix, iy);
				}
			}
		}
		else {
			for (int x = x1; x < x2; x++) {
				insulate(x, y1);
				insulate(x, y2 - 1);
			}
			for (int y = y1 + 1; y < y2 - 1; y++) {
				insulate(x1, y);
				insulate(x2 - 1, y);
			}
		}

	}
	void Simulator::insulate_circle(int cx, int cy, int radius, bool fill)
	{
		// Michener's algorithm
		// reference
		// http://atarasevich.blogspot.jp/2015/07/on-circle-rasterization-algorithms.html
		// http://dencha.ojaru.jp/programs_07/pg_graphic_09a1.html

		int d = 3 - 2 * radius;
		int dy = radius;
		int dx = 0;

		while (dx <= dy) {
			if (d < 0) {
				d += 6 + 4 * dx;
			}
			else {
				d += 10 + 4 * dx - 4 * dy;
				dy--;
			}

			if (fill) {
				for (int x = (cx - dy); x <= (cx + dy); x++) {
					insulate(x, cy + dx);
				}
				for (int x = (cx - dx); x <= (cx + dx); x++) {
					insulate(x, cy + dy);
				}
				for (int x = (cx - dy); x <= (cx + dy); x++) {
					insulate(x, cy - dx);
				}
				for (int x = (cx - dx); x <= (cx + dx); x++) {
					insulate(x, cy - dy);
				}
			}
			else {
				insulate(cx + dy, cy + dx);
				insulate(cx + dx, cy + dy);
				insulate(cx - dx, cy + dy);
				insulate(cx - dy, cy + dx);
				insulate(cx - dy, cy - dx);
				insulate(cx - dx, cy - dy);
				insulate(cx + dx, cy - dy);
				insulate(cx + dy, cy - dx);
			}
			dx++;
		}
	}

	//This is the core of algorithm
	void Simulator::simulate(int max_loop, BreakCallBack callback_on_break, LoopCallBack callback_on_loop)
	{
		int w = cells.width;
		int h = cells.height;

		//to accelerate, use two array of points alternately
		std::vector<Point> points1;
		std::vector<Point> points2;
		std::vector<Point>* pF, * pB, * ptmp;
		points1.reserve(w * h);
		points2.reserve(w * h);
		pF = &points1;
		pB = &points2;

		//rake up all broken cells
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				if (cells.get_broken(x, y) && !cells.get_insulated(x, y)) {
					pF->push_back(Point(x, y));
				}
			}
		}

		//enlarge broken area
		int loopcount = 0;
		while (!pF->empty()) {
			//update model
			if (model) {
				model->update(cells, *pF);
			}
			//callback
			if (callback_on_loop) {
				bool cancel = callback_on_loop(loopcount, max_loop, &cells);
				if (cancel) break;
			}

			bool flag_break = false;
			for (auto it = pF->begin(); it != pF->end(); ++it) {
				auto [x, y] = *it;
				auto [complete, cancel] = breaking(x, y, *pB, callback_on_break);
				if (!complete) {
					//this cell has non-broken neighbor, then add it in next loop
					pB->push_back(*it);
				}
				if (cancel) {
					flag_break = true;
					break;
				}
			}
			pF->clear();
			ptmp = pF; pF = pB; pB = ptmp; //swap
			loopcount++;
			if (loopcount == max_loop) {
				//Give up...
				break;
			}
			if (flag_break) {
				//Canceled
				break;
			}
		}

		//calc cell level
		//first of all, base point is level 1
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				if (!check_dir(x, y)) {
					cells.set_count(x, y, 1);
				}
			}
		}
		//the cell which receive the base point's electron is level 2.
		//and level3, 4 ... similarly.
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int cc = cells.get_count(x, y);
				if (cc == 1) {
					int cx = x;
					int cy = y;
					while (1) {
						int tx = cx;
						int ty = cy;
						Direction dir = cells.get_dir(cx, cy);
						if (dir == Direction::Up) {
							ty = cy - 1;
						}
						else if (dir == Direction::Down) {
							ty = cy + 1;
						}
						else if (dir == Direction::Left) {
							tx = cx - 1;
						}
						else if (dir == Direction::Right) {
							tx = cx + 1;
						}
						else if (dir == Direction::None)
						{
							break;
						}

						int tc = cells.get_count(tx, ty);
						if (tc > cc) {
							break; //this cell is connected with more long flow
						}
						else {
							cells.set_count(tx, ty, cc + 1);
						}

						cx = tx;
						cy = ty;
						cc = cc + 1;
					}
				}
			}
		}
	}
}
