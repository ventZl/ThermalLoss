#include "calc.h"
#include <model/building.h>

bool Calc::Calculation::load(Model::Building & building) {
	collectRooms(building);
	return true;
}

void Calc::Calculation::collectRooms(const Model::Building & building) {
	for (unsigned q = 0; q < building.rooms(); ++q) {
		collectWalls(building.room(q));
	}
	return;
}

void Calc::Calculation::collectWalls(const Model::Room & room) {
	for (unsigned q = 0; q < room.walls(); ++q) {
		collectWindows(room.wall(q));
	}
	return;
}

void Calc::Calculation::collectWindows(const Model::Wall & wall) {
	for (unsigned q = 0; q < wall.windows(); ++q) {
		const Model::Window & modelWin = wall.window(q);
		Window win(modelWin.width(), modelWin.height(), 1.0/modelWin.conductivity());
	}
	return;
}
