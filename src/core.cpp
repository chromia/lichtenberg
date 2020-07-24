#include "core.h"
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>


namespace lichtenberg {

	Point convertPoint(const PointC& src)
	{
		return Point(std::get<0>(src), std::get<1>(src));
	}

	//CellList2D
	CellList2D::CellList2D()
		: width(0), height(0), cells(nullptr)
	{
	}
	CellList2D::CellList2D(int width_, int height_)
	{
		width = width_;
		height = height_;
		cells = new CellInfo[width * height];
		clear();
	}
	CellList2D::CellList2D(const CellList2D& other)
	{
		this->width = other.width;
		this->height = other.height;
		int n = other.width * other.height;
		this->cells = new CellInfo[n];
		memcpy(this->cells, other.cells, n * sizeof(CellInfo));
	}
	CellList2D& CellList2D::operator=(const CellList2D& other)
	{
		if (this != &other) {
			int n = other.width * other.height;
			if (this->width != other.width || this->height != other.height) {
				this->width = other.width;
				this->height = other.height;
				delete[] this->cells;
				this->cells = new CellInfo[n];
				for (int i = 0; i < n; i++) {
					this->cells[i] = other.cells[i];
				}
			}
			else {
				for (int i = 0; i < n; i++) {
					this->cells[i] = other.cells[i];
				}
			}
		}
		return *this;
	}
	CellList2D::~CellList2D()
	{
		delete cells;
	}

	bool CellList2D::is_valid() const
	{
		return cells != nullptr && width > 0 && height > 0;
	}

	void CellList2D::clear()
	{
		if (is_valid()) {
			memset(cells, 0, sizeof(CellInfo) * (width * height));
		}
	}

	bool CellList2D::get_broken(int x, int y) const
	{
		return cells[y * width + x].broken;
	}
	void CellList2D::set_broken(int x, int y)
	{
		cells[y * width + x].broken = true;
	}
	int CellList2D::get_count(int x, int y) const
	{
		return cells[y * width + x].count;
	}
	void CellList2D::set_count(int x, int y, int n)
	{
		cells[y * width + x].count = n;
	}
	void CellList2D::add_count(int x, int y)
	{
		cells[y * width + x].count++;
	}
	Direction CellList2D::get_dir(int x, int y) const
	{
		return cells[y * width + x].direction;
	}
	void CellList2D::set_dir(int x, int y, Direction dir)
	{
		cells[y * width + x].direction = dir;
	}
	bool CellList2D::get_insulated(int x, int y) const
	{
		return cells[y * width + x].insulated;
	}
	void CellList2D::set_insulated(int x, int y)
	{
		cells[y * width + x].insulated = true;
	}

	int CellList2D::get_max_count() const
	{
		int maxcount = 0;
		int w = width;
		int h = height;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int count = get_count(x, y);
				if (count > maxcount) {
					maxcount = count;
				}
			}
		}
		return maxcount;
	}

	bool CellList2D::save(const char* filepath) const
	{
		if (!is_valid()) return false;
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
		FILE* fp = fopen(filepath, "wb");
#ifdef _MSC_VER
#pragma warning(default:4996)
#endif
		if (!fp) return false;

		//[version1]
		//header
		const int version = 1;
		if (fwrite(&version, sizeof(int), 1, fp) != 1) goto err;
		if (fwrite(&width, sizeof(int), 1, fp) != 1) goto err;
		if (fwrite(&height, sizeof(int), 1, fp) != 1) goto err;
		//data
		{
			const size_t size = width * height;
			if (fwrite(cells, sizeof(CellInfo), size, fp) != size) goto err;
		}
		fclose(fp);
		return true;

	err:
		fclose(fp);
		return false;
	}
	bool CellList2D::load(const char* filepath)
	{
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
		FILE* fp = fopen(filepath, "rb");
#ifdef _MSC_VER
#pragma warning(default:4996)
#endif
		if (!fp) return false;

		if (cells) {
			delete[] cells;
		}

		int version;
		if (fread(&version, sizeof(int), 1, fp) != 1) goto err;

		if (version == 1) {
			if (fread(&width, sizeof(int), 1, fp) != 1) goto err;
			if (fread(&height, sizeof(int), 1, fp) != 1) goto err;
			cells = new CellInfo[width * height];
			{
				const size_t size = width * height;
				if (fread(cells, sizeof(CellInfo), size, fp) != size) goto err;
			}
		}
		else {
			goto err;
		}

		fclose(fp);
		return true;
	err:
		fclose(fp);
		return false;
	}

	long long int CellList2D::addr() const
	{
		return (long long int)this->cells;
	}

	//TreeNode
	TreeNode::TreeNode()
	{
		this->parent = nullptr;
		this->prev = nullptr;
		this->next = nullptr;
		this->child = nullptr;
		this->point = PointC(0, 0, 0);
	}

	void TreeNode::set_point(const PointC& p)
	{
		this->point = p;
	}

	//Leaf
	Leaf::Leaf(const TreeNode* node, int count)
	{
		this->node = node;
		this->count = count;
	}

}