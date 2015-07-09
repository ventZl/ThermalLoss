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
	Parameters(): JSON_ATTACH(outTemp), JSON_ATTACH(roomTemp), JSON_ATTACH(groundTemp), JSON_ATTACH(ceilingTemp) { addProperties(); }

protected:
	void addProperties() { addProperty("outTemp", &json_outTemp); addProperty("roomTemp", &json_roomTemp); addProperty("groundTemp", &json_groundTemp); addProperty("ceilingTemp", &json_ceilingTemp); }

	JSON::Simple::Float json_outTemp;
	JSON::Simple::Float json_roomTemp;
	JSON::Simple::Float json_groundTemp;
	JSON::Simple::Float json_ceilingTemp;

	RO_PROPERTY(float, outTemp);
	RO_PROPERTY(float, roomTemp);
	RO_PROPERTY(float, groundTemp);
	RO_PROPERTY(float, ceilingTemp);
};

class RoomLosses: public JSON::Struct {
public:
	RoomLosses(): JSON_ATTACH(name), JSON_ATTACH(losses), m_losses(0) {	addProperties(); }

protected:
	void addProperties() { JSON_PROPERTY(name); JSON_PROPERTY(losses); }

	JSON::Simple::String json_name;
	std::string m_name;

	JSON::Simple::Float json_losses;
	RO_PROPERTY(float, losses);

public:
	void addLoss(double loss) { m_losses += loss; }
};

class Losses: public JSON::Struct {
public:
	Losses(): JSON_ATTACH(wallLosses), JSON_ATTACH(ceilLosses), JSON_ATTACH(windowLosses), JSON_ATTACH(floorLosses), JSON_ATTACH(area), JSON_ATTACH(energyRequired), JSON_ATTACH(relativeLosses), JSON_ATTACH(totalLosses) { addProperties(); m_wallLosses = 0; m_ceilLosses = 0; m_windowLosses = 0; m_floorLosses = 0; m_area = 0; m_relativeLosses = 0; m_totalLosses = 0; }

protected:
	void addProperties() { JSON_PROPERTY(wallLosses); JSON_PROPERTY(ceilLosses); JSON_PROPERTY(windowLosses); JSON_PROPERTY(floorLosses); JSON_PROPERTY(area); JSON_PROPERTY(energyRequired); JSON_PROPERTY(relativeLosses), JSON_PROPERTY(totalLosses); }

	JSON::Simple::Float json_wallLosses;
	JSON::Simple::Float json_ceilLosses;
	JSON::Simple::Float json_windowLosses;
	JSON::Simple::Float json_floorLosses;
	JSON::Simple::Float json_area;
	JSON::Simple::Float json_energyRequired;
	JSON::Simple::Float json_relativeLosses;
	JSON::Simple::Float json_totalLosses;
	
	RO_PROPERTY(float, wallLosses);
	RO_PROPERTY(float, ceilLosses);
	RO_PROPERTY(float, windowLosses);
	RO_PROPERTY(float, floorLosses);
	RO_PROPERTY(float, area);
	PROPERTY(float, energyRequired);
	RO_PROPERTY(float, relativeLosses);
	RO_PROPERTY(float, totalLosses);

public:
	void addWallLoss(unsigned room, double loss) { m_wallLosses += loss; m_totalLosses += loss; }
	void addCeilLoss(unsigned room, double loss) { m_ceilLosses += loss; m_totalLosses += loss; }
	void addWindowLoss(unsigned room, double loss) { m_windowLosses += loss; m_totalLosses += loss; }
	void addFloorLoss(unsigned room, double loss) { m_floorLosses += loss; m_totalLosses += loss; }
	void addArea(double area) { m_area += area; }
	void addRelativeLoss(double loss) { m_relativeLosses += loss; }
};

}

#endif
