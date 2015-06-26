#ifndef _LIB_UTILS_CPP
#define _LIB_UTILS_CPP

namespace Thermal {

class Resistance;

class Conductivity {
public:
	explicit Conductivity(double conductivity);
	operator Resistance() const;
	operator double() const;

protected:
	double m_conductivity;
};

class Resistance {
public:
	explicit Resistance(double resistance);
	operator Conductivity() const;
	operator double() const;

protected:
	double m_resistance;
};

class Flow {
public:
	explicit Flow(double temp1, double temp2, const Resistance & resistance);

	void temp1(double temp) { m_temp1 = temp1; }
	void temp2(double temp) { m_temp2 = temp2; }
	operator double() const;

protected:
	Resistance & m_resistance;
	double m_temp1;
	double m_temp2;
};

class Power {
public:
	explicit Power(double area, const Flow & flow)
};

}

#endif

