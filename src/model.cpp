#include <math.h>
#include "model.h"

#define pow2(x)	((x) * (x))

double Point::distance(const Point * other) const {
	return sqrt(pow2(this->x() - other->x()) + pow2(this->y() - other->y())); 
}

bool Material::validate() {
	if (m_conductivity != 0) return true;
	return false;
}

/** Validates material library.
 * For now validation basically consists of creating name:material pair and ensuring that each material name is unique
 */
bool MaterialLibrary::validate() {
	for (MaterialVector::const_iterator it = m_materials.begin(); it != m_materials.end(); ++it) {
		if (m_materialByName.find((*it)->name()) != m_materialByName.end()) return false;
		m_materialByName.insert(NameMaterialMap::value_type((*it)->name(), *it));
	}
	return true;
}

const Material * MaterialLibrary::material(std::string & name) const {
	NameMaterialMap::const_iterator it = m_materialByName.find(name);
	if (it != m_materialByName.end()) return it->second;
	return NULL;
}

bool Building::validate() {
//	unsigned max_point = m_points.size();
	return true;
}

bool Room::validate() {
	// we need equal number of walls and it's corners. first one is reused for both first and last wall
	if (m_points.size() != m_walls.size()) return false;
/*	for (PointRefVector::const_iterator it = m_points.begin(); it != m_points.end(); ++it) {
		if (m_building->point((*it)->getValue()) == NULL) return false;
	}*/
	return true;
}

const Point * Building::point(unsigned offset) const {
	if (offset < m_points.size()) return m_points[offset];
	return NULL;
}

/** resistance K/W */
void Window::compute() {
	m_resistance = 1.0/(surface() * conductivity());
}

void WallType::compute(MaterialLibrary * library) {
	double resistance = 0;
	for (MaterialUsageVector::const_iterator it = m_composition.begin(); it != m_composition.end(); ++it) {
		std::string materialName = (*it)->material();
		const Material * material = library->material(materialName);
		if (material != NULL) {
			resistance += (*it)->width() / material->conductivity();
		}
	}
}

void Wall::compute(Room * room, const Point * start_vertex, const Point * end_vertex) {
	m_room = room;
	m_start_vertex = start_vertex;
	m_end_vertex = end_vertex;
	if (m_start_vertex == NULL || m_end_vertex == NULL) return;

	m_length = m_start_vertex->distance(m_end_vertex);

	double surface = m_length * room->height();

	for (WindowVector::iterator it = m_windows.begin(); it != m_windows.end(); ++it) {
		(*it)->compute();
		surface -= (*it)->surface();
	}

	printf("Wall surface is: %.5f m^2\n", surface);
}

void Room::compute(Building * building) {
	m_building = building;
	PointRefVector::const_iterator pit1, pit2;
	pit1 = m_points.begin();
	pit2 = pit1 + 1;
	for (WallVector::iterator it = m_walls.begin(); it != m_walls.end(); ++it) {
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
		const Point * this_point;
		const Point * next_point;
		const Point * prev_point;
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

Losses * Building::compute(MaterialLibrary * lib) {
	m_library = lib;
	for (RoomVector::iterator it = m_rooms.begin(); it != m_rooms.end(); ++it) {
		(*it)->compute(this);
	}
	return NULL;
}

MaterialLibrary * loadMaterialLibrary(std::string fileName) {
	MaterialLibrary * ml = new MaterialLibrary();
	if (parse(fileName, ml)) {
	} else {
		delete ml;
		ml = NULL;
	}
	return ml;
}

Building * loadBuilding(std::string fileName) {
	Building * b = new Building();
	if (parse(fileName, b)) {
	} else {
		printf("Error while parsing building\n");
		delete b;
		b = NULL;
	}
	return b;
}

