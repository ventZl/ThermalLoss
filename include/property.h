#ifndef ___PROPERTY_H_
#define ___PROPERTY_H_

#define RO_PROPERTY(type, name) \
public:\
	type name() const { return m_ ## name; }\
\
protected:\
	type m_ ## name

#define PROPERTY(type, name) \
public:\
	void name(type name) { m_ ## name = name; }\
	void name(type & name) { m_ ## name = name; }\
	RO_PROPERTY(type, name)

#endif
