#pragma once
#include <tuple>

namespace lichtenberg {

	typedef std::tuple<int, int> Point;  //(x, y)
	typedef std::tuple<int, int, int> PointC;  //(x, y, count)

	Point convertPoint(const PointC& src);

	//express direction of electron
	enum class Direction
	{
		None,	//base point
		Up,	//to Y-
		Left,	//to X-
		Down,	//to Y+
		Right,	//to X+
	};

	struct CellInfo
	{
		Direction direction;  //The direction to which the cell broke me
		int count;  //Distance from start cell
		bool broken;  //The flag which this cell is broken or not
		bool insulated;  //The flag which this cell is protected or not
	};

	class CellList2D
	{
	private:
		CellInfo* cells;

	public:
		int width;
		int height;

		CellList2D();
		CellList2D(int width, int height);
		CellList2D(const CellList2D& other);
		CellList2D& operator =(const CellList2D& other);
		~CellList2D();

		bool is_valid() const;
		void clear();

		//accessor to each cell info( no bounds checking )
		bool get_broken(int x, int y) const;
		void set_broken(int x, int y);
		int get_count(int x, int y) const;
		void set_count(int x, int y, int n);
		void add_count(int x, int y);
		Direction get_dir(int x, int y) const;
		void set_dir(int x, int y, Direction dir);
		bool get_insulated(int x, int y) const;
		void set_insulated(int x, int y);

		int get_max_count() const;

		//archiving
		bool save(const char* filepath) const;
		bool load(const char* filepath);

		long long int addr() const;
	};

	class TreeNode
	{
	public:
		TreeNode* parent;
		TreeNode* prev;
		TreeNode* next;
		TreeNode* child;
		PointC point;
		TreeNode();
		void set_point(const PointC& p);
	};

	class Leaf
	{
	public:
		const TreeNode* node;
		int count;
		Leaf(const TreeNode* node, int count);
	};

}
