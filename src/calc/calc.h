#ifndef __CALC_H___
#define __CALC_H___

#include <set>
#include <vector>
#include <map>
#include <string>
#include <../include/property.h>

namespace Model {
	class Building;
	class Room;
	class Window;
	class Wall;
	class Parameters;
	class Losses;
	class MaterialLibrary;
	class Wall;
	class WallType;
	class Point;
	class WindowDef;
}

namespace Geometry {
	class Polygon;
	class Vertex2D;
	class Vertex3D;
}

namespace Calc {

struct MaterialUsage {
	MaterialUsage(std::string name, double depth): name(name), depth(depth) {}
	std::string name;
	double depth;
};

class WallType {
public:
	WallType(const std::string & name): m_name(name) {}
	void addLayer(const std::string & name, double depth);
	double resistance(const Model::MaterialLibrary & materials) const;

	RO_PROPERTY(std::string, name);

protected:
	std::vector<MaterialUsage> m_layers;
};

class Edge {
public:
	Edge(unsigned start_edge, unsigned end_edge): start_edge(start_edge), end_edge(end_edge) {}
	bool operator==(const Edge & other) { if (this->start_edge == other.start_edge && this->end_edge == other.end_edge) return true; return false; }
	Edge opposite() const { return Edge(end_edge, start_edge); }

protected:
	unsigned start_edge;
	unsigned end_edge;
};

class Window;
class Wall;
class Room;

class Wall {
public:
	Wall(const Room * room): m_room(room), m_otherRoom(NULL) {}

	RO_PROPERTY(const Room *, room);
	PROPERTY(double, resistance);
	PROPERTY(double, area);
	RO_PROPERTY(Geometry::Vertex3D *, startVertex);
	RO_PROPERTY(Geometry::Vertex3D *, endVertex);
	PROPERTY(const Room *, otherRoom);
	PROPERTY(WallType *, wallType);
	RO_PROPERTY(double, length);

public:
	void append(Window * window) { m_windows.push_back(window); }
	void vertices(Geometry::Vertex3D * startVertex, Geometry::Vertex3D * endVertex);
	bool isOpposite(const Wall & other) const;
	/** This may potentially lead to nasty errors... */
	bool operator==(const Wall & other) const;
	void calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const;

protected:
	std::vector<Window *> m_windows;
};

class Calculation;

class Room {
public:
	Room(const Calculation * calc);
	virtual ~Room();
	PROPERTY(double, height);
	PROPERTY(WallType *, floorType);
	PROPERTY(WallType *, ceilingType);
	PROPERTY(bool, bottomMost);
	PROPERTY(bool, topMost);
	PROPERTY(unsigned, level);
	PROPERTY(double, roomTemp);
	RO_PROPERTY(const Calculation *, calc);

public:
	void append(Wall * wall) { m_walls.push_back(wall); }
	virtual void calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const;
	double area() const;
	void addVertex(Geometry::Vertex3D * vertex);
	double calcTemp(const Model::Parameters & parameters) const;

protected:
	std::vector<Wall *> m_walls;
	Geometry::Polygon * m_polygon;
};

class Window {
public:
	Window(const Wall * wall): m_wall(wall) {}
	virtual ~Window() {}
	RO_PROPERTY(const Wall *, wall);

public:
	virtual void calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const = 0;
	virtual double area() const = 0;
};

class WindowBySpecs: public Window {
public:
	WindowBySpecs(const Wall * wall): Window(wall) {}
	virtual ~WindowBySpecs() {}
	PROPERTY(double, resistance);
	PROPERTY(double, area);

public:
	virtual void calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const;
};

class WindowByDef: public Window {
public:
	WindowByDef(const Wall * wall): Window(wall) {}
	virtual ~WindowByDef() {}
	PROPERTY(std::string, name);

public:
	virtual double area() const;
	virtual void calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses) const;
};

class WindowDef;

typedef std::vector<WindowDef *> WindowDefVector;

class WindowDef {
public:
	WindowDef(): m_width(0), m_height(0) {}

	RO_PROPERTY(double, resistance);	// conductivity of layer of defined  width and defined material type 
	RO_PROPERTY(double, area);
	PROPERTY(double, width);
	PROPERTY(double, height);
	PROPERTY(double, depth);
	PROPERTY(std::string, material);
	PROPERTY(std::string, name);

public:
	void calculate(const Model::MaterialLibrary & materials, const Model::Parameters & parameters, Model::Losses & losses);
	bool isSizeSet() const { if (m_width != 0 && m_height != 0) return true; return false; }
	void addHole(WindowDef * winDef) { m_holes.push_back(winDef); }
	void addLayer(WindowDef * winDef) { m_layers.push_back(winDef); }

protected:
	WindowDefVector m_layers;
	WindowDefVector m_holes;

};

typedef std::vector<Wall *> Walls;
typedef std::vector<Room *> Rooms;
typedef std::vector<Window *> Windows;
typedef std::vector<Geometry::Vertex3D *> Vertices;
typedef std::map<std::string, WallType *> WallTypes;
typedef std::map<std::string, WindowDef *> WindowDefs;

class Calculation {
	public:
		Calculation(): m_maxLevel(0) {}

		bool load(Model::Building & building);
		Model::Losses * calculate(Model::Parameters & parameters, Model::MaterialLibrary & materials);

	protected:
		Wall * collectWall(const Calc::Room * room, const Model::Wall & wall);
		Room * collectRoom(const Calculation * calc, const Model::Room & room);
		Window * collectWindow(const Calc::Wall * wall, const Model::Window & window);
		Geometry::Vertex3D * collectVertex(const Model::Point & point);
		WindowDef * collectWindowDef(const Model::WindowDef & winDef);
		WallType * collectWallType(const Model::WallType & wallType);
		WallType * wallType(const std::string & name) const;

	public:
		WindowDef * windowDef(const std::string & name) const;

	protected:
		Walls m_walls;
		Rooms m_rooms;
		Windows m_windows;
		Vertices m_vertices;
		WallTypes m_wallTypes;
		std::vector<std::string> m_floors;
		WindowDefs m_windowDefs;
		
		unsigned m_maxLevel;
};

}

#endif

