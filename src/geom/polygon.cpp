#include <math.h>
#include "polygon.h"

Geometry::Polygon::Polygon() {
}

void Geometry::Polygon::addVertex(const Vertex2D & vertex) {
	m_vertices.push_back(Vertex2D(vertex.x(), vertex.y()));
	return;
}

/* silently assumes that z() of all vertices is on the same level */
double Geometry::Polygon::area() const {
	double area = 0;
	unsigned vertex_count = m_vertices.size();
	for (unsigned q = 0; q <= vertex_count; ++q) {
//		printf("q is %d\n", q);
		const Geometry::Vertex2D * this_point;
		const Geometry::Vertex2D * next_point;
		const Geometry::Vertex2D * prev_point;
		if (q == 0) prev_point = &(m_vertices.back());
		else prev_point = &(m_vertices[q-1]);
		next_point = &(m_vertices[(q + 1) % vertex_count]);
		this_point = &(m_vertices[(q) % vertex_count]);
		area += this_point->x() * (next_point->y() - prev_point->y());
//		printf("area is %f\n", area);
	}	
	area = fabs(area / 2);
//	printf("final area is %f\n", area);
	return area;
}

