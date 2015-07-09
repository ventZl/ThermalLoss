#ifndef _LIB_THERMAL_H__
#define _LIB_THERMAL_H__

#include <vector>
#include "named.h"

#define KELVIN			273.16

namespace Solver {
	class System;
}

namespace Thermal {

class Path;

/** Interface for purpose of representing heat amount
 * in certain region of space 
 */
class Cell: public Namable::Named {
public:
	/** Abstract cell constructor
	 * @param key user-chosen ID which will be used to identify this cell again
	 */
	Cell(unsigned key): Named(key) {};

protected:
	/** For experimental JSON serialization purposes.
	 */
	Cell() {}

public:
	virtual ~Cell();

	/** Clone the instance regardless of user-visible type
	 * @return cloned instance
	 */
	virtual Cell * clone() const = 0 ; //{ return new Cell(this); }

	/** Get temperature equivalent for given amount of energy.
	 * This is not physically accurate value! In reality entity
	 * energy is not directly proportional to it's temperature
	 * due to e.g. phase transition from solid to liquid.
	 * @param energy amount of energy contained in cell
	 * @return temperature of cell (in Kelvins)
	 */
	virtual double temperature(double energy) const = 0;

	/** Get energy equivalent for given temperature of cell
	 * This is not physically accurate value! In reality entity
	 * temperature is not directly proportional to it's energy
	 * due to e.g. phase transition from solid to liquid.
	 * @param temperature temperature of cell
	 * @return energy of cell (in Jouls)
	 */
	virtual double energy(double temperature) const = 0;

	/** Get list of paths going from or to this cell
	 * @return paths which ends in this cell
	 */
	const std::vector<Path *> paths() const { return m_paths; }

	/** Get user-set ID of this cell.
	 * @return ID
	 */
	unsigned key() const { return name(); }

protected:
	/** Adds path ending in this cell
	 * @param path path to be added
	 */
	void addPath(Path * path) { m_paths.push_back(path); }

	/** Remove path from list of paths.
	 * @param path path to be removed
	 */
	void removePath(Path * path);

	std::vector<Path *> m_paths;			///< list of paths ending in this cell

	friend class Path;
};

/** Interface for purpose of representing point-to-point
 * connection between two entities able to conduct
 * heat.
 */
class Path: public Namable::Named {
public:
	/* Create heat transport path from cell1 to cell2
	 * @param key user-set external ID of this path
	 * @param cell1 one endpoint of path
	 * @param cell2 another endpoint of path
	 */
	Path(unsigned key, Cell * cell1, Cell * cell2): Named(key), m_cell1(cell1), m_cell2(cell2) { m_cell1->addPath(this); m_cell2->addPath(this); }
	
	/** Clone the object retaining correct type.
	 * @return clone of object
	 */
	virtual Path * clone() const = 0; //{ return new Path(this); }

protected:
	/** Used in experimental JSON serializer.
	 */
	Path(): m_cell1(NULL), m_cell2(NULL) {}

	/** Set first endpoint of path.
	 * @param cell pointer to cell
	 */
	void cell1(Cell * cell) { m_cell1 = cell; }

	/** Set second endpoint of path.
	 * @param cell pointer to cell
	 */
	void cell2(Cell * cell) { m_cell2 = cell; }

public:
	virtual ~Path() { m_cell1->removePath(this); m_cell2->removePath(this); }

	/** Compute the amount of heat transported by this path.
	 * @param system mesh definition
	 * @param timeslice amount of time for which heat transport is simulated (in seconds)
	 * @note For use with static thermal studies the time of 1 second is used
	 * @return amount of heat transported (in Joules)
	 */ 
	virtual double transport(Solver::System * system, double timeslice) = 0;

	/** Get external ID
	 * @return ID
	 */
	unsigned key() const { return name(); }

	/** Get first endpoint of path.
	 * @return pointer to cell
	 */
	Cell * cell1() { return m_cell1; }

	/** Get second endpoint of path.
	 * @return pointer to cell
	 */
	Cell * cell2() { return m_cell2; }

protected:
	Cell * m_cell1, * m_cell2;			///< associated cells
};

/** Representation of mass holding heat
 */
class Mass: public Cell {
public:
	/** Create new mass.
	 * @param key user-set identifier
	 * @param volume spatial volume of mass
	 * @param density material density of mass
	 * @param capacity thermal capacity of mass
	 */
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

/** Representation of thermal conductive barrier.
 * Represents material of which it's heat conductivity
 * and amount of heat transferred is important.
 */
class Barrier: public Path {
public:
	/** Create new barrier.
	 * @param key user-set identifier
	 * @param surface surface of the barrier
	 * @param width width of barrier
	 * @param conductivity thermal conductivity of material
	 * @param cell1 mass at first endpoint of barrier
	 * @param cell2 mass at other endpoint of barrier
	 */
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

/** Simplification for room with same height at all points.
 */
class Room: public Mass {
public:
	/** Create room.
	 * @param key user-set identifier
	 * @param area surface area of room floor
	 * @param height floor height in room
	 */
	Room(unsigned key, double area, double height): Mass(key, area * height, AIR_DENSITY, AIR_CAPACITY) {}

	virtual Cell * clone() const { return new Room(*this); }

};

}

#endif

