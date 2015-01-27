#ifndef __SRC_GEOM_VERTEX2D_H__
#define __SRC_GEOM_VERTEX2D_H__

#include <../include/property.h>

namespace Geometry {

class Vertex2D {
public:
	Vertex2D(double x, double y): m_x(x), m_y(y) {}
	virtual ~Vertex2D() {}
	virtual bool operator==(const Vertex2D & other) const;
	RO_PROPERTY(double, x);
	RO_PROPERTY(double, y);

public:
	virtual double distance(const Vertex2D & other) const;
};

class Vertex3D: public Vertex2D {
public:
	Vertex3D(double x, double y, double z): Vertex2D(x, y), m_z(z) {}
	virtual ~Vertex3D() {}
	virtual bool operator==(const Vertex3D & other) const;
	RO_PROPERTY(double, z);

public:
	virtual double distance(const Vertex3D & other) const;
};


}

#endif

