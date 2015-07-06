#ifndef _LIB_REYNOLDS_H__
#define _LIB_REYNOLDS_H__

#define WATER_DENSITY	1
#define WATER_VISCOSITY_40 0.547
#define WATER_VISCOSITY_90 0.315


namespace Reynolds {

double number(double diameter, double velocity, double density, double viscosity);
double diameter(double reynolds, double velocity, double density, double viscosity);
double velocity(double reynolds, double diameter, double density, double viscosity); 

}
#endif

