#include <assert.h>
#include <stdexcept>
#include <math.h>
#include "solver.h"
#include "thermal.h"
#include "report.h"
#include "persistent.h"

Solver::Instant::Instant(): m_size(0) {
}

Solver::Instant::Instant(size_t size): m_size(size), m_energy(size, 0), m_allocMap((size/8)+1) {
	for (std::vector<unsigned char>::iterator it = m_allocMap.begin(); it != m_allocMap.end(); ++it) (*it) = 0;
}

Solver::ReportInstant::ReportInstant(size_t size): Instant(size) {
	for (std::vector<unsigned char>::iterator it = m_allocMap.begin(); it != m_allocMap.end(); ++it) (*it) = 0xFF;
}

void Solver::Instant::dump(const std::string & label) const {
	printf("\"%s\" instant dump\n=============\n", label.c_str());
	for (unsigned q = 0; q < m_energy.size(); q++) {
		if (valueIsSet(q)) printf("[%d] energy %f J\n", q, m_energy[q]);
		else printf("[%d]   <not set>\n", q);
	}
	printf("\n");
}

void Solver::Instant::resize(size_t new_size) {
//	size_t old_size = m_size;
	m_energy.resize(new_size, 0);
	m_allocMap.resize((new_size / 8) + 1, 0);
	// zero out newly "allocated elements"
//	for (unsigned q = old_size; q < new_size; ++q) {
//		printf("Zeroing %dth bit at offset %d in allocation array\n", q%8, q / 8);
//		m_allocMap[q/8] &= ~(1 < (q % 8));
//	}
	m_size = new_size;
}

bool Solver::Instant::valueIsSet(unsigned cell) const {
	if (cell >= m_size) return false;
	return (m_allocMap[cell / 8] & (1 << (cell % 8))) != 0;
}

void Solver::Instant::valueIsSet(unsigned cell, bool is) {
	if (is) m_allocMap[cell / 8] |= (1 << (cell % 8));
	else m_allocMap[cell / 8] &= ~(1 << (cell % 8));
}

double Solver::Instant::energy(unsigned cell) const {
	if (cell >= m_size) throw std::out_of_range("Not defined");
	if (!valueIsSet(cell)) throw std::invalid_argument("Not defined");
	return m_energy[cell];
}

bool Solver::Instant::validateEnergy(unsigned cell, double energy) {
	if (energy < 0.0) throw std::invalid_argument("");
	assert(energy >= 0.0);
	return true;
}

bool Solver::ReportInstant::validateEnergy(unsigned cell, double energy) {
	return true;
}

void Solver::Instant::energy(unsigned cell, double energy) {
	if (!this->validateEnergy(cell, energy)) return;
//	assert(energy >= 0.0);
	// not the most optimal way
	if (cell >= m_size) resize(cell + 1);
//	printf("%s(): Setting cell %d energy to %f\n", __FUNCTION__, cell, energy);
	m_energy[cell] = energy;
	valueIsSet(cell, true);
	return;
}

Solver::System::~System() {
	for (std::vector<Thermal::Path *>::iterator it = m_paths.begin(); it != m_paths.end(); ++it) delete *it;
	for (std::vector<Thermal::Cell *>::iterator it = m_cells.begin(); it != m_cells.end(); ++it) delete *it;
}

