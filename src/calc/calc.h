#ifndef __CALC_H___
#define __CALC_H___

#include <set>
#include <vector>

namespace Model {
	class Building;
	class Room;
	class Window;
	class Parameters;
	class Losses;
}

namespace Calc {

struct Vertex3D {
	Vertex3D(double x, double y, double z): m_x(x), m_y(y), m_z(z) {}
	double m_x, m_y, m_z;
};

class Edge {
public:
	Edge(unsigned start_edge, unsigned end_edge): start_edge(start_edge), end_edge(end_edge) {}
	bool operator=(const Edge & other) { if (this.start_edge == other.start_edge && this.end_edge == other.end_edge) return true; return false; }
	Edge opposite() const { return Edge(end_edge, start_edge); }

protected:
	unsigned start_edge;
	unsigned end_edge;
}

class Wall {
	/** Temporary wall record.
	 * Used for intermediate loading phase of wall processing
	 */
	struct Temp {
		std::vector<unsigned> m_vertices;
		double m_resistance;
	};

	std::vector<Vertex3D> m_vertices;
	double m_resistance;
};

class Room {
	std::vector<Room> m_rooms;
	double m_height;
	double m_area;
};

typedef std::set<Wall> Walls;
typedef std::set<Room> Rooms;

class Calculation {
	public:
		Calculation() {}

		bool load(Model::Building & building);
		Model::Losses calculate(Model::Parameters & parameters, Model::MateralLibrary & materials);

	protected:
		void collectWalls(const Model::Room & room);
		void collectRooms(const Model::Building & building);
		void collectWindows(const Model::Wall & wall);

	protected:
		Walls m_walls;
		Rooms m_rooms;
		Windows m_windows;
};

}

#endif

