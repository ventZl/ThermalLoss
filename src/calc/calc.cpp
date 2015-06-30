#include <math.h>
#include <cstdio>
#include "calc.h"
#include <model/building.h>
#include <model/losses.h>
#include <geom/vertex.h>
#include <geom/polygon.h>

void Calc::Wall::calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const {
//	printf("\n\nWall thermal report\n===================\n");
	double surface = room()->height() * this->length();
	//	printf("Room of this wall is %p\n", room());
//	printf("Wall height of %.2fm and length of %.2fm has surface of %.2fm^2\n", room()->height(), length(), surface);
	double resistance = m_wallType->resistance(materials) + (0.13 * 1); // 0.13 is resistance of thin air layer near wall
//	printf("Wall construction resistance is %.3f m^2.K/W\n", resistance);
	
	double windowSurface = 0;
	for (unsigned q = 0; q < m_windows.size(); ++q) {
		windowSurface += m_windows[q]->area();
	}

	if (windowSurface > surface) {
		fprintf(stderr, "Cumulative window surface of certain wall is greater than surface of wall itself! Something is wrong with building definition!\n"); 
	} else {
		surface -= windowSurface;
	}

	/* for now ignore thermal transfer between two internal rooms */
	if (otherRoom() != NULL) {
//		fprintf(stderr, "Ignoring thermal transfer between internal rooms for now\n");
		return;
	}

	double flow = (room()->calcTemp(parameters) - parameters.outTemp()) / resistance;
//	printf("Thermal flow is %.3f W/m^2 (for temp difference of %.2f deg. C)\n", flow, (room()->calcTemp(parameters) - parameters.outTemp()));
	double power = flow * surface;
//	printf("Thermal dissipation power is %.3f W\n", power);

//	printf("Adding wall loss of %.4f (%f, %f, %f)\n", power, flow, resistance, surface);
	losses.addWallLoss(0, power);
	losses.addRelativeLoss(surface / resistance);
	return;
}

Calc::Room::Room(const Calc::Calculation * calc): m_bottomMost(true), m_topMost(true), m_calc(calc), m_polygon(new Geometry::Polygon()) {
}

Calc::Room::~Room() {
	delete m_polygon;
}

/* silently assumes that z() of all vertices is on the same level */
double Calc::Room::area() const {
	return m_polygon->area();
}

double Calc::Room::calcTemp(const Model::Parameters & parameters) const {
	if (m_roomTemp == -1)
		return parameters.roomTemp();
	if (m_roomTemp < parameters.groundTemp() && m_level == 0)
		return parameters.groundTemp();
	return m_roomTemp;
}

void Calc::Room::calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const {
//	printf("\nRoom thermal report\n===================\n");
	double area = this->area();
//	printf("Room area is %.2f m^2\n", area);
	/* Currently we ignore intra-level heat transfer */
	if (m_bottomMost) {
		double floorResistance = m_floorType->resistance(materials) + 0.2; // 0.2 is resistance of air layer near floor
//		printf("Floor resistance is %.3f m^2.K/W\n", floorResistance);
		double flow = (calcTemp(parameters) - parameters.groundTemp()) / floorResistance;
//		printf("Thermal difference is %.2f deg. C (%.2f, %.2f)\n", calcTemp(parameters) - parameters.groundTemp(), calcTemp(parameters), parameters.groundTemp());
		double power = area * flow;
		losses.addFloorLoss(0, power);
		losses.addRelativeLoss(area / floorResistance);
	}
	if (m_topMost) {
		double ceilingResistance = m_ceilingType->resistance(materials) + 0.1; // 0.1 is resistance of air layer near ceiling
//		printf("Ceiling resistance is %.3f m^2.K/W\n", ceilingResistance);
		double flow = (calcTemp(parameters) - parameters.outTemp()) / ceilingResistance;
//		printf("Thermal difference is %.2f deg. C\n", calcTemp(parameters) - parameters.outTemp());
		double power = area * flow;
		losses.addCeilLoss(0, power);
		losses.addRelativeLoss(area / ceilingResistance);
	}
//	printf("adding loss area of %f\n", area);
	losses.addArea(area);
	return;
}

