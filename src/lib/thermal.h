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
	virtual double temperature() const = 0;

	// Add / remove heat from/to cell (value in Jouls)
	virtual void heat(double amount) = 0;

protected:
	void addPath(Path & path) { m_paths.push_back(path); }
	void removePath(Path & path) { std::vector<Path &>::iterator it = m_paths.find(path); if (it != m_paths.end()) m_paths.erase(it); }

	std::vector<Path &> m_paths;
};


class Path {
public:
	// Create heat transport path from cell1 to cell2
	Path(Cell & cell1, Cell & cell2): m_cell1(cell1), m_cell2(cell2) { m_cell1.addPath(this); m_cell2.addPath(this); }
	virtual ~Path() { m_cell1.removePath(this); m_cell2.removePath(this); }

	// Transport given amount of heat between connected cells
	virtual void transport(double timeslice);

	virtual void commit() = 0;
	virtual void rollback() = 0;

protected:
	Cell & cell1() { return m_cell1; }
	Cell & cell2() { return m_cell2; }

protected:
	Cell & m_cell1, & m_cell2;
};

class ThermalMass;

class MassInteractor {
	virtual void heat(ThermalMass * mass, double heat) = 0;
	virtual void commit(ThermalMass * mass) = 0;
	virtual void rollback(ThermalMass * mass) = 0;
};

class ThermalMass: public Cell {
public:
	ThermalMass(double volume, double density double capacity, MassInteractor * interactor): m_volume(volume), m_density(volume), m_capacity(capacity), m_interactor(interactor) {}

	virtual double temperature() const = 0;
	virtual void heat(double heat) { m_interactor->heat(this, heat); }
	virtual void commit() { m_interactor->commit(this); }
	virtual void rollback() { m_interactor->rollback(this); }

	virtual double oldHeat() const = 0;
	virtual void changeHeat(double delta) = 0;
	virtual void commitHeat() = 0;
	virtual void rollbackHeat() = 0;

protected:
	double m_volume;
	double m_density;
	double m_capacity;
	MassInteractor * m_interactor;
};

class ThermalBarrier;

class BarrierInteractor {

};

class ThermalBarrier: public Path {
public:
	ThermalBarrier(double surface, double width, double conductivity, BarrierInteractor * interactor): m_surface(surface), m_width(width), m_conductivity(conductivity) {}

	/* Return true if rate of change of thermal flow is within limits */
	virtual bool transport(double timeslice); // { m_interactor->transport(timeslice); }
	virtual void commit() { m_interactor->commit(this); }
	virtual void rollback() { m_interactor->rollback(this); }

	virtual void commitFlow() = 0;
	virtual void rollbackFlow() = 0;

protected:
	BarrierInteractor * m_interactor;
	double m_surface;
	double m_width;
	double m_conductivity;
};

class Room: public ThermalMass {
	
};

}

#endif

