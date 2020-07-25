
#include "simulator.h"
#include "model/valuenoisemodel.h"
#include "model/manualmodel.h"
#include "model/dbmmodel.h"
#include "model/fastdbm.h"
#include "model/dlamodel.h"
#include "util/electricfield.h"
#include "tree.h"
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <cstdlib>

using namespace lichtenberg;
using namespace lichtenberg::model;

void set_random_seed(int seed)
{
	srand(seed);
}

namespace py = pybind11;
PYBIND11_MODULE(_lichtenberg, m) {
	m.doc() = "lichtenberg: an implementation of lichtenberg figure";
	m.def("set_random_seed", &set_random_seed);
	m.def("make_electric_field", &util::make_electric_field,
		py::arg("width"), py::arg("height"), py::arg("points"),
		py::arg("max_loop") = 1000, py::arg("eps") = 1.0e-6f, py::arg("sor_coef") = 1.5f);

	py::class_<BreakModel, BreakModelPtr>(m, "BreakModel")
		.def("test", &BreakModel::test,
			py::arg("x"), py::arg("y"))
		.def("update", &BreakModel::update,
			py::arg("cells"), py::arg("broken_list"));

	py::class_<DefaultBreakModel, std::shared_ptr<DefaultBreakModel>, BreakModel>(m, "DefaultBreakModel")
		.def(py::init<>());

	py::class_<ValueNoiseBreakModel, std::shared_ptr<ValueNoiseBreakModel>, BreakModel>(m, "ValueNoiseBreakModel")
		.def(py::init<int, int, float, int, float, float, int>(),
			py::arg("width"), py::arg("height"),
			py::arg("min_guarantee") = 0.05f, py::arg("seed") = int(0),
			py::arg("scale") = 1.0f, py::arg("persistence") = 0.5f, py::arg("octaves") = int(5))
		.def("get", &ValueNoiseBreakModel::get,
			py::arg("x"), py::arg("y"));

	py::class_<ManualBreakModel, std::shared_ptr<ManualBreakModel>, BreakModel>(m, "ManualBreakModel")
		.def(py::init<int, int, std::vector<float>&, float>(),
			py::arg("width"), py::arg("height"),
			py::arg("source"), py::arg("min_guarantee") = 0.05f);

	py::class_<DBMCell>(m, "DBMCell")
		.def(py::init<double, int>(),
			py::arg("potential") = DBM_POTENTIAL_VOID, py::arg("lock") = false)
		.def_readwrite("potential", &DBMCell::potential)
		.def_readwrite("lock", &DBMCell::lock);

	py::class_<DielectricBreakModel, std::shared_ptr<DielectricBreakModel>, BreakModel>(m, "DielectricBreakModel")
		.def(py::init<int, int, DBMGrid, double, double>(),
			py::arg("width"), py::arg("height"), py::arg("initial_state"),
			py::arg("min_guarantee") = 0.0, py::arg("eta") = 1.0)
		.def("get_potential", &DielectricBreakModel::get_potential);

	py::class_<FastDBM, std::shared_ptr<FastDBM>, BreakModel>(m, "FastDBM")
		.def(py::init<int, int, double, double>(),
			py::arg("width"), py::arg("height"),
			py::arg("min_guarantee") = 0.0, py::arg("eta") = 1.0);

	py::class_<DLABreakModel, std::shared_ptr<DLABreakModel>, BreakModel>(m, "DLABreakModel")
		.def(py::init<int, int, int>(),
			py::arg("width"), py::arg("height"), py::arg("num_particle"));

	py::enum_<Direction>(m, "Direction")
		.value("Nim", Direction::None)
		.value("Up", Direction::Up)
		.value("Left", Direction::Left)
		.value("Down", Direction::Down)
		.value("Right", Direction::Right);

	py::class_<CellList2D>(m, "CellList2D")
		.def(py::init<>())
		.def(py::init<int, int>())
		.def_readonly("width", &CellList2D::width)
		.def_readonly("height", &CellList2D::height)
		.def("is_valid", &CellList2D::is_valid)
		.def("load", &CellList2D::load)
		.def("save", &CellList2D::save)
		.def("get_broken", &CellList2D::get_broken,
			py::arg("x"), py::arg("y"))
		.def("set_broken", &CellList2D::set_broken,
			py::arg("x"), py::arg("y"))
		.def("get_count", &CellList2D::get_count,
			py::arg("x"), py::arg("y"))
		.def("set_count", &CellList2D::set_count,
			py::arg("x"), py::arg("y"), py::arg("n"))
		.def("add_count", &CellList2D::add_count,
			py::arg("x"), py::arg("y"))
		.def("get_dir", &CellList2D::get_dir,
			py::arg("x"), py::arg("y"))
		.def("set_dir", &CellList2D::set_dir,
			py::arg("x"), py::arg("y"), py::arg("dir"))
		.def("get_insulated", &CellList2D::get_insulated,
			py::arg("x"), py::arg("y"))
		.def("set_insulated", &CellList2D::set_insulated,
			py::arg("x"), py::arg("y"))
		.def("get_max_count", &CellList2D::get_max_count);

	py::class_<Simulator>(m, "Simulator")
		.def(py::init<int, int, BreakModelPtr, bool, bool, bool, bool>(),
			py::arg("width"), py::arg("height"), py::arg("model") = BreakModelPtr(),
			py::arg("up") = true, py::arg("down") = true, py::arg("left") = true, py::arg("right") = true)
		.def("breakdown", &Simulator::breakdown,
			py::arg("x"), py::arg("y"))
		.def("insulate", &Simulator::insulate)
		.def("insulate_square", &Simulator::insulate_square,
			py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), py::arg("fill") = false)
		.def("insulate_circle", &Simulator::insulate_circle,
			py::arg("cx"), py::arg("cy"), py::arg("radius"), py::arg("fill") = false)
		.def("simulate", &Simulator::simulate, py::arg("max_loop") = Simulator::MAX_LOOP_COUNT,
			py::arg("callback_on_break") = nullptr, py::arg("callback_on_loop") = nullptr)
		.def_readwrite("cells", &Simulator::cells);

	py::class_<TreeNode>(m, "TreeNode")
		.def(py::init<>())
		.def_readwrite("parent", &TreeNode::parent)
		.def_readwrite("prev", &TreeNode::prev)
		.def_readwrite("next", &TreeNode::next)
		.def_readwrite("child", &TreeNode::child)
		.def_readonly("point", &TreeNode::point)
		.def("set_point", &TreeNode::set_point,
			py::arg("point"));

	py::class_<Leaf>(m, "Leaf")
		.def(py::init<const TreeNode*, int>(),
			py::arg("node"), py::arg("count"))
		.def_readonly("node", &Leaf::node)
		.def_readonly("count", &Leaf::count);

	py::class_<Tree>(m, "Tree")
		.def(py::init<const CellList2D&>())
		.def("get_node", &Tree::get_node,
			py::arg("x"), py::arg("y"))
		.def("get_path", &Tree::get_path,
			py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"))
		.def("get_leaves", &Tree::get_leaves);

}

