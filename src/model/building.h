#ifndef __SRC_MODEL_BUILDING_H__
#define __SRC_MODEL_BUILDING_H__

#include <vector>
#include <string>
#include <json/json.h>
#include <stdexcept>
#include "material.h"

namespace Model {

/** Definition of 2D vertex. Used for positioning of walls */

class Point: public JSON::Struct {
public:
	Point(): json_x(&this->m_x), json_y(&this->m_y) { addProperties(); }

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

/** Ordered list of vertices */
typedef std::vector<Point *> PointVector;

/** Definition of window. Window is (for now) defined by it's dimensions and thermal conductivity */
class Window: public JSON::Struct {
public:
	Window(): json_width(&this->m_width), json_height(&this->m_height), json_conductivity(&this->m_conductivity), json_name(&m_name) { addProperties(); }
	double conductivity() const { return m_conductivity; }
	double surface() const { return m_width * m_height; }
	const std::string & name() const { return m_name; }

protected:
	void addProperties() {
		addProperty("width", &json_width); addProperty("height", &json_height); addProperty("conductivity", &json_conductivity);
		addProperty("name", &json_name);
	}

	JSON::Simple::Float json_width;
	JSON::Simple::Float json_height;
	JSON::Simple::Float json_conductivity;
	JSON::Simple::String json_name;
	float m_width;
	float m_height;
	float m_conductivity;
	std::string m_name;
};

class WindowDef;

typedef std::vector<WindowDef *> WindowDefVector;

class WindowDef: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(WindowDefHoleProxy, WindowDef, m_holes, WindowDef);
	JSON_ARRAY_PROXY(WindowDefLayerProxy, WindowDef, m_layers, WindowDef);

	WindowDef(): json_name(&m_name), json_material(&m_material), json_width(&m_width), m_width(0), json_height(&m_height), m_height(0), json_depth(&m_depth), m_depth(0), json_holes(this), json_layers(this) { addProperties(); }
	virtual bool validate(std::string & message) const;

	WindowDef & hole(unsigned offset) const { if (offset < m_holes.size()) return *(m_holes[offset]); throw std::range_error("Invalid window hole"); }
	unsigned holes() const { return m_holes.size(); }

	WindowDef & layer(unsigned offset) const { if (offset < m_layers.size()) return *(m_layers[offset]); throw std::range_error("Invalid window layer"); }
	unsigned layers() const { return m_layers.size(); }

	std::string name() const { return m_name; }

	float width() const { return m_width; }
	float height() const { return m_height; }
	float depth() const { return m_depth; }
	const std::string & material() const { return m_material; } 

protected:
	void addProperties() {
		addProperty("name", &json_name); addProperty("material", &json_material); addProperty("depth", &json_depth), addProperty("width", &json_width); addProperty("height", &json_height); addProperty("holes", &json_holes); addProperty("layers", &json_layers);
	}

	JSON::Simple::String json_name;
	std::string m_name;

	JSON::Simple::String json_material;
	std::string m_material;

	JSON::Simple::Float json_width;
	float m_width;

	JSON::Simple::Float json_height;
	float m_height;

	JSON::Simple::Float json_depth;
	float m_depth;

	WindowDefHoleProxy json_holes;
	WindowDefVector m_holes;

	WindowDefLayerProxy json_layers;
	WindowDefVector m_layers;
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

//	double resistance() const { return m_resistance; }

	const std::string & name() const { return m_name; }

	const MaterialUsage & composition(unsigned layer) const { if (layer < m_composition.size()) return *(m_composition[layer]); throw std::range_error("Invalid layer"); }
	unsigned compositions() const { return m_composition.size(); }

protected:
	void addProperties() {
		addProperty("composition", &json_composition);
		addProperty("name", &json_name);
	}

	WallTypeCompositionProxy json_composition;
	JSON::Simple::String json_name;
	std::string m_name;

	MaterialUsageVector m_composition;

//	double m_resistance;
};

typedef std::vector<WallType *> WallTypeVector;

class Wall: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(WallWindowProxy, Wall, m_windows, Window);

	Wall(): json_windows(this), json_walltype(&this->m_walltype), m_room(NULL) { addProperties(); }
//	double losses(double deltaTemp);
//	Room * room() const { return m_room; }

	const Window & window(unsigned offset) const { if (offset < m_windows.size()) return *(m_windows[offset]); throw std::range_error("Invalid window"); }
	unsigned windows() const { return m_windows.size(); }

	const std::string & wallType() const { return m_walltype; }

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
	