void Calc::Room::addVertex(Geometry::Vertex3D * vertex) {
	m_polygon->addVertex(*vertex); 
}


void Calc::WindowBySpecs::calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const {
	double flow = (wall()->room()->calcTemp(parameters) - parameters.outTemp()) / m_resistance;
	double power = m_area * flow;
	losses.addWindowLoss(0, power);
	losses.addRelativeLoss(m_area / m_resistance);
	return;
}

void Calc::WindowByDef::calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const {
	Calc::WindowDef * winDef = wall()->room()->calc()->windowDef(m_name);
	double flow = (wall()->room()->calcTemp(parameters) - parameters.outTemp()) / winDef->resistance();
	double power = winDef->area() * flow;
	losses.addWindowLoss(0, power);
	losses.addRelativeLoss(winDef->area() / winDef->resistance());
	return;
}

double Calc::WindowByDef::area() const {
	Calc::WindowDef * winDef = wall()->room()->calc()->windowDef(m_name);
	return winDef->area();
}

void Calc::WindowDef::calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) {
	if (!m_layers.empty()) {
		double m_resistance = 0;
		m_area = m_width * m_height;
		for (unsigned q = 0; q < m_layers.size(); q++) {
			if (m_layers[q]->isSizeSet()) {
				fprintf(stderr, "Layer has dimensions set (%.2fx%.2f vs %.2fx%.2f) albeit it should not have any!\n", m_layers[q]->width(), m_layers[q]->height(), width(), height());
				exit(1);
			}
			m_layers[q]->width(width());
			m_layers[q]->height(height());
			m_resistance += m_layers[q]->resistance();
		}
		if (m_name != "") printf("Window definition resistance is %.3f m^2.K/W\n", m_resistance);
	} else {
		if (m_width <= 0.0 || m_height <= 0.0 ) {
			fprintf(stderr, "Window definition contains no size definition. This should not happen!");
			abort();
		}
		m_area = m_width * m_height;
		double compute_area = m_area;
		double sum_resistance = 0;
		double sum_area = 0;
		for (unsigned q = 0; q < m_holes.size(); q++) {
			if (!m_holes[q]->isSizeSet()) {
				fprintf(stderr, "Hole does not contain any size definition!");
				abort();
			}
			m_holes[q]->calculate(materials, parameters, losses);
			compute_area -= m_holes[q]->area();
			sum_area += m_holes[q]->area();
			sum_resistance += m_holes[q]->area() * m_holes[q]->resistance();
		}
		const Model::Material * material = materials.material(m_material);
		if (material == NULL) {
			printf("Material '%s' is not defined!\n", m_material.c_str());
		}
		if (compute_area <= 0.0) {
			fprintf(stderr, "Size of holes is greater than size of window itself. Wrong window definition!\n");
			exit(1);
		}
		sum_area += compute_area;
		sum_resistance += compute_area * (m_depth / material->conductivity());
		m_resistance = sum_resistance / sum_area;
		m_resistance += 1 * 0.13;
//		if (m_name != "") printf("Window definition '%s' U = %.3f W/m^2.K\n", m_name.c_str(), 1.0 / m_resistance);

	} 
}

double Calc::WallType::resistance(const Model::MaterialLibrary & materials) const {
	double resistance = 0;
//	printf("## Resistance report: ");
	for (unsigned q = 0; q < m_layers.size(); ++q) {
		const Calc::MaterialUsage & layer = m_layers[q];
		const Model::Material * material = materials.material(layer.name);
		if (material != NULL) {
//			printf("(%.3f W/m.K @ %.3f m) ", material->conductivity(), layer.depth);
			resistance += layer.depth / material->conductivity();
		} else {
			fprintf(stderr, "Material '%s' undefined. Ignoring layer!\n", layer.name.c_str());
			abort();
		}
	}
//	printf("\n");
	return resistance;
}

