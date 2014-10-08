#ifndef __SRC_MODEL_H__
#define __SRC_MODEL_H__

#include <vector>
#include <string>
#include "json/json.h"

/** @file Data model used for describing computation model. */

/** Definition of material from which wall is composed */
class Material: public JSON::Struct {
public:
	Material(): json_name(&this->m_name), json_resistance(&this->m_resistance), json_density(&this->m_density) { addProperties(); }
	std::string name() const { return m_name; }
	float resistance() const { return m_resistance; }
	float density() const { return m_density; }

protected:
	void addProperties() {
		addProperty("name", &json_name); addProperty("resistance", &json_resistance); addProperty("density", &json_density);
	}

	JSON::Simple::String json_name;
	JSON::Simple::Float json_resistance;
	JSON::Simple::Float json_density;
	std::string m_name;
	float m_resistance;
	float m_density;
};

/** Ordered list of materials */
typedef std::vector<Material *> MaterialVector; 
typedef std::map<std::string, Material *> NameMaterialMap;

/** Usage of defined material. References material by it's name and defines width of layer of given material */
class MaterialUsage: public JSON::Struct {
public:
	MaterialUsage(): json_width(&this->m_width), json_material(&this->m_material) { addProperties(); }

protected:
	void addProperties() {
		addProperty("material", &json_material); 
		addProperty("width", &json_width);
	}

	JSON::Simple::Float json_width;
	JSON::Simple::String json_material;
	std::string m_material;
	float m_width;
};

/** Ordered list of material usages */
typedef std::vector<MaterialUsage *> MaterialUsageVector;

/** Top-level object covering whole material library. Contains list of available material definitions */
class MaterialLibrary: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(MaterialArrayProxy, MaterialLibrary, m_materials, Material);

	MaterialLibrary(): json_materials(this) { addProperties(); }
	bool validate();
	const Material * material(std::string & name) const;

protected:
	void addProperties() {
		addProperty("materials", &json_materials);
	}

	MaterialArrayProxy json_materials;
	MaterialVector m_materials;
	NameMaterialMap m_materialByName;

//	friend class MaterialLibraryProxy;
};

/** Definition of 2D vertex. Used for positioning of walls */
class Point: public JSON::Struct {
public:
	Point(): json_x(&this->m_x), json_y(&this->m_y) { addProperties(); }

	double distance(const Point * another) const;
	float x() const { return m_x; }
	float y() const { return m_y; }

protected:
	void addProperties() {
		addProperty("x", &json_x); addProperty("y", &json_y);
	}

	JSON::Simple::Float json_x;
	JSON::Simple::Float json_y;
	float m_x;
	float m_y;
};

/** Ordered list of verteices */
typedef std::vector<Point *> PointVector;

/** Definition of window. Window is (for now) defined by it's dimensions and thermal conductivity */
class Window: public JSON::Struct {
public:
	Window(): json_width(&this->m_width), json_height(&this->m_height), json_conductivity(&this->m_conductivity) { addProperties(); }
	void compute();
	double conductivity() const { return m_conductivity; }
	double resistance() const { return m_resistance; }
	double surface() const { return m_width * m_height; }

protected:
	void addProperties() {
		addProperty("width", &json_width); addProperty("height", &json_height); addProperty("conductivity", &json_conductivity);
	}

	JSON::Simple::Float json_width;
	JSON::Simple::Float json_height;
	JSON::Simple::Float json_conductivity;
	float m_width;
	float m_height;
	float m_conductivity;
	double m_resistance;
};

/** Ordered list of windows */
typedef std::vector<Window *> WindowVector;

class Building;
class Room;

/** Definition of wall. Currently only planar walls are supported. Wall is defined by it's composition, boundary vertices and list of windows */
class WallType: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(WallTypeCompositionProxy, WallType, m_composition, MaterialUsage);

	WallType(): json_composition(this), json_name(&m_name) { addProperties(); }
	void compute(MaterialLibrary * lib);

	double resistance() const { return m_resistance; }

protected:
	void addProperties() {
		addProperty("composition", &json_composition);
		addProperty("name", &json_name);
	}

	WallTypeCompositionProxy json_composition;
	JSON::Simple::String json_name;
	std::string m_name;

	MaterialUsageVector m_composition;

	double m_resistance;
};

typedef std::vector<WallType *> WallTypeVector;

class Wall: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(WallWindowProxy, Wall, m_windows, Window);

	Wall(): json_windows(this), json_walltype(&this->m_walltype), m_resistance(0), m_room(NULL) { addProperties(); }
	void compute(Room * room, const Point * start_vertex, const Point * end_vertex);
	double resistance() const { return m_resistance; }	
	Room * room() const { return m_room; }

protected:
	void addProperties() {
		addProperty("windows", &json_windows);
		addProperty("type", &json_walltype);
	}

	WallWindowProxy json_windows;
	WindowVector m_windows;

	JSON::Simple::String json_walltype;
	std::string m_walltype;

	const Point * m_start_vertex;
	const Point * m_end_vertex;

	double m_resistance;
	double m_length;

	Room * m_room;
};

/** Ordered list of wall definitions */
typedef std::vector<Wall *> WallVector;
typedef std::vector<JSON::Simple::Int *> PointRefVector;

/** Definition of room. Room is (for now) defined by list of it's walls and ceiling height. Only one level supported for now. */
class Room: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(RoomWallsProxy, Room, m_walls, Wall);
	JSON_ARRAY_PROXY(PointRefsProxy, Room, m_points, JSON::Simple::Int);
	
	Room(): json_walls(this), json_points(this), json_height(&this->m_height), m_building(NULL) { addProperties(); }
	bool validate();
	void compute(Building * building);
	Building * building() const { return m_building; }
	float height() const { return m_height; }

protected:
	void addProperties() {
		addProperty("walls", &json_walls); addProperty("height", &json_height); addProperty("corners", &json_points);
	}

	RoomWallsProxy json_walls;
	WallVector m_walls;

	PointRefsProxy json_points;
	PointRefVector m_points;

	JSON::Simple::Float json_height;
	float m_height;

	Building * m_building;
};

/** Ordered list of room definitions. */
typedef std::vector<Room *> RoomVector;

class Losses;

/** Top-level object covering definition of building. Building is defined by ordered list of vertices and ordered list of rooms. */
class Building: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(BuildingPointsProxy, Building, m_points, Point);
	JSON_ARRAY_PROXY(BuildingWallTypeProxy, Building, m_walltype, WallType);
	JSON_ARRAY_PROXY(BuildingRoomsProxy, Building, m_rooms, Room);

	Building(): json_points(this), json_rooms(this), json_walltype(this), m_library(NULL) { addProperties(); }
	bool validate();
	Losses * compute(MaterialLibrary * lib);
	const Point * point(unsigned offset) const;
	MaterialLibrary * library() const { return m_library; }

protected:
	void addProperties() {
		addProperty("points", &json_points); addProperty("rooms", &json_rooms); addProperty("walltypes", &json_walltype);
	}

	BuildingPointsProxy json_points;
	PointVector m_points;

	BuildingRoomsProxy json_rooms;
	RoomVector m_rooms;

	BuildingWallTypeProxy json_walltype;
	WallTypeVector m_walltype;

	MaterialLibrary * m_library;
};

MaterialLibrary * loadMaterialLibrary(std::string fileName);
Building * loadBuilding(std::string fileName);

#endif