	Room(): json_walls(this), json_points(this), json_height(&this->m_height), m_height(0), json_internalTemperature(&this->m_internalTemperature), m_internalTemperature(-1), json_level(&this->m_level), json_name(&this->m_name), m_level(0), m_building(NULL), m_id(0) { addProperties(); }
	virtual bool validate(std::string & message) const;
//	Building * building() const { return m_building; }
	float height() const { return m_height; }

	unsigned point(unsigned offset) const { if (offset < m_points.size()) return m_points[offset]->getValue(); throw std::range_error("Invalid point"); }
	unsigned points() const { return m_points.size(); }

	const Wall & wall(unsigned offset) const { if (offset < m_walls.size()) return *(m_walls[offset]); throw std::range_error("Invalid wall"); }
	unsigned walls() const { return m_walls.size(); }

	unsigned level() const { return m_level; }

	const std::string & name() const { return m_name; }

	float internalTemperature() const { return m_internalTemperature; }

	unsigned id() const { return m_id; }
	void id(unsigned id) { m_id = id; }

protected:
	void addProperties() {
		addProperty("walls", &json_walls); addProperty("height", &json_height); addProperty("corners", &json_points); addProperty("level", &json_level); addProperty("temperature", &json_internalTemperature); addProperty("name", &json_name);
	}

	RoomWallsProxy json_walls;
	WallVector m_walls;

	PointRefsProxy json_points;
	PointRefVector m_points;

	JSON::Simple::Float json_height;
	float m_height;

	JSON::Simple::Float json_internalTemperature;
	float m_internalTemperature;

	JSON::Simple::Int json_level;
	int m_level;

	JSON::Simple::String json_name;
	std::string m_name;

	Building * m_building;

	unsigned m_id;
};

/** Ordered list of room definitions. */
typedef std::vector<Room *> RoomVector;

typedef std::vector<JSON::Simple::String *> WallTypeRefVector;

class Losses;

/** Top-level object covering definition of building. Building is defined by ordered list of vertices and ordered list of rooms. */
class Building: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(BuildingPointsProxy, Building, m_points, Point);
	JSON_ARRAY_PROXY(BuildingWallTypeProxy, Building, m_walltype, WallType);
	JSON_ARRAY_PROXY(BuildingRoomsProxy, Building, m_rooms, Room);
	JSON_ARRAY_PROXY(BuildingFloorsProxy, Building, m_floors, JSON::Simple::String);
	JSON_ARRAY_PROXY(BuildingWindowDefProxy, Building, m_winDefs, WindowDef);

	Building(): json_points(this), json_rooms(this), json_walltype(this), json_floors(this), json_winDefs(this) { addProperties(); }
	virtual bool validate(std::string & message) const;

	const Point & point(unsigned offset) const { if (offset < m_points.size()) return *(m_points[offset]); throw std::range_error("Invalid point"); }
	unsigned points() const { return m_points.size(); }

	const Room & room(unsigned offset) const { if (offset < m_rooms.size()) return *(m_rooms[offset]); throw std::range_error("Invalid room"); }
	unsigned rooms() const { return m_rooms.size(); }

	const WallType & wallType(unsigned offset) const { if (offset < m_walltype.size()) return *(m_walltype[offset]); throw std::range_error("Invalid wall type"); }
	unsigned wallTypes() const { return m_walltype.size(); }

	const std::string & floor(unsigned offset) const { if (offset < m_floors.size()) return m_floors[offset]->getValue(); throw std::range_error("Invalid floor/ceiling type"); }

	const WindowDef & windowDef(unsigned offset) const { if (offset < m_winDefs.size()) return *(m_winDefs[offset]); throw std::range_error("Invalid window definition"); }
	unsigned windowDefs() const { return m_winDefs.size(); }

	unsigned floors() const { return m_floors.size(); }

protected:
	void addProperties() {
		addProperty("points", &json_points); addProperty("rooms", &json_rooms); addProperty("walltypes", &json_walltype); addProperty("floors", &json_floors);
		addProperty("windows", &json_winDefs);
	}

	BuildingPointsProxy json_points;
	PointVector m_points;

	BuildingRoomsProxy json_rooms;
	RoomVector m_rooms;

	BuildingWallTypeProxy json_walltype;
	WallTypeVector m_walltype;

	BuildingFloorsProxy json_floors;
	WallTypeRefVector m_floors;

	BuildingWindowDefProxy json_winDefs;
	WindowDefVector m_winDefs;
};

Building * loadBuilding(std::string fileName);

}

#endif