unsigned Solver::System::addCell(Thermal::Cell * cell) { 
	unsigned cellId = m_cells.size(); 
	m_cells.push_back(cell); 
	m_cellKeys.insert(std::map<unsigned, unsigned>::value_type(cell->key(), cellId)); 
//	printf("%s(): Cell key %d id is %d\n", __FUNCTION__, cell->key(), cellId);
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

Solver::System * Solver::StaticDissipation::clone() const {
	return new Solver::StaticDissipation(*this);
}

void Solver::System::initialTemperature(unsigned key, double temperature) {
	unsigned cellId = cellIdByKey(key);
//	printf("%s(): Cell key %d id is %d\n", __FUNCTION__, key, cellId);
	m_initialInstant.energy(cellId, cell(cellId)->energy(temperature));
//	m_initialInstant.dump("while setting initial temperatures");
}

bool Solver::StaticDissipation::solve(Report & report) {
	if (!verify()) return false;

	m_initialInstant.dump("initial");

	Instant * currentInstant = new Solver::Instant(m_initialInstant);
	this->currentInstant(currentInstant);

	// set fixed temperature of 20 kelvins for all undetermined cells
	// so initial conditions are all set
	for (unsigned q = 0; q < m_cells.size(); ++q) {
		if (!currentInstant->valueIsSet(q)) {
			printf("Setting artifical energy for cell %d\n", q);
			currentInstant->energy(q, m_cells[q]->energy(20 + KELVIN));
		}
	}
	
	double max_change = 0;
	double time_step = 1.0; // (in seconds)
	bool stepTooLong = false;

	do {
		Instant * newInstant = new Solver::Instant(*currentInstant);

		std::vector<Thermal::Path *>::iterator it;
		for (it = m_paths.begin(); it != m_paths.end(); ++it) {
			// calculate the amount of energy transferred during time of one second
			// this numerically equals to heat overall transfer
			double power = (*it)->transport(this, time_step);
	//		report.pathProperty((*it)->key(), "dissipatedPower", power);
			unsigned cell1Key = (*it)->cell1()->key();
			unsigned cell2Key = (*it)->cell2()->key();
			unsigned cell1Id = cellIdByKey(cell1Key);
			unsigned cell2Id = cellIdByKey(cell2Key);
	
			/* By definition positive power flow means that it flows from cell1 to cell2
			 * That basically means that cell1 temperature is higher than cell2 temperature
			 * at given instant.
			 */
			try {
				newInstant->energy(cell1Id, newInstant->energy(cell1Id) -	power);
				newInstant->energy(cell2Id, newInstant->energy(cell2Id) + power);
			} catch(std::invalid_argument & e) {
				time_step /= 10;
				printf("Shrinking time stemp to %f s\n", time_step);
				delete newInstant;
				stepTooLong = true;
				break;;
			}
		}
		if (stepTooLong) {
			stepTooLong = false;
			max_change = 10000000; // simply too large number
			continue;
		}

		max_change = 0;

		for (unsigned q = 0; q < m_cells.size(); ++q) {
			// if value is pre-set at begin of solve, fix it here too
			// this way only undetermined cell temperatures will change over time
			if (m_initialInstant.valueIsSet(q)) newInstant->energy(q, m_initialInstant.energy(q));

			// compute percentage amount of energy change
			double change = fabs((currentInstant->energy(q) - newInstant->energy(q)) / newInstant->energy(q));
			if (change > max_change) max_change = change;
		}
//		printf("%s(): max_change = %.3f\n", __FUNCTION__, max_change);
		// swap the instants
		delete currentInstant;
		currentInstant = newInstant;
		this->currentInstant(currentInstant);
		// we will end this if largest energy change is smaller than 0.1%
	} while (max_change > 0.0001);

	{
		ReportInstant reportInstant(currentInstant->size());
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
			try {
				reportInstant.energy(cell1Id, reportInstant.energy(cell1Id) - power);
				reportInstant.energy(cell2Id, reportInstant.energy(cell2Id) + power);
			} catch (std::exception & e) {
				fprintf(stderr, "Shit happens...\n");
				abort();
			}
		}

		std::vector<Thermal::Cell *>::iterator it2;
		for (it2 = m_cells.begin(); it2 != m_cells.end(); ++it2) {
			unsigned cellKey = (*it2)->key();
			unsigned cellId = cellIdByKey(cellKey);

			report.cellProperty(cellKey, "powerLoss", reportInstant.energy(cellId));
			if (!m_initialInstant.valueIsSet(cellId))
				report.cellProperty(cellKey, "equlibrium", (*it2)->temperature(currentInstant->energy(cellId)) - KELVIN);
		}

	}


	return true;
}
