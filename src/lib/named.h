#ifndef _LIB_NAMED_H__
#define _LIB_NAMED_H__

#include <stdexcept>
#include <map>

namespace Namable {

class Named {
public:
	Named(unsigned name): m_name(name) { if (name == 0) throw std::runtime_error("Invalid name of namable"); Named::add(m_name, this); }
	Named(): m_name(0) {}
	virtual ~Named() { Named::remove(m_name); }

public:
	static Named * find(unsigned m_name);

protected:
	static void add(unsigned m_name, Named * entity);
	static void remove(unsigned m_name);

public:
	void name(unsigned name) { if (m_name == 0) m_name = name; Named::add(m_name, this); }
	unsigned name() const { return m_name; }

protected:
	unsigned m_name;

	typedef std::map<unsigned, Named *> NamedList;

	static NamedList s_namedList;
};

}

#endif

