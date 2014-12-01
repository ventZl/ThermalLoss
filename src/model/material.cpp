#include "material.h"

bool Model::Material::validate() {
	if (m_conductivity != 0) return true;
	return false;
}

/** Validates material library.
 * For now validation basically consists of creating name:material pair and ensuring that each material name is unique
 */
bool Model::MaterialLibrary::validate() {
	for (Model::MaterialVector::const_iterator it = m_materials.begin(); it != m_materials.end(); ++it) {
		if (m_materialByName.find((*it)->name()) != m_materialByName.end()) return false;
		m_materialByName.insert(Model::NameMaterialMap::value_type((*it)->name(), *it));
	}
	return true;
}

const Model::Material * Model::MaterialLibrary::material(std::string & name) const {
	Model::NameMaterialMap::const_iterator it = m_materialByName.find(name);
	if (it != m_materialByName.end()) return it->second;
	return NULL;
}

Model::MaterialLibrary * Model::loadMaterialLibrary(std::string fileName) {
	Model::MaterialLibrary * ml = new Model::MaterialLibrary();
	if (parse(fileName, ml)) {
	} else {
		delete ml;
		ml = NULL;
	}
	return ml;
}


