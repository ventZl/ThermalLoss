#include "model/material.h"
#include "model/building.h"
#include "model/losses.h"
#include "calc/calc.h"
#include <lib/solver.h>

#define STR_AUX(s) #s
#define STR(s) STR_AUX(s)

Model::Parameters * loadParameters(const std::string fileName) {
	Model::Parameters * p = new Model::Parameters();
	if (parse(fileName, p)) {
	} else {
		printf("Error while parsing parameters\n");
		delete p;
		p = NULL;
	}
	return p;
}

int main(int argc, char ** argv) {
	Model::MaterialLibrary * ml = Model::loadMaterialLibrary(STR(SOURCE_DIR) "/shared/default.materials");
	if (ml == NULL) printf("Material library '" STR(SOURCE_DIR) "/shared/default.materials' is invalid\n");
	if (argc < 4) {
		printf("Usage: %s file.building file.parameters file.out\n\nfile.building      - file containing building definition\nfile.parameters - file containing computational parameters\nfile.out     - file where output will be written\n", argv[0]);
		return 0;
	}
	Model::Building * building = Model::loadBuilding(argv[1]);
	if (building == NULL) printf("Building is invalid\n");
	Model::Parameters * parameters = loadParameters(argv[2]);
	Solver::StaticDissipation * solver = new Solver::StaticDissipation();
	Calc::Calculation * calc = new Calc::Calculation(solver);
	calc->load(*building);
	Model::Losses * losses = calc->calculate(*parameters, *ml);
	JSON::store(argv[3], losses);

	return 0;
}