void Calc::WallType::addLayer(const std::string & name, double depth) {
	m_layers.push_back(Calc::MaterialUsage(name, depth));
	return;
}

bool Calc::Calculation::load(Model::Building & building) {
	for (unsigned q = 0; q < building.points(); ++q) {
		m_vertices.push_back(collectVertex(building.point(q)));
	}

	for (unsigned q = 0; q < building.windowDefs(); ++q) {
		Calc::WindowDef * winDef = collectWindowDef(building.windowDef(q));
		m_windowDefs.insert(Calc::WindowDefs::value_type(winDef->name(), winDef));
	}

	for (unsigned q = 0; q < building.floors(); ++q) {
		m_floors.push_back(building.floor(q));
	}

	for (unsigned q = 0; q < building.wallTypes(); ++q) {
		Calc::WallType * type = collectWallType(building.wallType(q));
		m_wallTypes.insert(Calc::WallTypes::value_type(type->name(), type));
	}

	for (unsigned q = 0; q < building.rooms(); ++q) {
		m_rooms.push_back(collectRoom(this, building.room(q)));
	}

	for (unsigned q = 0; q < m_rooms.size(); ++q) {
		if (m_rooms[q]->level() < m_maxLevel) m_rooms[q]->topMost(false);
	}

	return true;
}

Calc::WallType * Calc::Calculation::collectWallType(const Model::WallType & wallType) {
	Calc::WallType * type = new Calc::WallType(wallType.name());
	for (unsigned q = 0; q < wallType.compositions(); ++q) {
		const Model::MaterialUsage & layerDef = wallType.composition(q);
		type->addLayer(layerDef.material(), layerDef.width());
	}
	return type;
}

Geometry::Vertex3D * Calc::Calculation::collectVertex(const Model::Point & point) {
	Geometry::Vertex3D * vertex = new Geometry::Vertex3D(point.x(), point.y(), 0);
	return vertex;
}

Calc::WindowDef * Calc::Calculation::collectWindowDef(const Model::WindowDef & winDef) {
	Calc::WindowDef * windowDef = new Calc::WindowDef();
	for (unsigned q = 0; q < winDef.holes(); q++) {
		windowDef->addHole(collectWindowDef(winDef.hole(q)));
	}
	for (unsigned q = 0; q < winDef.layers(); q++) {
		windowDef->addLayer(collectWindowDef(winDef.layer(q)));
	}
	windowDef->width(winDef.width());
	windowDef->height(winDef.height());
	windowDef->depth(winDef.depth());
	windowDef->material(winDef.material());
	windowDef->name(winDef.name());
	return windowDef;	
}

void Calc::Wall::vertices(Geometry::Vertex3D * startVertex, Geometry::Vertex3D * endVertex)  {
	m_startVertex = startVertex; 
	m_endVertex = endVertex; 
	m_length = m_startVertex->distance(*m_endVertex); 
	return;
}

bool Calc::Wall::isOpposite(const Wall & other) const { 
	if ((*startVertex()) == (*other.endVertex()) && (*endVertex()) == (*other.startVertex())) return true; 
	return false; 
}

bool Calc::Wall::operator==(const Wall & other) const { 
	if ((*startVertex()) == (*other.startVertex()) && (*endVertex()) == (*other.endVertex())) return true; 
	return false; 
}

