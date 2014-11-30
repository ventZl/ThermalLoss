#ifndef __CALC_H___
#define __CALC_H___

#include <set>
#include <vector>

class Building;
class Parameters;
class Losses;

struct Vertex3D {
	Vertex3D(double x, double y, double z): m_x(x), m_y(y), m_z(z) {}
	double m_x, m_y, m_z;
};

class Wall {
	std::vector<Vertex3D> m_vertices;
	double m_resistance;
};

class Room {
	
};

class TmpWall {
	std::vector<unsigned> m_vertices;
	double m_resistance;
};

typedef std::set<Wall> Walls;
typedef std::set<Room> Rooms;

class Calculation {
	public:
		Calculation(Building * building): m_building(building) {}

		Losses * calculate(Parameters * parameters);

	protected:

		bool analyzeBuilding();
		Walls & collectWalls();
		Rooms & collectRooms();

	protected:
		Building * m_building;
		Walls m_walls;
};

#endif

