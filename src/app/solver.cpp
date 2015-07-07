#include <lib/solver.h>
#include <lib/thermal.h>
#include <lib/persistent.h>

int main(int argc, char ** argv) {
	Persistent::StaticDissipation * persistentSolver = new Persistent::StaticDissipation();
	Solver::System * system = NULL;
	if (JSON::parse(argv[1], persistentSolver)) {
		system = persistentSolver->clone();
	}
	return 0;
}
