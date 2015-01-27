#include <math.h>
#include "vertex.h"

double Geometry::Vertex2D::distance(const Geometry::Vertex2D & other) const {
	double comp_x = x() - other.x();
	double comp_y = y() - other.y();
	double distance = sqrt(comp_x * comp_x + comp_y * comp_y);
	return distance;
}

bool Geometry::Vertex2D::operator==(const Geometry::Vertex2D & other) const { 
	if (m_x == other.m_x && m_y == other.m_y) return true; 
	return false; 
}

double Geometry::Vertex3D::distance(const Geometry::Vertex3D & other) const {
	double comp_x = x() - other.x();
	double comp_y = y() - other.y();
	double comp_z = z() - other.z();
	double distance = sqrt(comp_x * comp_x + comp_y * comp_y + comp_z * comp_z);
//	fprintf(stderr, "Vectors [%f, %f, %f] [%f, %f, %f] distance is %f\n", x(), y(), z(), other.x(), other.y(), other.z(), distance);
	return distance;
}

bool Geometry::Vertex3D::operator==(const Geometry::Vertex3D & other) const { 
	if (Vertex2D::operator==(other) && m_z == other.m_z) return true; 
	return false; 
}

