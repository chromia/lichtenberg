#include "tree.h"
#include <stack>
#include <set>
#include <algorithm>


namespace lichtenberg {

	inline void setpoint(TreeNode* p, int x, int y, int count)
	{
		p->point = std::make_tuple(x, y, count);
	}

	inline void connect1side(TreeNode* parent, TreeNode* newchild)
	{
		TreeNode* c = parent->child;
		newchild->next = c;
		if (c) c->prev = newchild;
		parent->child = newchild;
		//newchild->parent = parent;
		newchild->parent = nullptr;
	}

	inline void connect(TreeNode* parent, TreeNode* newchild)
	{
		TreeNode* c = parent->child;
		newchild->next = c;
		if (c) c->prev = newchild;
		parent->child = newchild;
		newchild->parent = parent;
	}

	struct Compare {
		int width;
		int height;
		Compare(int width, int height)
			: width(width), height(height) {}
		bool operator ()(const Point& lhs, const Point& rhs) const {
			int pl = std::get<1>(lhs) * width + std::get<0>(lhs);
			int pr = std::get<1>(rhs) * width + std::get<0>(rhs);
			return pl < pr;
		}
	};

	Tree::Tree(const CellList2D& cells)
	{
		int w = cells.width;
		int h = cells.height;
		width = w;
		height = h;
		nodes.resize(w * h);

		//link: root -> all base cells( one-way )
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				if (cells.get_dir(x, y) == Direction::None && !cells.get_insulated(x, y)) {
					TreeNode* q = &nodes[y * w + x];
					setpoint(q, x, y, cells.get_count(x, y));
					connect1side(&root, q);
				}
			}
		}

		//link: base cell -> neighbor cells
		std::stack<TreeNode*> stack;
		stack.push(&root);

		auto set_node = [&](int x, int y, TreeNode* p) {
			TreeNode* q = &nodes[y * w + x];
			setpoint(q, x, y, cells.get_count(x, y));
			connect(p, q);
		};

		while (!stack.empty()) {
			TreeNode* parent = stack.top();
			stack.pop();

			bool add = false;
			for (TreeNode* p = parent->child; p; p = p->next) {
				int x = std::get<0>(p->point);
				int y = std::get<1>(p->point);

				if (x - 1 >= 0) {
					if (cells.get_dir(x - 1, y) == Direction::Right) {
						set_node(x - 1, y, p);
						add = true;
					}
				}
				if (x + 1 < w) {
					if (cells.get_dir(x + 1, y) == Direction::Left) {
						set_node(x + 1, y, p);
						add = true;
					}
				}
				if (y - 1 >= 0) {
					if (cells.get_dir(x, y - 1) == Direction::Down) {
						set_node(x, y - 1, p);
						add = true;
					}
				}
				if (y + 1 < h) {
					if (cells.get_dir(x, y + 1) == Direction::Up) {
						set_node(x, y + 1, p);
						add = true;
					}
				}

				if (add) {
					stack.push(p);
				}
			}
		}
	}

	Tree::~Tree()
	{
	}

	const TreeNode* Tree::get_node(int x, int y) const
	{
		return &nodes[y * width + x];
	}

	const TreeNode* get_base_node(const TreeNode* node)
	{
		if (!node) {
			return nullptr;
		}
		while (node->parent) {
			node = node->parent;
		}
		return node;
	}

	std::vector<Point> Tree::get_path(int x1, int y1, int x2, int y2) const
	{
		const TreeNode* node_from = get_node(x1, y1);
		const TreeNode* node_to = get_node(x2, y2);
		// climb up to base-node from node_from
		const TreeNode* node = node_from;
		std::vector<Point> path_list_from = { Point(x1, y1) };
		Compare cp(width, height);
		std::set<Point, Compare> set_from(cp);
		set_from.insert(Point(x1, y1));
		while (node->parent) {
			if (node == node_to) {
				return path_list_from;
			}
			node = node->parent;
			Point point = convertPoint(node->point);
			path_list_from.push_back(point);
			set_from.insert(point);
		}
		// base-node means te cell broken by user manually
		const TreeNode* base_node_from = node;
		const TreeNode* base_node_to = get_base_node(node_to);
		// check if both node_from and node_to has the same base-node
		if (base_node_from != base_node_to) {
			return std::vector<Point>();
		}
		// climb up to base-node from node_to
		node = node_to;
		std::vector<Point> path_list_to = { Point(x2, y2) };
		while (node->parent) {
			node = node->parent;
			Point point = convertPoint(node->point);
			path_list_to.push_back(point);
			if (set_from.count(point) != 0) {
				// found common-node
				auto it = std::find(path_list_from.begin(), path_list_from.end(), point);
				// cut down redundant path(common-node's parent to base-node)
				path_list_from.erase(it, path_list_from.end());
				break;
			}
		}

		std::vector<Point> result;
		result.reserve(path_list_from.size() + path_list_to.size());
		for (const auto& p : path_list_from) {
			result.push_back(p);
		}
		std::for_each(
			std::make_reverse_iterator(path_list_to.end()),
			std::make_reverse_iterator(path_list_to.begin()),
			[&](const Point& p) {
				result.push_back(p);
			});
		return result;
	}

	std::vector<Leaf> Tree::get_leaves() const
	{
		std::vector<Leaf> leaves;
		int w = width;
		int h = height;
		for (int y = 0; y < h; y++) {
			int offset = y * w;
			for (int x = 0; x < w; x++) {
				const TreeNode& node = nodes[offset + x];
				if (!node.child) {
					continue;
				}
				else {
					int count = 0;
					const TreeNode* p = &node;
					while (p) {
						count++;
						p = p->parent;
					}
					Leaf leaf(&node, count);
					leaves.push_back(leaf);
				}
			}
		}
		return leaves;
	}
}