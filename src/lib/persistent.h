#include <json/json.h>
#include "named.h"
#include "solver.h"
#include "thermal.h"

#define JSON_ATTACH(name) json_ ## name(&m_ ## name)
#define JSON_AUX_PROPERTY(name) # name
#define JSON_PROPERTY(name) addProperty(JSON_AUX_PROPERTY(name), &json_ ## name)

namespace Namable {
	class Named;
}

namespace Namable {
	namespace Persistent {
		class Name: public JSON::Number {
		public:
			Name(Namable::Named * entity): m_entity(entity) {}
			void setValue(float value) { m_entity->name((unsigned) value); }
			float getValue() const { return m_entity->name(); }

		protected:
			Namable::Named * m_entity;
		};
	}
}

namespace Persistent {
	class Mass: public Thermal::Mass, public JSON::Struct {
	public:
		Mass(): json_name(this), JSON_ATTACH(volume), JSON_ATTACH(density), JSON_ATTACH(capacity) { addProperties(); }

	protected:
		void addProperties() { JSON_PROPERTY(name); JSON_PROPERTY(volume); JSON_PROPERTY(density); JSON_PROPERTY(capacity); }

	protected:
		Namable::Persistent::Name json_name;
		JSON::Simple::Double json_volume;
		JSON::Simple::Double json_density;
		JSON::Simple::Double json_capacity;
	};

	class Barrier: public Thermal::Barrier, public JSON::Struct {
	public:
		Barrier(): json_name(this), JSON_ATTACH(surface), JSON_ATTACH(width), JSON_ATTACH(conductivity) { addProperties(); }

	protected:
		void addProperties() { JSON_PROPERTY(name); JSON_PROPERTY(surface); JSON_PROPERTY(width); JSON_PROPERTY(conductivity); }

	protected:
		Namable::Persistent::Name json_name;
		JSON::Simple::Double json_surface;
		JSON::Simple::Double json_width;
		JSON::Simple::Double json_conductivity;
	};

	typedef std::vector<Mass *> MassVector;
	typedef std::vector<Barrier *> BarrierVector;

	class StaticDissipation: public Solver::StaticDissipation, public JSON::Struct {
	public:
		JSON_ARRAY_PROXY(StaticDissipationMassProxy, StaticDissipation, m_masses, Mass);
		JSON_ARRAY_PROXY(StaticDissipationBarriersProxy, StaticDissipation, m_barriers, Barrier);

		StaticDissipation(): json_barriers(this), json_masses(this) { addProperties(); }

	protected:
		void addProperties() { JSON_PROPERTY(barriers); JSON_PROPERTY(masses); }

	protected:
		StaticDissipationBarriersProxy json_barriers;
		BarrierVector m_barriers;

		StaticDissipationMassProxy json_masses;
		MassVector m_masses;
	};

}
