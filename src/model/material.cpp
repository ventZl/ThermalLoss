#include "material.h"

bool Model::Material::validate() {
	if (m_conductivity != 0) return true;
	return false;
}

/** Validates material library.
 * For now validation basically consists of creating name:material pair and ensuring that each material name is unique
 */
bool Model::MaterialLibrary::init() {
	for (Model::MaterialVector::const_iterator it = m_materials.begin(); it != m_materials.end(); ++it) {
		if (m_materialByName.find((*it)->name()) != m_materialByName.end()) return false;
		m_materialByName.insert(Model::NameMaterialMap::value_type((*it)->name(), *it));
	}
	return true;
}

const Model::Material * Model::MaterialLibrary::material(const std::string & name) const {
//	printf("Known materials: %d\n", m_materialByName.size());
	Model::NameMaterialMap::const_iterator it = m_materialByName.find(name);
	if (it != m_materialByName.end()) return it->second;
	fprintf(stderr, "Material '%s' is not defined!\n", name.c_str());
	fprintf(stderr, "Material library summary:\n=========================\nKnown materials: %d\n", m_materialByName.size());
	exit(1);
	return NULL;
}

Model::MaterialLibrary * Model::loadMaterialLibrary(std::string fileName) {
	Model::MaterialLibrary * ml = new Model::MaterialLibrary();
	if (parse(fileName, ml)) {
		if (!ml->init()) {
			delete ml;
			ml = NULL;
		}
	} else {
		delete ml;
		ml = NULL;
	}
	return ml;
}


