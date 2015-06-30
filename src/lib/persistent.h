#include <json/json.h>
#include "solver.h"
#include "thermal.h"

namespace Persistent {
	class System: public Solver::System, public JSON::Struct {
	};

	class Mass: public Thermal::Mass, public JSON::Struct {
	};

	class Barrier: public Thermal::Barrier, public JSON::Struct {
	};
};
