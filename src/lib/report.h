#ifndef _LIB_REPORT_H__
#define _LIB_REPORT_H__

#include <map>
#include <vector>
#include <string>
#include <stdio.h>

namespace Solver {

/** Mesh entity to which properties are related
 */
class ReportItem {
public:
	/**
	 * @param key user-set identifier of entity
	 */
	ReportItem(unsigned key): m_key(key) {}

	/** Adds new property to report
	 * @param property name of property
	 * @param value value of property
	 */
	void property(std::string property, double value) { m_properties[property] = value; }

	/** Q&D way to push log on to the disk
	 * @param f file descriptor to save report to
	 */
	void save(FILE * f) const;

protected:
	std::map<std::string, double> m_properties;
	unsigned m_key;
};

/** Main report container.
 */
class Report {
public:
	Report() {}
	virtual ~Report() {}

	/** Add property to cell.
	 * @param key external ID of cell
	 * @param property name of property
	 * @param value property value
	 */
	void cellProperty(unsigned key, std::string property, double value);
	void cellProperty(unsigned key, std::string property, int value);
	/** Add property to path.
	 * @param key external ID of path
	 * @param property name of property
	 * @param value property value
	 */
	void pathProperty(unsigned key, std::string property, double value);

	void save(std::string filename) const;

protected:
	/** Get (or create) item from list by it's ID
	 * @param list list of items
	 * @param key external ID of item
	 * @note if no item with such ID is found, new is created
	 * @return item
	 */
	ReportItem * getItem(std::vector<ReportItem *> & list, unsigned key);
	ReportItem * getCell(unsigned key) { return getItem(m_cells, key); }
	ReportItem * getPath(unsigned key) { return getItem(m_paths, key); }

protected:
	std::vector<ReportItem *> m_cells;
	std::vector<ReportItem *> m_paths;
};

}

#endif

