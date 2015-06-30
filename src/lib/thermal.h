#ifndef _LIB_THERMAL_H__
#define _LIB_THERMAL_H__

#include <vector>

namespace Solver {
	class System;
}

namespace Thermal {

class Path;

// Interface for purpose of representing heat amount
// in certain region of space 
class Cell {
public:
	Cell(unsigned key): m_key(key) {};
	virtual ~Cell();

	// Get representation of temperature of cell (value in Kelvins)
	virtual double temperature(double energy) const = 0;
	virtual double energy(double temperature) const = 0;
	const std::vector<Path *> paths() const { return m_paths; }
	unsigned key() const { return m_key; }

protected:
	void addPath(Path * path) { m_paths.push_back(path); }
	void removePath(Path * path);

	std::vector<Path *> m_paths;
	unsigned m_key;

	friend class Path;
};

class Path {
public:
	// Create heat transport path from cell1 to cell2
	Path(unsigned key, Cell * cell1, Cell * cell2): m_key(key), m_cell1(cell1), m_cell2(cell2) { m_cell1->addPath(this); m_cell2->addPath(this); }
	virtual ~Path() { m_cell1->removePath(this); m_cell2->removePath(this); }

	// Transport given amount of heat between connected cells
	virtual double transport(Solver::System * system, double timeslice) = 0;
	unsigned key() const { return m_key; }

	Cell * cell1() { return m_cell1; }
	Cell * cell2() { return m_cell2; }

protected:
	double m_key;
	Cell * m_cell1, * m_cell2;
};

class Mass: public Cell {
public:
	Mass(unsigned key, double volume, double density, double capacity): Cell(key), m_volume(volume), m_density(volume), m_capacity(capacity) {}

	virtual double temperature(double energy) const;
	virtual double energy(double temperature) const;

protected:
	double m_volume;
	double m_density;
	double m_capacity;
};

class Barrier: public Path {
public:
	Barrier(unsigned key, double surface, double width, double conductivity, Cell * cell1, Cell * cell2): Path(key, cell1, cell2), m_surface(surface), m_width(width) {}

	/* Return true if rate of change of thermal flow is within limits */
	virtual double transport(Solver::System * system, double timeslice);

protected:
	double m_surface;
	double m_width;
	double m_conductivity;
};

#define AIR_DENSITY (1.2*0.001)
#define AIR_CAPACITY 1.005

class Room: public Mass {
public:
	Room(unsigned key, double width, double depth, double height): Mass(key, width * depth * height, AIR_DENSITY, AIR_CAPACITY) {}
};

}

#endif

