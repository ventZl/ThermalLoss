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
	printf("%s(): cell1 id = %d\ncell2 id = %d\n", __FUNCTION__, cell1Id, cell2Id);
	double temp1 = cell1()->temperature(system->currentInstant()->energy(cell1Id));
	double temp2 = cell2()->temperature(system->currentInstant()->energy(cell2Id));
	printf("%s(): cell1 temperature = %.2f\n%s(): cell2 temperature = %.2f\n", __FUNCTION__, temp1,  __FUNCTION__,temp2);
	printf("%s(): conductivity = %.2f m.K/W\n%s(): surface = %.2f m^2\n%s(): width = %.2f m\n", __FUNCTION__, m_conductivity, __FUNCTION__, m_surface, __FUNCTION__, m_width);
	double value = (((m_conductivity * (temp1 - temp2)) / m_width) * m_surface) / timeslice;	
	printf("%s(): heat transported = %.2f J\n", __FUNCTION__, value);
	return value;
}
