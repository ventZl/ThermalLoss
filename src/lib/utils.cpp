#ifndef _LIB_UTILS_CPP
#define _LIB_UTILS_CPP

class HeatTransfer {
public:
	HeatTransfer();

	void resistance(double resistance) { assert(resistance > 0.0); m_resistance = resistance; }
	void surface(double surface) { if (surface <= 0) { assert(surface > 0.0); m_surface = surface; }
}

#endif

