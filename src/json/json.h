/*
 *  json.h
 *  StarStacker
 *
 *  Created by ventyl on 3/6/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __JSON_OBJECT_H__
#define __JSON_OBJECT_H__

#include <stdexcept>
#include <string>
#include <map>

/** The proxy is used for storing items of nested arrays into owning container. Decreases scattering of model. */
#define JSON_ARRAY_PROXY(proxy_name, proxy_type, variable, item_type) \
	class proxy_name: public JSON::Array { \
	public:\
		proxy_name(proxy_type * target): m_target(target) {}\
		virtual JSON::Node * createItem() { int lastOffset = m_target->variable.size(); m_target->variable.push_back(new item_type()); return m_target->variable[lastOffset]; }\
		virtual JSON::Node * getItemAt(unsigned int offset) const { return m_target->variable[offset]; }\
		virtual unsigned int getItemCount() const { return m_target->variable.size(); }\
\
	protected:\
		proxy_type * m_target;\
	};\
\
	friend class proxy_name

namespace JSON {
	typedef enum { STRING, NUMBER, STRUCT, ARRAY } Type;
	
	class Node;
	class String;
	class Struct;
	class Array;
	class Number;
	
	class Node {
	public:
		Node() {}
		virtual ~Node() {}
/*		virtual Array * createArray(const std::string & name) { return NULL; }
		virtual Struct * createStruct(const std::string & name) { return NULL; }
		virtual String * createString(const std::string & name) { return NULL; }
		virtual Number * createNumber(const std::string & name) { return NULL; }*/
		virtual Type getType() const = 0;
		virtual bool validate() { return true; }
		
	protected:
//		std::string name;
	};
	
	template <class T> class iterator {
	public:
		iterator & operator++() { position++; return *this; }
		iterator & operator--() { position--; return *this; }
		bool operator==(iterator other) { if (this->position == other.position) return true; return false; }
		bool operator!=(iterator other) { return !(operator==(other)); }
		const Node * operator*() const { return this->iterable->getItemAt(this->position); }
	protected:
		iterator(T iterable, unsigned position): iterable(iterable), position(position) {}
		T iterable;
		unsigned position;
		friend class Struct;
		friend class Array;
	};
	
	class String: public Node {
	public:
		String(): Node() {}
		virtual Type getType() const { return STRING; }
		virtual const std::string & getValue() const = 0;
		virtual void setValue(const std::string & value) = 0;
	};
	
	class Number: public Node {
	public:
		Number(): Node() {}
		virtual Type getType() const { return NUMBER; }
		virtual float getValue() const = 0;
		virtual void setValue(float value) = 0;
	};
	
	class Array: public Node {
	public:
		Array(): Node() {}
		
		typedef JSON::iterator<Array *> iterator;
		virtual Type getType() const { return ARRAY; }
		virtual iterator begin() const { iterator it((JSON::Array *) this, 0); return it; }
		virtual iterator end() const { iterator it((JSON::Array *) this, this->getItemCount()); return it; }

		virtual Array * createArray();
		virtual Struct * createStruct();
		virtual String * createString();
		virtual Number * createNumber();

		virtual Node * createItem() = 0;
		virtual const Node * getItemAt(unsigned offset) const = 0;
		virtual unsigned getItemCount() const = 0;
		
		friend class JSON::iterator<Array *>;
	};
	
	class Struct: public Node {
	public:
		Struct(): Node() {}
		
		typedef std::map<std::string, JSON::Node *> property_container;
		
		virtual Type getType() const { return STRUCT; }
		virtual property_container::const_iterator begin() const { return this->properties.begin(); }
		virtual property_container::const_iterator end() const { return this->properties.end(); }

		virtual Node * findProperty(const std::string & name);
		
		virtual Array * createArray(const std::string & name);
		virtual Struct * createStruct(const std::string & name);
		virtual String * createString(const std::string & name);
		virtual Number * createNumber(const std::string & name);
				
	protected:
		void addProperty(const std::string & name, JSON::Node * nd) { properties.insert(std::pair<std::string, JSON::Node *>(name, nd)); }
		
		property_container properties;
	};
	
	namespace Simple {
		class Int: public JSON::Number {
		public:
			Int(): Number(), m_holder(new int), m_buffer(m_holder.get()) {}
			Int(int * buffer): Number(), m_holder(NULL), m_buffer(buffer) {}
			virtual float getValue() const { return *m_buffer; }
			virtual void setValue(float value) { *(m_buffer) = (int) value; }
			
		protected:
			std::auto_ptr<int> m_holder;
			int * m_buffer;
		};

		class Unsigned: public JSON::Number {
		public:
			Unsigned(): Number(), m_holder(new unsigned int), m_buffer(m_holder.get()) {}
			Unsigned(unsigned int * buffer): Number(), m_holder(NULL), m_buffer(buffer) {}
			virtual float getValue() const { return *m_buffer; }
			virtual void setValue(float value) { *(m_buffer) = (unsigned) value; }
			
		protected:
			std::auto_ptr<unsigned int> m_holder;
			unsigned int * m_buffer;
		};

		class Float: public JSON::Number {
		public:
			Float(): Number(), m_holder(new float), m_buffer(m_holder.get()) {}
			Float(float * buffer): Number(), m_buffer(buffer) {}
			virtual float getValue() const { return *m_buffer; }
			virtual void setValue(float value) { *(m_buffer) = value; }
			
		protected:
			std::auto_ptr<float> m_holder;
			float * m_buffer;
		};

		class Double: public JSON::Number {
		public:
			Double(): Number(), m_holder(new double), m_buffer(m_holder.get()) {}
			Double(double * buffer): Number(), m_buffer(buffer) {}
			virtual float getValue() const { return *m_buffer; }
			virtual void setValue(float value) { *(m_buffer) = (double) value; }
			
		protected:
			std::auto_ptr<double> m_holder;
			double * m_buffer;
		};
		
		class String: public JSON::String {
		public:
			String(): JSON::String(), m_holder(new std::string()), m_buffer(m_holder.get()) {}
			String(std::string * buffer): JSON::String(), m_buffer(buffer) {}
			virtual const std::string & getValue() const { return *m_buffer; }
			virtual void setValue(const std::string & value) { *(m_buffer) = value; }
			
		protected:
			std::auto_ptr<std::string> m_holder;
			std::string * m_buffer;
		};
	}

	bool parse(std::string filename, Struct * root);
	bool store(std::string filename, const Struct * root);
}

#endif
