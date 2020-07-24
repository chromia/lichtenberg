#pragma once
#include "core.h"
#include <vector>


namespace lichtenberg {

	class Tree
	{
	private:
		int width;
		int height;
		TreeNode root;
		typedef std::vector<TreeNode> NodeList;
		NodeList nodes;
	public:
		Tree(const CellList2D& list);
		~Tree();

		const TreeNode* get_node(int x, int y) const;
		std::vector<Point> get_path(int x1, int y1, int x2, int y2) const;
		std::vector<Leaf> get_leaves() const;
	};

}
