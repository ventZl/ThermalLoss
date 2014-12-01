#include "model/material.h"
#include "model/building.h"

int main(int argc, char ** argv) {
	Model::MaterialLibrary * ml = Model::loadMaterialLibrary("./default.ml");
	if (ml == NULL) printf("Material library is invalid\n");
	Model::Building * b = Model::loadBuilding("./building.tl");
	if (b == NULL) printf("Building is invalid\n");
//	b->compute(ml);
	return 0;
}
