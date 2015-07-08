#ifndef _LIB_REPORT_H__
#define _LIB_REPORT_H__

#include <map>
#include <vector>
#include <string>
#include <stdio.h>

namespace Solver {

class ReportItem {
public:
	ReportItem(unsigned key): m_key(key) {}
	void property(std::string property, double value) { m_properties[property] = value; }
	void save(FILE * f) const;

protected:
	std::map<std::string, double> m_properties;
	unsigned m_key;
};

class Report {
public:
	Report() {}
	virtual ~Report() {}

	void cellProperty(unsigned key, std::string property, double value);
	void cellProperty(unsigned key, std::string property, int value);
	void pathProperty(unsigned key, std::string property, double value);

	void save(std::string filename) const;

protected:
	ReportItem * getItem(std::vector<ReportItem *> & list, unsigned key);
	ReportItem * getCell(unsigned key) { return getItem(m_cells, key); }
	ReportItem * getPath(unsigned key) { return getItem(m_paths, key); }

protected:
	std::vector<ReportItem *> m_cells;
	std::vector<ReportItem *> m_paths;
};

}

#endif

