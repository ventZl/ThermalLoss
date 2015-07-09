#include "report.h"

std::string Solver::ReportItem::Property::value() const {
	char buffer[32];
	if (m_type == Solver::ReportItem::Property::NONE) {
		return "(unset)";
	} else if (m_type == Solver::ReportItem::Property::DOUBLE) {
		snprintf(buffer, sizeof(buffer), "%.3f", m_double);
		return buffer;
	} else if (m_type == Solver::ReportItem::Property::INT) {
		snprintf(buffer, sizeof(buffer), "%d", m_int);
		return buffer;
	} else if (m_type == Solver::ReportItem::Property::BOOL) {
		if (m_bool) return "true"; else return "false";
	} else {
		return "\"" + m_string + "\"";
	}
}

void Solver::ReportItem::save(FILE * f) const {
	fprintf(f, "(%d): ", m_key);
	bool first = true;
	for (std::map<std::string, Property>::const_iterator it = m_properties.begin(); it != m_properties.end(); ++it) {
		if (!first) fprintf(f, ", ");
		fprintf(f, "%s = %s", it->first.c_str(), it->second.value().c_str());
		if (first) first = false;
	}
	fprintf(f, "\n");
}

void Solver::Report::cellProperty(unsigned key, std::string property, double value) {
	getCell(key)->property(property, value);
}

void Solver::Report::cellProperty(unsigned key, std::string property, int value) {
	getCell(key)->property(property, value);
}

void Solver::Report::cellProperty(unsigned key, std::string property, const std::string & value) {
	getCell(key)->property(property, value);
}

void Solver::Report::cellProperty(unsigned key, std::string property, bool value) {
	getCell(key)->property(property, value);
}

void Solver::Report::pathProperty(unsigned key, std::string property, double value) {
	getPath(key)->property(property, value);
}

void Solver::Report::pathProperty(unsigned key, std::string property, int value) {
	getPath(key)->property(property, value);
}

void Solver::Report::pathProperty(unsigned key, std::string property, const std::string & value) {
	getPath(key)->property(property, value);
}

void Solver::Report::pathProperty(unsigned key, std::string property, bool value) {
	getPath(key)->property(property, value);
}


void Solver::Report::save(std::string filename) const {
	FILE * f = fopen(filename.c_str(), "w");
	if (f == NULL) return;
	for (std::vector<ReportItem *>::const_iterator it = m_cells.begin(); it != m_cells.end(); ++it) {
		if (*it == NULL) continue;
		fprintf(f, "cell ");
		(*it)->save(f);
	}
	for (std::vector<ReportItem *>::const_iterator it = m_paths.begin(); it != m_paths.end(); ++it) {
		if (*it == NULL) continue;
		fprintf(f, "path ");
		(*it)->save(f);
	}
	fclose(f);
}

Solver::ReportItem * Solver::Report::getItem(std::vector<Solver::ReportItem *> & list, unsigned key) {
	size_t old_size = list.size();
	if (old_size <= key) {
		list.resize(key + 1);
		for (unsigned q = old_size; q < key + 1; ++q) list[q] = NULL;
	}
	if (list[key] == NULL) list[key] = new ReportItem(key);
	return list[key];

}


