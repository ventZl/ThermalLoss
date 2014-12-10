#include <math.h>
#include "building.h"

#define pow2(x)	((x) * (x))

double Model::Point::distance(const Point * other) const {
	return sqrt(pow2(this->x() - other->x()) + pow2(this->y() - other->y())); 
}

bool Model::Building::validate() {
//	unsigned max_point = m_points.size();
	return true;
}

bool Model::Room::validate() {
	// we need equal number of walls and it's corners. first one is reused for both first and last wall
	if (m_points.size() != m_walls.size()) return false;
/*	for (PointRefVector::const_iterator it = m_points.begin(); it != m_points.end(); ++it) {
		if (m_building->point((*it)->getValue()) == NULL) return false;
	}*/
	return true;
}

const Model::Point & Model::Building::point(unsigned offset) const {
	if (offset < m_points.size()) return *m_points[offset];
	throw std::range_error("Offset out of range");
}


const Model::Room & Model::Building::room(unsigned offset) const { 
	if (offset < m_rooms.size()) return *m_rooms[offset]; 
	throw std::runtime_error("Offset out of range"); 
}

const Model::WallType & Model::Building::wallType(unsigned offset) const { 
	if (offset < m_walltype.size()) return *m_walltype[offset]; 
	throw std::runtime_error("Offset out of range"); 
}

	
/** resistance K/W */
/*bool Model::Window::validate() {
	m_resistance = 1.0/(surface() * conductivity());
}*/

/*
bool Model::WallType::compute(Model::MaterialLibrary * library) {
	double resistance = 0;
	for (Model::MaterialUsageVector::const_iterator it = m_composition.begin(); it != m_composition.end(); ++it) {
		std::string materialName = (*it)->material();
		const Model::Material * material = library->material(materialName);
		if (material != NULL) {
			resistance += (*it)->width() / material->conductivity();
		}
	}
}

void Model::Wall::compute(Model::Room * room, const Model::Point * start_vertex, const Model::Point * end_vertex) {
	m_room = room;
	m_start_vertex = start_vertex;
	m_end_vertex = end_vertex;
	if (m_start_vertex == NULL || m_end_vertex == NULL) return;

	m_length = m_start_vertex->distance(m_end_vertex);

	double surface = m_length * room->height();

	for (Model::WindowVector::iterator it = m_windows.begin(); it != m_windows.end(); ++it) {
		(*it)->compute();
		surface -= (*it)->surface();
	}

	printf("Wall surface is: %.5f m^2\n", surface);
}

void Model::Room::compute(Model::Building * building) {
	m_building = building;
	Model::PointRefVector::const_iterator pit1, pit2;
	pit1 = m_points.begin();
	pit2 = pit1 + 1;
	for (Model::WallVector::iterator it = m_walls.begin(); it != m_walls.end(); ++it) {
		const Point * point1 = building->point((*pit1)->getValue());
		const Point * point2 = building->point((*pit2)->getValue());
		(*it)->compute(this, point1, point2);
		++pit1; ++pit2;
		if (pit2 == m_points.end()) pit2 = m_points.begin();
	}

	size_t points_count = m_points.size();
	m_area = 0;
	for (size_t q = 0; q <= points_count; ++q) {
//		printf("q is %d\n", q);
		const Model::Point * this_point;
		const Model::Point * next_point;
		const Model::Point * prev_point;
		if (q == 0) prev_point = building->point((*(m_points.rbegin()))->getValue());
		else prev_point = building->point(m_points[q-1]->getValue());
		next_point = building->point(m_points[(q+1)%points_count]->getValue());
		this_point = building->point(m_points[(q)%points_count]->getValue());
//		printf("Points are prev [%f, %f], cur [%f, %f], next [%f, %f]\n", prev_point->x(), prev_point->y(), this_point->x(), this_point->y(), next_point->x(), next_point->y());
		m_area += this_point->x() * (next_point->y() - prev_point->y());
//		printf("area is %f\n", m_area);
	}	
	m_area = fabs(m_area / 2);
	printf("Room area is %f m^2\n", m_area);
	return;
}

Model::Losses * Model::Building::compute(MaterialLibrary * lib) {
	m_library = lib;
	for (RoomVector::iterator it = m_rooms.begin(); it != m_rooms.end(); ++it) {
		(*it)->compute(this);
	}
	return NULL;
}*/

Model::Building * Model::loadBuilding(std::string fileName) {
	Model::Building * b = new Model::Building();
	if (parse(fileName, b)) {
	} else {
		printf("Error while parsing building\n");
		delete b;
		b = NULL;
	}
	return b;
}

