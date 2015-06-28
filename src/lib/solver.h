#ifndef _LIB_SOLVER_H__
#define _LIB_SOLVER_H__

#include <vector>

namespace Thermal {
	class Cell;
	class Path;
};

namespace Solver {

class Instant {
public:
	double energy(Thermal::Cell & cell) const;
	void energy(Thermal::Cell & cell, double energy);

protected:
	std::map<Thermal::Cell &, double> m_energy;
};

class System {
public:
	System() {}
	void addCell(Cell & cell) { m_cells.push_back(cell); }
	void addPath(Path & path) { m_paths.push_back(path); }

	void solve();

protected:
	std::vector<Cell &> m_cells;
	std::vector<Path &> m_paths;
};

}

#endif

