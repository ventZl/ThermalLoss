#ifndef __SRC_MODEL_BUILDING_H__
#define __SRC_MODEL_BUILDING_H__

#include <vector>
#include <json/json.h>
#include <stdexcept>
#include "material.h"

namespace Model {

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

/** Ordered list of vertices */
typedef std::vector<Point *> PointVector;

/** Definition of window. Window is (for now) defined by it's dimensions and thermal conductivity */
class Window: public JSON::Struct {
public:
	Window(): json_width(&this->m_width), json_height(&this->m_height), json_conductivity(&this->m_conductivity) { addProperties(); }
	double conductivity() const { return m_conductivity; }
	double width() const { return m_width; }
   	double height() const { return m_height; }

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

//	double resistance() const { return m_resistance; }

	const std::string & name() const { return m_name; }

	const MaterialUsage & composition(unsigned layer) const { if (layer < m_composition.size()) return *m_composition[layer]; throw std::runtime_error("Layer out of interval"); }
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

	const Window & window(unsigned offset) const { if (offset < m_windows.size()) return *m_windows[offset]; throw std::runtime_error("Offset out of range"); }
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
	
	Room(): json_walls(this), json_points(this), json_height(&this->m_height), m_building(NULL) { addProperties(); }
	bool validate();
//	Building * building() const { return m_building; }
	float height() const { return m_height; }

	unsigned point(unsigned offset) const { if (offset < m_points.size()) return m_points[offset]->getValue(); throw std::runtime_error("Offset out of range"); }
	unsigned points() const { return m_points.size(); }

	const Wall & wall(unsigned offset) const { if (offset < m_walls.size()) return *m_walls[offset]; throw std::runtime_error("Offset out of range"); }
	unsigned walls() const { return m_walls.size(); }

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

	Building(): json_points(this), json_rooms(this), json_walltype(this) { addProperties(); }
	bool validate();

	const Point & point(unsigned offset) const;
	unsigned points() const { return m_points.size(); }

	const Room & room(unsigned offset) const;
	unsigned rooms() const { return m_rooms.size(); }

	const WallType & wallType(unsigned offset) const;
	unsigned wallTypes() const { return m_walltype.size(); }

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
};

Building * loadBuilding(std::string fileName);

}

#endif

