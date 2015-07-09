#ifndef _LIB_SOLVER_H__
#define _LIB_SOLVER_H__

#include <vector>
#include <map>

namespace Thermal {
	class Cell;
	class Path;
}

namespace Persistent {
	class System;
}

namespace Solver {

class Report;

/** Contains values of energy for cells at a given time.
 * This class allows to implement transient solvers with
 * adaptive time step.
 */
class Instant {
public:
	/** Creates empty instant */
	Instant() {}
	/** Creates pre-sized instant.
	 * Allows potentially faster operation if size of mesh is known.
	 * @param size mesh size for which instant is created
	 */
	Instant(size_t size) { m_energy.resize(size); }

	/** Get energy of a mesh cell.
	 * @param cell ID (offset) of cell in mesh
	 * @return energy (in Joules)
	 */
	double energy(unsigned cell) const;

	/** Set energy of a mesh cell.
	 * @param cell ID (offset) of cell in mesh
	 * @param energy energy of cell (in Joules)
	 */
	void energy(unsigned cell, double energy);

	/** Get size of instant (count of cells)
	 * @return amount of cells in instant
	 */
	size_t size() const { return m_energy.size(); }

protected:
	std::vector<double> m_energy;		///< cell energy storage
};

/** Solver public interface.
 * Allows to add cells and paths into mesh, specify starting conditions
 * and to launch solve process.
 */
class System {
public:
	System() {}
	virtual ~System();

	/** Clone the object.
	 * Useful in case when you need to get the most down-the-tree type
	 * from place somewhere you don't know the exact type.
	 * Used in experimental JSON serializer when getting rid of
	 * persistence allowed instance.
	 * @return cloned instance
	 */
	virtual System * clone() const = 0;

	/** Adds a cell into mesh
	 * Adds another cell into mesh. It is not generally a good idea
	 * to attempt to add one cell twice into mesh as weird things will happen.
	 * This case is not checked. Usually is good idea to add cell before
	 * adding paths connecting it to another cells.
	 * @param cell cell to be added
	 * @return internal ID assigned to cell
	 */
	unsigned addCell(Thermal::Cell * cell);

	/** Adds a path into mesh
	 * Add another path into mesh. Is it not generally a good idea
	 * to attempt to add one path twice into mesh as weird thing will happen.
	 * Try to add paths only after related cells were already added into mesh.
	 * @param path path to be added
	 * @return internal ID assigned to path
	 */
	unsigned addPath(Thermal::Path * path);

	/** Get cell by it's ID
	 * Returns cell by it's ID.
	 * @param id internal ID of cell
	 * @return cell. If no such cell exists NULL will be returned (hopefully)
	 */
	Thermal::Cell * cell(unsigned id) { return m_cells[id]; }

	/** Get path by it's ID
	 * @param internal id ID of path
	 * @return path. If no such path exists NULL will be returned (hopefully)
	 */
	Thermal::Path * path(unsigned id) { return m_paths[id]; }

	/** Sets initial temperature of cell
	 * Allows user to set initial conditions for simulation
	 * @param key external ID of cell to be set
	 * @param temperature temperature in Kelvins to be set
	 */
	void initialTemperature(unsigned key, double temperature);

	/** Get cell internal ID by it's external ID
	 * @param key external ID of cell
	 * @return internal ID of cell
	 */
	unsigned cellIdByKey(unsigned key) { return m_cellKeys[key]; }
//	unsigned pathIdByKey(unsigned key) { return m_pathKeys[key]; }
	/** Verify that all the cells are connected.
	 * @return true if no problems were found false otherwise
	 */
	bool verify();

	/** Launch the solve process.
	 * Solves the mesh given solve override.
	 * @param report report to store solve results
	 */
	virtual bool solve(Report & report) = 0;

	/** Get the current instant.
	 * Returns current instant - the instant which is last
	 * full computed state.
	 * @return instant instance
	 */
	Instant * currentInstant() const { return m_currentInstant; }

protected:
	/** Sets the instant as current instant.
	 * Usually called at end of computation step or
	 * when rolling back last step due to decrease 
	 * of precision.
	 * @param instant instant to be set as current
	 */
	void currentInstant(Instant * instant) { m_currentInstant = instant; } 

protected:
	std::map<unsigned, unsigned> m_cellKeys;	///< mapping from external ID to internal ID for cells
//	std::map<unsigned, unsigned> m_pathKeys;	///< 
	std::vector<Thermal::Cell *> m_cells;		///< cells contained in mesh
	std::vector<Thermal::Path *> m_paths;		///< paths contained in mesh

	Instant m_initialInstant;					///< initial instant
	Instant * m_currentInstant;					///< last fully computed instant
};

/** Static dissipation solver.
 * Solves thermal static stress simulations.
 * Records the amount of heat emitted and received for
 * each cell and amount of heat trasmitted over each path for
 * given initial conditions. Only one step is calculated always
 * in this type of simulation.
 */
class StaticDissipation: virtual public System {
public:
	StaticDissipation() {}

	/** Implementation of clone method
	 */
	virtual System * clone() const;

	/** Solve static dissipation problem.
	 */
	virtual bool solve(Report & report);
};

}

#endif

