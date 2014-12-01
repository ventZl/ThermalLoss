#ifndef __SRC_MODEL_MATERIAL_H__
#define __SRC_MODEL_MATERIAL_H__

#include <vector>
#include <string>
#include <json/json.h>

namespace Model {

/** @file Data model used for describing computation model. */

/** Definition of material from which wall is composed */
class Material: public JSON::Struct {
public:
	Material(): json_name(&this->m_name), json_conductivity(&this->m_conductivity), json_density(&this->m_density), m_conductivity(), m_density(0) { addProperties(); }
	std::string name() const { return m_name; }
	float conductivity() const { return m_conductivity; }
	float density() const { return m_density; }
	bool validate();

protected:
	void addProperties() {
		addProperty("name", &json_name); addProperty("conductivity", &json_conductivity); addProperty("density", &json_density);
	}

	JSON::Simple::String json_name;
	JSON::Simple::Float json_conductivity;
	JSON::Simple::Float json_density;
	std::string m_name;
	float m_conductivity;
	float m_density;
};

/** Ordered list of materials */
typedef std::vector<Material *> MaterialVector; 
typedef std::map<std::string, Material *> NameMaterialMap;

/** Usage of defined material. References material by it's name and defines width of layer of given material */
class MaterialUsage: public JSON::Struct {
public:
	MaterialUsage(): json_width(&this->m_width), json_material(&this->m_material) { addProperties(); }
	std::string material() const { return m_material; }
	double width() const { return m_width; }

protected:
	void addProperties() {
		addProperty("material", &json_material); 
		addProperty("width", &json_width);
	}

	JSON::Simple::Float json_width;
	JSON::Simple::String json_material;
	std::string m_material;
	float m_width;
};

/** Ordered list of material usages */
typedef std::vector<MaterialUsage *> MaterialUsageVector;

/** Top-level object covering whole material library. Contains list of available material definitions */
class MaterialLibrary: public JSON::Struct {
public:
	JSON_ARRAY_PROXY(MaterialArrayProxy, MaterialLibrary, m_materials, Material);

	MaterialLibrary(): json_materials(this) { addProperties(); }
	bool validate();
	const Material * material(std::string & name) const;

protected:
	void addProperties() {
		addProperty("materials", &json_materials);
	}

	MaterialArrayProxy json_materials;
	MaterialVector m_materials;
	NameMaterialMap m_materialByName;

//	friend class MaterialLibraryProxy;
};

MaterialLibrary * loadMaterialLibrary(std::string fileName);

}

#endif
