#ifndef _LIB_THERMAL_H__
#define _LIB_THERMAL_H__

#include <vector>

namespace Thermal {

class Path;

// Interface for purpose of representing heat amount
// in certain region of space 
class Cell {
public:
	virtual ~Cell() {}

	// Get representation of temperature of cell (value in Kelvins)
	virtual double temperature(double energy) const = 0;
	const std::vector<Path &> paths() const { return m_paths; }

protected:
	void addPath(Path & path) { m_paths.push_back(path); }
	void removePath(Path & path) { std::vector<Path &>::iterator it = m_paths.find(path); if (it != m_paths.end()) m_paths.erase(it); }

	std::vector<Path &> m_paths;

	friend class Path;
};

class Path {
public:
	// Create heat transport path from cell1 to cell2
	Path(Cell & cell1, Cell & cell2): m_cell1(cell1), m_cell2(cell2) { m_cell1.addPath(this); m_cell2.addPath(this); }
	virtual ~Path() { m_cell1.removePath(this); m_cell2.removePath(this); }

	// Transport given amount of heat between connected cells
	virtual double transport(double timeslice) = 0;

protected:
	Cell & cell1() { return m_cell1; }
	Cell & cell2() { return m_cell2; }

protected:
	Cell & m_cell1, & m_cell2;
};

class Mass: public Cell {
public:
	Mass(double volume, double density double capacity): m_volume(volume), m_density(volume), m_capacity(capacity) {}

	virtual double temperature(double energy) const;

protected:
	double m_volume;
	double m_density;
	double m_capacity;
};

class Barrier: public Path {
public:
	Barrier(double surface, double width, double conductivity, Cell & cell1, Cell & cell2): Path(cell1, cell2), m_surface(surface), m_width(width) {}

	/* Return true if rate of change of thermal flow is within limits */
	virtual double transport(double timeslice);

protected:
	double m_surface;
	double m_width;
	double m_conductivity;
};

class Room: public Mass {
public:
	Room(double width, double depth, double height): Mass(width * depth * height, AIR_DENSITY, AIR_CAPACITY) {}
};

}

#endif