Calc::Room * Calc::Calculation::collectRoom(const Calc::Calculation * calc, const Model::Room & room) {
	Calc::Room * room_ = new Calc::Room(calc);
	room_->height(room.height());
	for (unsigned q = 0; q < room.walls(); ++q) {
		Calc::Wall * wall = collectWall(room_, room.wall(q));
		room_->addVertex(m_vertices[room.point(q)]);
		wall->vertices(m_vertices[room.point(q)], m_vertices[room.point((q+1) % room.points())]);
		room_->append(wall);
	}
	/* Bricks will be shat... */
	if (room.level() >= m_floors.size()) {
		fprintf(stderr, "Level of this room is too high compared to numbers of defined floors!\n");
		abort();
	}
	room_->floorType(wallType(m_floors[room.level()]));
	room_->ceilingType(wallType(m_floors[room.level() + 1]));
	room_->roomTemp(room.internalTemperature());
//	printf("Room internal temperature is %.2f deg. C\n", room_->roomTemp());
	if (room.level() > 0) room_->bottomMost(false);
	if (m_maxLevel < room.level()) m_maxLevel = room.level();
	return room_;
}

Calc::Window * Calc::Calculation::collectWindow(const Calc::Wall * wall, const Model::Window & window) {
	if (window.name() == "") {
		Calc::WindowBySpecs * win = new Calc::WindowBySpecs(wall);
		win->resistance(1.0/window.conductivity());
		win->area(window.surface());
		m_windows.push_back(win);
		return win;
	} else {
		Calc::WindowByDef * win = new Calc::WindowByDef(wall);
		win->name(window.name());
		return win;
	}
}

Calc::Wall * Calc::Calculation::collectWall(const Calc::Room * room, const Model::Wall & wall) {
	Calc::Wall * wall_ = new Calc::Wall(room);
	wall_->wallType(wallType(wall.wallType()));
	for (unsigned q = 0; q < wall.windows(); ++q) {
		Calc::Window * window = collectWindow(wall_, wall.window(q));
		wall_->append(window);
		m_windows.push_back(window);
	}
	m_walls.push_back(wall_);
	return wall_;
}

Calc::WallType * Calc::Calculation::wallType(const std::string & name) const {
	Calc::WallTypes::const_iterator type_it = m_wallTypes.find(name);
	if (type_it != m_wallTypes.end()) {
		return type_it->second;
	} else {
		fprintf(stderr, "Unknown wall type '%s' employed!\n", name.c_str());
		return NULL;
	}
}

Calc::WindowDef * Calc::Calculation::windowDef(const std::string & name) const {
	Calc::WindowDefs::const_iterator it = m_windowDefs.find(name);
	if (it != m_windowDefs.end()) {
		return it->second;
	} else {
		fprintf(stderr, "Unknown window definition '%s' employed!\n", name.c_str());
		return NULL;
	}
}

Model::Losses * Calc::Calculation::calculate(Model::Parameters & parameters, Model::MaterialLibrary & materials) {
	
	Model::Losses * outLosses = new Model::Losses();
	for (Calc::WindowDefs::iterator it = m_windowDefs.begin(); it != m_windowDefs.end(); ++it) {
		it->second->calculate(materials, parameters, *outLosses);
	}

	// pair other sides of same walls together
	for (unsigned q = 0; q < m_walls.size(); q++) {
		for (unsigned w = q+1; w < m_walls.size(); w++) {
			if ((*m_walls[q]).isOpposite(*(m_walls[w]))) {
//				fprintf(stderr, "Wall %d is opposite side of wall %d!\n", q, w);
				m_walls[q]->otherRoom(m_walls[w]->room());
				m_walls[w]->otherRoom(m_walls[q]->room());
			}
		}
	}

	for (unsigned q = 0; q < m_walls.size(); q++) {
		m_walls[q]->calculate(materials, parameters, *outLosses);
	}

	for (unsigned q = 0; q < m_rooms.size(); q++) {
		m_rooms[q]->calculate(materials, parameters, *outLosses);
	}

	for (unsigned q = 0; q < m_windows.size(); q++) {
//		printf("Calculating window %d\n", q);
		m_windows[q]->calculate(materials, parameters, *outLosses);
	}

	outLosses->energyRequired(outLosses->relativeLosses() * 100000 / outLosses->area());

	return outLosses;
}
