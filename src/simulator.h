#pragma once
#include "core.h"
#include "model/model.h"
#include <functional>
#include <tuple>
#include <memory>


namespace lichtenberg {

	class Simulator
	{
	private:
		model::BreakModelPtr model;
		bool def_up, def_down, def_left, def_right;
		std::function<bool(int, int)> callback_broken;

	public:
		CellList2D cells;

		Simulator(int width, int height, model::BreakModelPtr model,
			bool up = true, bool down = true, bool left = true, bool right = true);
		Simulator(const Simulator&) = delete;
		Simulator& operator =(const Simulator&) = delete;

		void breakdown(int x, int y);

		void insulate(int x, int y);
		void insulate_square(int x1, int y1, int x2, int y2, bool fill = false);
		void insulate_circle(int x, int y, int radius, bool fill = false);

		static const int MAX_LOOP_COUNT = 50000;
		typedef std::function<bool(int, int)> BreakCallBack;
		typedef std::function<bool(int, int, const CellList2D*)> LoopCallBack;
		void simulate(int maxloopcount = MAX_LOOP_COUNT, BreakCallBack callback_on_break = nullptr,
			LoopCallBack callback_on_loop = nullptr);

	private:
		bool check_dir(int x, int y);
		std::tuple<bool, bool> breaking(int x, int y, std::vector<Point>& points, BreakCallBack callback);
		void insulate_cell(int x, int y);
	};

}