#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cstdio>
#include <lib/reynolds.h>
#include "heating.h"

// Below are two variants of friction coefficient calculation.
// Former is simpler latter is more precise.
// For my purposes the results were roughly the same

/**
 * @param Re reynolds number
 * @param d inner diameter of pipe (in meters)
 * @param k absolute roughness (in meters)
 * @return friction coefficient lambda
 */

/** Not a bad result but less exact as below? */
double __friction_coeff(double Re, double d, double k) {
	return 0.1 * pow(100.0/Re + k/d, 0.25);
}

double __a(double Re, double epsylon) {
	return pow(
			-2.457 * log(
					pow(7.0/Re, 0.9) + 0.27 * epsylon
				)
			,16
			);
}

double __b(double Re) {
	return pow(37530.0/Re, 16);
}

double friction_coeff(double Re, double d, double k) {
	return 8.0 * pow(
			pow(8.0/Re, 12) + (1.0f/ pow(__a(Re, k/d) + __b(Re), 1.5))
			, (1.0/12.0)
			);
}

double friction_loss(double lambda, double length, double diameter, double speed, double density) {
	return lambda * (length/diameter) * (pow(speed, 2.0) / 2.0) * density;
}

double flow(double power, double deltaT) {
	return (power / (4186 * deltaT)) / 985.7;
}

double speed(double flow, double diameter) {
	double surface = M_PI * pow(diameter / 2.0, 2);
	return flow / surface;
}

double pipeTempLoss(double temp, double diameter, double wall, double length, double speed) {
	double R = wall / 372;
	double dT = temp - 20;
	double fi = dT / R;
	printf("Flow is %f ??\n", fi);
	double surface = 2.0 * M_PI * ((diameter / 2.0) + (wall / 2.0)) * length;
	printf("Surface is %f m2\n", surface);
	double volume = M_PI * pow(diameter / 2.0, 2) * length;
	double power = fi * surface;
	double energy = power / (length / speed);

	return (volume * 985.7 * 4186) / energy;
}

struct pipe {
	double diameter;
	double wall;
	double length;
};

int main(int argc, char ** argv) {
	double powers[] = { 1000, 1000, // obyvacka
						601, // kuchyna
						1520, // spalna
						364, // kupelna
						690, // hostovska
						922 // kuchyna
						};

	double gradient[] = { 10 - 2*0.06, 10 - 2*.06,
						10,
						10,
						10,
						10,
						10
						};
//	double deltaT = 10;
//
	double sourceTemp = 55;

	printf("Pipe loss is %f deg. C\n", pipeTempLoss(55, 0.02, 0.001, 19.29, 0.47));
	char * names[] = { "Obyvacka juh", "Obyvacka vychod", "Kuchyna", "Spalna", "Kupelna", "Hostovska", "Kuchyna #2" };
	double * flows = (double *) malloc(sizeof(powers));
	double overal = 0;
	for (unsigned q = 0; q < sizeof(powers)/sizeof(powers[0]); q++) {
		flows[q] = flow(powers[q], gradient[q]);
		printf("Flow in %s is %.6f m^3/s (%.3f l/s)\n", names[q], flows[q], flows[q]*1000.0);
		overal += flows[q];
	}
	printf("\nOveral flow is %.6f m^3/s %.3f m^3/h (%.3f l/s)\n", overal, overal*3600, overal*1000.0);
	double diameters[] = { 0.010, 0.013, 0.016, 0.02 };

	for (unsigned q = 0; q < sizeof(diameters)/sizeof(diameters[0]); q++) {	
		double diameter = diameters[q];
		printf("\nDiameter = %d mm\n================\n", (int) (diameter * 1000.0f));

		double spd = speed(overal, diameter);

		printf("Flow speed is %.3f m/s\n", spd);

		double Re = Reynolds::number(diameter, spd, 985.7, 0.000547);

		printf("Reynolds number is %f\n", Re);

		double lambda = friction_coeff(Re, diameter, 0.000003);

		printf("Lambda is %f\n", lambda);

		double loss = friction_loss(lambda, 5, diameter, spd, 985.7);

		printf("Friction loss of 5m of pipe is %.3f Pa (%.3f mbar)\n", loss, loss/1000.0f);
	}
	return 0;
}
