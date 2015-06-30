#ifndef _LIB_SOLVER_H__
#define _LIB_SOLVER_H__

#include <vector>
#include <map>

namespace Thermal {
	class Cell;
	class Path;
}

namespace Solver {

class Report;

class Instant {
public:
	double energy(unsigned cell) const;
	void energy(unsigned cell, double energy);

protected:
	std::vector<double> m_energy;
};

class System {
public:
	System() {}
	virtual ~System();
	unsigned addCell(Thermal::Cell * cell);
	unsigned addPath(Thermal::Path * path);
	Thermal::Cell * cell(unsigned id) { return m_cells[id]; }
	Thermal::Path * path(unsigned id) { return m_paths[id]; }
	void initialTemperature(unsigned key, double temperature);
	unsigned cellIdByKey(unsigned key) { return m_cellKeys[key]; }
//	unsigned pathIdByKey(unsigned key) { return m_pathKeys[key]; }
	/** Verify that all the cells are connected.
	 */
	bool verify();

	virtual void solve() = 0;

	Instant * currentInstant() const { return m_currentInstant; }

protected:
	void currentInstant(Instant * instant) { m_currentInstant = instant; } 

protected:
	std::map<unsigned, unsigned> m_cellKeys;
	std::map<unsigned, unsigned> m_pathKeys;
	std::vector<Thermal::Cell *> m_cells;
	std::vector<Thermal::Path *> m_paths;

	Instant m_initialInstant;
	Instant * m_currentInstant;
};

class StaticDissipation: public System {
public:
	StaticDissipation() {}

	virtual bool solve(Report & report);
};

}

#endif

