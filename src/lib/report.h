#ifndef _LIB_REPORT_H__
#define _LIB_REPORT_H__

#include <string>

namespace Solver {

class Report {
public:
	Report();
	virtual ~Report();

	void cellProperty(unsigned key, std::string property, double value);
	void cellProperty(unsigned key, std::string property, int value);
	void pathProperty(unsigned key, std::string property, double value);
};

}

#endif

