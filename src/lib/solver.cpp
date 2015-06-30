#include "solver.h"
#include "thermal.h"
#include "report.h"

unsigned Solver::System::addCell(Thermal::Cell * cell) { 
	unsigned cellId = m_cells.size(); 
	m_cells.push_back(cell); 
	m_cellKeys.insert(std::map<unsigned, unsigned>::value_type(cell->key(), cellId)); 
	return cellId; 
}

unsigned Solver::System::addPath(Thermal::Path * path) { 
	unsigned pathId = m_paths.size(); 
	m_paths.push_back(path);
	/* m_pathKeys.insert(std::map<unsigned, unsigned>::value_type(path.key(), pathId));*/ 
	return pathId; 
}

bool Solver::System::verify() {
	std::vector<Thermal::Cell *>::iterator it;
	for (it = m_cells.begin(); it != m_cells.end(); ++it) {
		if ((*it)->paths().size() == 0) return false;
	}
	return true;
}

bool Solver::StaticDissipation::solve(Report & report) {
	if (!verify()) return false;

	Instant * currentInstant = new Solver::Instant(m_initialInstant);
	this->currentInstant(currentInstant);
	Instant * reportInstant = new Solver::Instant();

	std::vector<Thermal::Path *>::iterator it;
	for (it = m_paths.begin(); it != m_paths.end(); ++it) {
		// calculate the amount of energy transferred during time of one second
		// this numerically equals to heat overall transfer
		double power = (*it)->transport(this, 1.0);
		report.pathProperty((*it)->key(), "dissipatedPower", power);
		unsigned cell1Key = (*it)->cell1()->key();
		unsigned cell2Key = (*it)->cell2()->key();
		unsigned cell1Id = cellIdByKey(cell1Key);
		unsigned cell2Id = cellIdByKey(cell2Key);

		/* By definition positive power flow means that it flows from cell1 to cell2
		 * That basically means that cell1 temperature is higher than cell2 temperature
		 * at given instant.
		 */
		reportInstant->energy(cell1Id, reportInstant->energy(cell1Id) -	power);
		reportInstant->energy(cell2Id, reportInstant->energy(cell2Id) + power);
	}

	std::vector<Thermal::Cell *>::iterator it2;
	for (it2 = m_cells.begin(); it2 != m_cells.end(); ++it2) {
		unsigned cellKey = (*it2)->key();
		unsigned cellId = cellIdByKey(cellKey);

		report.cellProperty(cellKey, "powerLoss", reportInstant->energy(cellId));
	}

	return true;
}
