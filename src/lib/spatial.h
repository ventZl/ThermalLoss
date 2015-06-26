#define _LIB_SPATIAL_H__
#define _LIB_SPATIAL_H__

namespace Spatial {

class Position {
public:
	Position(double x, double y, double z): m_x(x), m_y(y), m_z(z) {}
	double x() const { return m_x; }
	double y() const { return m_y; }
	double z() const { return m_z; }

protected:
	double m_x, m_y, m_z;
};

class Cell {
public:
	Cell(const Position & position);
};

}

#endif

