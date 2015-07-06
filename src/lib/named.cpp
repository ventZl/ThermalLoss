#include "named.h"

void Namable::Named::add(unsigned name, Namable::Named * entity) {
	if (Namable::Named::s_namedList.find(name) == Namable::Named::s_namedList.end())
		Namable::Named::s_namedList.insert(Namable::Named::NamedList::value_type(name, entity));
	else
		std::runtime_error("Two or more entities with same name!");
}

void Namable::Named::remove(unsigned name) {
	Namable::Named::s_namedList.erase(name);
}

Namable::Named * Namable::Named::find(unsigned name) {
	Namable::Named::NamedList::const_iterator it = Namable::Named::s_namedList.find(name);
	if (it != Namable::Named::s_namedList.end()) return it->second;
	return NULL;
}
