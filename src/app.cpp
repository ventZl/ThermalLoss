#include "model.h"

int main(int argc, char ** argv) {
	MaterialLibrary * ml = loadMaterialLibrary("./default.ml");
	if (ml == NULL) printf("Material library is invalid\n");
	Building * b = loadBuilding("./building.tl");
	if (b == NULL) printf("Building is invalid\n");
	b->compute(ml);
	return 0;
}
