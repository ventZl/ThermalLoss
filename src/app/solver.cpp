#include <lib/solver.h>
#include <lib/thermal.h>
#include <lib/persistent.h>
#include <lib/report.h>

using namespace Thermal;

#define KELVIN +273.15

int main(int argc, char ** argv) {
	Mass * r1 = new Room(1, 5.5 * 5.5, 2.4);
	Mass * m2 = new Mass(2, 1.0, AIR_DENSITY, AIR_CAPACITY);
	Barrier * b1 = new Barrier(3, 5.5*2.4, 0.25, 0.3, r1, m2);
	Barrier * b2 = new Barrier(3, 5.5*2.4, 0.25, 0.3, r1, m2);
	Barrier * b3 = new Barrier(3, 5.5*2.4, 0.25, 0.3, r1, m2);
	Barrier * b4 = new Barrier(3, 5.5*2.4, 0.25, 0.3, r1, m2);
	Solver::StaticDissipation * solver = new Solver::StaticDissipation();
	solver->addCell(r1);
	solver->addCell(m2);
	solver->addPath(b1);
	solver->addPath(b2);
	solver->addPath(b3);
	solver->addPath(b4);
	solver->initialTemperature(1, 20 KELVIN);
	solver->initialTemperature(2, -12 KELVIN);
	Solver::Report report;
	if (solver->solve(report)) {
		report.save("report.thermal");
	}
	return 0;
}
