#include "thermal.h"

double Thermal::Mass::temperature(double energy) {
	return energy / (m_volume * m_density * m_capacity);
}

double Thermal::Barrier(double timeslice) {

}
