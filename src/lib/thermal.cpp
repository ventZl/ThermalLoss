#include <algorithm>

#include "thermal.h"
#include "solver.h"

Thermal::Cell::~Cell() {
}

void Thermal::Cell::removePath(Path * path) { 
	std::vector<Path *>::iterator it = std::find(m_paths.begin(), m_paths.end(), path); 
	if (it != m_paths.end()) m_paths.erase(it); 
}


double Thermal::Mass::temperature(double energy) const {
	return energy / (m_volume * m_density * m_capacity);
}

double Thermal::Mass::energy(double temperature) const {
	return m_volume * m_density * m_capacity * temperature;
}

double Thermal::Barrier::transport(Solver::System * system, double timeslice) {
	unsigned cell1Id = system->cellIdByKey(cell1()->key());
	unsigned cell2Id = system->cellIdByKey(cell2()->key());
	printf("cell1 id = %d\ncell2 id = %d\n", cell1Id, cell2Id);
	double temp1 = cell1()->temperature(system->currentInstant()->energy(cell1Id));
	double temp2 = cell2()->temperature(system->currentInstant()->energy(cell2Id));
	printf("cell1 temperature = %.2f\ncell2 temperature = %.2f\n", temp1, temp2);
	printf("conductivity = %.2f m.K/W\nsurface = %.2f m^2\nwidth = %.2f m\n", m_conductivity, m_surface, m_width);
	return ((m_conductivity * (temp1 - temp2)) / m_width) * m_surface;	
}
