#ifndef _LIB_THERMAL_H__
#define _LIB_THERMAL_H__

#include <vector>
#include "named.h"

namespace Solver {
	class System;
}

namespace Thermal {

class Path;

// Interface for purpose of representing heat amount
// in certain region of space 
class Cell: public Namable::Named {
public:
	Cell(unsigned key): Named(key) {};

protected:
	Cell() {}

public:
	virtual ~Cell();

	virtual Cell * clone() const = 0 ; //{ return new Cell(this); }

	// Get representation of temperature of cell (value in Kelvins)
	virtual double temperature(double energy) const = 0;
	virtual double energy(double temperature) const = 0;
	const std::vector<Path *> paths() const { return m_paths; }
	unsigned key() const { return name(); }

protected:
	void addPath(Path * path) { m_paths.push_back(path); }
	void removePath(Path * path);

	std::vector<Path *> m_paths;
	unsigned m_key;

	friend class Path;
};

class Path: public Namable::Named {
public:
	// Create heat transport path from cell1 to cell2
	Path(unsigned key, Cell * cell1, Cell * cell2): Named(key), m_cell1(cell1), m_cell2(cell2) { m_cell1->addPath(this); m_cell2->addPath(this); }
	virtual Path * clone() const = 0; //{ return new Path(this); }

protected:
	Path(): m_cell1(NULL), m_cell2(NULL) {}
	void cell1(Cell * cell) { m_cell1 = cell; }
	void cell2(Cell * cell) { m_cell2 = cell; }

public:
	virtual ~Path() { m_cell1->removePath(this); m_cell2->removePath(this); }

	// Transport given amount of heat between connected cells
	virtual double transport(Solver::System * system, double timeslice) = 0;
	unsigned key() const { return name(); }

	Cell * cell1() { return m_cell1; }
	Cell * cell2() { return m_cell2; }

protected:
	double m_key;
	Cell * m_cell1, * m_cell2;
};

class Mass: public Cell {
public:
	Mass(unsigned key, double volume, double density, double capacity): Cell(key), m_volume(volume), m_density(density), m_capacity(capacity) {}


protected:
	Mass() {}

public:
	virtual Cell * clone() const { return new Mass(*this); }
	virtual double temperature(double energy) const;
	virtual double energy(double temperature) const;

protected:
	double m_volume;
	double m_density;
	double m_capacity;
};

class Barrier: public Path {
public:
	Barrier(unsigned key, double surface, double width, double conductivity, Cell * cell1,	Cell * cell2): Path(key, cell1, cell2), m_surface(surface), m_width(width), m_conductivity(conductivity) {}
	// Constructor for cases when you know overall thermal conductivity, not just material
	Barrier(unsigned key, double surface, double conductivity, Cell * cell1, Cell * cell2): Path(key, cell1, cell2), m_surface(surface), m_width(1), m_conductivity(conductivity) {}

protected:
	Barrier() {}

public:
	virtual Path * clone() const { return new Barrier(*this); }
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
	Room(unsigned key, double area, double height): Mass(key, area * height, AIR_DENSITY, AIR_CAPACITY) {}
	virtual Cell * clone() const { return new Room(*this); }

};

}

#endif

