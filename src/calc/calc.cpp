#include "calc.h"

bool Calc::Calculation::load(Model::Building & building) {
	for (int q = 0; q < building.rooms(); ++q) {
		collectRooms(building.room(q));
	}	
}

void Calc::Calculation::collectRooms(const Model::Room & room) {
	
}
