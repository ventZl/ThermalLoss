#ifndef __SRC_GEOM_POLYGON_H__
#define __SRC_GEOM_POLYGON_H__

#include <vector>
#include "vertex.h"

namespace Geometry {

class Polygon {
public:
	Polygon();
	bool operator==(const Polygon & other) const;
	Polygon operator-(const Polygon & other) const;
	Polygon operator+(const Polygon & other) const;

	void addVertex(const Vertex2D & vertex);
	double area() const;

protected:
	std::vector<Vertex2D> m_vertices;
};

}

#endif

