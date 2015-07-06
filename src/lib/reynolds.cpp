#include "reynolds.h"

double Reynolds::number(double diameter, double velocity, double density, double viscosity) {
	return (diameter * velocity * density) / (viscosity);
}

double Reynolds::diameter(double reynolds, double velocity, double density, double viscosity) {
	return (reynolds * viscosity) / (velocity * density);
}

double Reynolds::velocity(double reynolds, double diameter, double density, double viscosity) {
	return (reynolds * viscosity) / (diameter * density);
}

