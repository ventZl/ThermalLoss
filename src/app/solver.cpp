#include <lib/solver.h>
#include <lib/thermal.h>
#include <lib/persistent.h>

int main(int argc, char ** argv) {
	Persistent::System * persistentSolver = new Persistent::System();
	Solver::System * system = NULL;
	if (JSON::parse(argv[1], persistentSolver)) {
		system = new Solver::System(*persistentSolver);
	}
	return 0;
}
