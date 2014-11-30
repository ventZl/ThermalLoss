#ifndef __SRC_LOSSES_H__
#define __SRC_LOSSES_H__

class Building;

class Losses {
public:
	Losses(double insideTemp, double outsidetemp, double groundTemp);

	bool compute(Building * building);

	double wallLoss() const { return m_wallLoss; }
	double floorLoss() const { return m_floorLoss; }
	double ceilingLoss() const { return m_wallLoss; }
	double bridgeLoss() const { return m_bridgeLoss; }
	double holeLoss() const { return m_holeLoss; }
	double airLoss() const { return m_airLoss; }

protected:
	double m_insideTemp;	// teplota dnu
	double m_outsideTemp;	// teplota vonku
	double m_groundTemp;	// teplota zeme
	double m_wallLoss;		// steny
	double m_floorLoss;		// podlaha
	double m_ceilingLoss;	// strop
	double m_bridgeLoss;	// tepelne mosty
	double m_holeLoss;		// otvory
	double m_airLoss;		// vetranie
};

#endif

