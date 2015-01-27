#ifndef __SRC_MODEL_LOSSES_H__
#define __SRC_MODEL_LOSSES_H__

#include <json/json.h>
#include <../include/property.h>

#define JSON_ATTACH(name) json_ ## name(&m_ ## name)
#define JSON_AUX_PROPERTY(name) # name
#define JSON_PROPERTY(name) addProperty(JSON_AUX_PROPERTY(name), &json_ ## name)

namespace Model {

class Parameters: public JSON::Struct {
public:
	Parameters(): JSON_ATTACH(outTemp), JSON_ATTACH(roomTemp), JSON_ATTACH(groundTemp) { addProperties(); }

protected:
	void addProperties() { addProperty("outTemp", &json_outTemp); addProperty("roomTemp", &json_roomTemp); addProperty("groundTemp", &json_groundTemp); }

	JSON::Simple::Float json_outTemp;
	JSON::Simple::Float json_roomTemp;
	JSON::Simple::Float json_groundTemp;

	RO_PROPERTY(float, outTemp);
	RO_PROPERTY(float, roomTemp);
	RO_PROPERTY(float, groundTemp);
};

class Losses: public JSON::Struct {
public:
	Losses(): JSON_ATTACH(wallLosses), JSON_ATTACH(ceilLosses), JSON_ATTACH(windowLosses), JSON_ATTACH(floorLosses), JSON_ATTACH(area), JSON_ATTACH(energyRequired), JSON_ATTACH(relativeLosses) { addProperties(); m_wallLosses = 0; m_ceilLosses = 0; m_windowLosses = 0; m_floorLosses = 0; m_area = 0; m_relativeLosses = 0; }

protected:
	void addProperties() { JSON_PROPERTY(wallLosses); JSON_PROPERTY(ceilLosses); JSON_PROPERTY(windowLosses); JSON_PROPERTY(floorLosses); JSON_PROPERTY(area); JSON_PROPERTY(energyRequired); JSON_PROPERTY(relativeLosses); }

	JSON::Simple::Float json_wallLosses;
	JSON::Simple::Float json_ceilLosses;
	JSON::Simple::Float json_windowLosses;
	JSON::Simple::Float json_floorLosses;
	JSON::Simple::Float json_area;
	JSON::Simple::Float json_energyRequired;
	JSON::Simple::Float json_relativeLosses;
	
	RO_PROPERTY(float, wallLosses);
	RO_PROPERTY(float, ceilLosses);
	RO_PROPERTY(float, windowLosses);
	RO_PROPERTY(float, floorLosses);
	RO_PROPERTY(float, area);
	PROPERTY(float, energyRequired);
	RO_PROPERTY(float, relativeLosses);

public:
	void addWallLoss(double loss) { m_wallLosses += loss; }
	void addCeilLoss(double loss) { m_ceilLosses += loss; }
	void addWindowLoss(double loss) { m_windowLosses += loss; }
	void addFloorLoss(double loss) { m_floorLosses += loss; }
	void addArea(double area) { m_area += area; }
	void addRelativeLoss(double loss) { m_relativeLosses += loss; }
};

}

#endif
