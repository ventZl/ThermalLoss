/*
 *  json.cpp
 *  StarStacker
 *
 *  Created by ventyl on 3/6/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <fstream>
#include <iostream>
#include <string>

#include "json.h"

/** Custom implementation of istream.
 * Provides us with information about currently parsed line content
 * and line number being currently processed.
 */
class parseIstream: public std::ifstream {
public:
	parseIstream(const char * filename): std::ifstream(filename), m_lineReset(false), m_lineNo(1) {}
	std::istream & get(char & c) { if (m_lineReset) { m_lineReset = false; m_lineBuffer = ""; m_lineNo++; } std::ifstream::get(c); m_lineBuffer += c; if (c == '\n' || c == '\r') m_lineReset = true; else m_lineReset = false; return *this; }
	std::istream & unget() { if (!m_lineBuffer.empty()) m_lineBuffer.resize(m_lineBuffer.length() - 1); return std::ifstream::unget(); }
	std::string line() const { return m_lineBuffer; }
	unsigned lineNo() const { return m_lineNo; }

protected:
	std::string m_lineBuffer;
	bool m_lineReset;
	unsigned m_lineNo;
};

typedef std::multimap<std::string, JSON::Reference *> FwdReferenceMap;
typedef std::map<std::string, JSON::Node *> RevReferenceMap;

/** Allows resolution of cross-references.
 */
class ReferenceManager {
public:
	static void addReference(const std::string & id, JSON::Reference * __reference);
	static void addBackReference(const std::string & id, JSON::Node * __node);

protected:
	static FwdReferenceMap s_forwardReference; // names and references waiting for their pointers
	static RevReferenceMap s_backReference; // names and nodes holding them
};

FwdReferenceMap ReferenceManager::s_forwardReference;
RevReferenceMap ReferenceManager::s_backReference;


void ReferenceManager::addReference(const std::string & id, JSON::Reference * __reference) {
	ReferenceManager::s_forwardReference.insert(FwdReferenceMap::value_type(id, __reference));
	RevReferenceMap::iterator it = ReferenceManager::s_backReference.find(id);
	if (it != ReferenceManager::s_backReference.end())
		__reference->setValue(it->second);

	return;
}

void ReferenceManager::addBackReference(const std::string & id, JSON::Node * __node) {
	ReferenceManager::s_backReference.insert(RevReferenceMap::value_type(id, __node));
	FwdReferenceMap::iterator it = ReferenceManager::s_forwardReference.find(id);
	while (it != ReferenceManager::s_forwardReference.end() && it->first == id) {
		it->second->setValue(__node);
	}
	return;
}

#define NODE_CAST_TO(type, obj)	(*(dynamic_cast<const type *>((obj))))

static bool storeStruct(std::ostream & output, const std::string & name, const JSON::Struct & _struct);
static bool storeArray(std::ostream & output, const std::string & name, const JSON::Array & _array);
static bool storeString(std::ostream & output, const std::string & name, const JSON::String & _string);
static bool storeNumber(std::ostream & output, const std::string & name, const JSON::Number & _number);
static bool storeReference(std::ostream & output, const std::string & name, const JSON::Reference & _reference);

#define WRITE_NAME_IF_NOT_EMPTY(name, output) if (name != "") output << " \"" << name << "\" :"

typedef bool (*traverseCallback)(void * cbData, JSON::Node & __traverseNode);

/*
static bool traverseStruct(JSON::Struct & __struct, void * cbData, traverseCallback cbFunc) {
	
}*/

/** Traverse JSON tree.
 * @return true if callback ordered to stop traversal somewhere.
 */
/*
static bool traverse(JSON::Node & root, void * cbData, traverseCallback cbFunc) {
	switch(root.getType()) {
		case JSON::STRUCT: if (!traverseStruct(NODE_CAST_TO(JSON::Struct, root), cbData, cbFunc)) return true; break;
		case JSON::ARRAY: if (!traverseArray(NODE_CAST_TO(JSON::Array, root), cbData, cbFunc)) return true; break;
// these below should probably directly fire cbFunc?
		case JSON::STRING: if (!traverseStruct(NODE_CAST_TO(JSON::String, root), cbData, cbFunc)) return true; break;
		case JSON::NUMBER: if (!traverseStruct(NODE_CAST_TO(JSON::Number, root), cbData, cbFunc)) return true; break;
		case JSON::REFERENCE: if (!traverseReference(NODE_CAST_TO(JSON::Reference, root), cbData, cbFunc)) return true; break;
	}
}*/

static bool storeStruct(std::ostream & output, const std::string & name, const JSON::Struct & __struct) {
	WRITE_NAME_IF_NOT_EMPTY(name, output);
	output << " {";
	for (std::map<std::string, JSON::Node *>::const_iterator it = __struct.begin(); it != __struct.end(); ++it) {
		if (it != __struct.begin()) output << ",";
		switch (it->second->getType()) {
			case JSON::STRUCT: if (!storeStruct(output, it->first, NODE_CAST_TO(JSON::Struct, it->second))) return false; break;
			case JSON::ARRAY: if (!storeArray(output, it->first, NODE_CAST_TO(JSON::Array, it->second))) return false; break;
			case JSON::STRING: if (!storeString(output, it->first, NODE_CAST_TO(JSON::String, it->second))) return false; break;
			case JSON::NUMBER: if (!storeNumber(output, it->first, NODE_CAST_TO(JSON::Number, it->second))) return false; break;
			case JSON::REFERENCE: if (!storeReference(output, it->first, NODE_CAST_TO(JSON::Reference, it->second))) return false; break;
		}
	}
	output << " }";
	output.flush();
	return true;
}

static bool storeArray(std::ostream & output, const std::string & name, const JSON::Array & _array) {
	WRITE_NAME_IF_NOT_EMPTY(name, output);
	output << " [";
	for (JSON::Array::iterator it = _array.begin(); it != _array.end(); ++it) {
		if (it != _array.begin()) output << ",";
		switch ((*it)->getType()) {
			case JSON::STRUCT: {
				if (!storeStruct(output, "", NODE_CAST_TO(JSON::Struct, (*it)))) return false; break;
			}
			case JSON::ARRAY: if (!storeArray(output, "", NODE_CAST_TO(JSON::Array, (*it)))) return false; break;
			case JSON::STRING: if (!storeString(output, "", NODE_CAST_TO(JSON::String, *it))) return false; break;
			case JSON::NUMBER: if (!storeNumber(output, "", NODE_CAST_TO(JSON::Number, *it))) return false; break;
			case JSON::REFERENCE: if (!storeReference(output, "", NODE_CAST_TO(JSON::Reference, *it))) return false; break;
		}
	}
	output << " ]";
	output.flush();
	return true;
}

static bool storeString(std::ostream & output, const std::string & name, const JSON::String & _string) {
	WRITE_NAME_IF_NOT_EMPTY(name, output);
	output << " \"" << _string.getValue() << "\"";
	output.flush();
	return true;
}

static bool storeNumber(std::ostream & output, const std::string & name, const JSON::Number & _number) {
	WRITE_NAME_IF_NOT_EMPTY(name, output);
	output << " " << _number.getValue();
	output.flush();
	return true;
}

static bool storeReference(std::ostream & output, const std::string & name, const JSON::Reference & _reference) {
	WRITE_NAME_IF_NOT_EMPTY(name, output);
	char tmpPtrStr[10];
	snprintf(tmpPtrStr, sizeof(tmpPtrStr), "%p", (void *) _reference.getValue());
	output << " @" << tmpPtrStr;
	output.flush();
	return true;
}


JSON::Node * JSON::Struct::findProperty(const std::string & name) {
	std::map<std::string, JSON::Node *>::iterator it = this->properties.find(name);
	if (it != this->properties.end()) return it->second;
	printf("WARNING: Property `%s` not found!\n", name.c_str());
	return NULL;
}

JSON::Array * JSON::Struct::createArray(const std::string & name) {
	return dynamic_cast<JSON::Array *>(this->findProperty(name));
}

JSON::Struct * JSON::Struct::createStruct(const std::string & name) {
	return dynamic_cast<JSON::Struct *>(this->findProperty(name));
}

JSON::Number * JSON::Struct::createNumber(const std::string & name) {
	return dynamic_cast<JSON::Number *>(this->findProperty(name));
}

JSON::String * JSON::Struct::createString(const std::string & name) {
	return dynamic_cast<JSON::String *>(this->findProperty(name));
}

JSON::Reference * JSON::Struct::createReference(const std::string & name) {
	return dynamic_cast<JSON::Reference *>(this->findProperty(name));
}

JSON::Array * JSON::Array::createArray() {
	return dynamic_cast<JSON::Array *>(this->createItem());
}

JSON::Struct * JSON::Array::createStruct() {
	return dynamic_cast<JSON::Struct *>(this->createItem());
}

JSON::Number * JSON::Array::createNumber() {
	return dynamic_cast<JSON::Number *>(this->createItem());
}

JSON::String * JSON::Array::createString() {
	return dynamic_cast<JSON::String *>(this->createItem());
}

JSON::Reference * JSON::Array::createReference() {
	return dynamic_cast<JSON::Reference *>(this->createItem());
}

bool JSON::store(const std::string & filename, const JSON::Struct * root) {
	std::ofstream output(filename.c_str());
	if (!output.is_open()) {
		return false;
	}
	if (!storeStruct(output, std::string(""), *root)) {
		output.close();
		return false;
	}
	output << std::endl;
	output.close();
	return true;
}

void parseError(parseIstream & stream, std::string errmsg) {
	try {
		parseIstream & parseStream = dynamic_cast<parseIstream &>(stream);
		std::string lastLine = parseStream.line();
		std::string outLine;
		for (std::string::const_iterator it = lastLine.begin(); it != lastLine.end(); ++it) {
			if (*it == '\t') {
				outLine += "        ";
			} else {
				outLine += *it;
			}
		}
		std::string restOfLine;
		getline(stream, restOfLine);
		std::string marker;
		for (unsigned q = 0; q < outLine.length() - 1; q++) marker += '-';
		marker += '^';
		printf("Parse error on line %d: %s\n%s%s\n%s\n", parseStream.lineNo(), errmsg.c_str(), outLine.c_str(), restOfLine.c_str(), marker.c_str());
	} catch (std::bad_cast & e) {
		printf("Parse error: %s\n", errmsg.c_str());
		abort();
	}
	exit(1);
}

char parseWhitespace(parseIstream & input) {
	char c;
	unsigned comment = 0;
	do {
		input.get(c);
		if (c == '/') {
			comment++;
			c = ' ';
		} else {
			// this is not beginning of the comment thus unget last char and return /
			if (comment == 1) {
				input.unget();
				return '/';
			}
		}
		if (comment > 1) {
			if (c != '\n') c = ' '; // let everything inside comment until newline be whitespace
		}
	} while (c == ' ' || c == '\t' || c == '\n');
	return c;
}

void parseArray(parseIstream & input, JSON::Array & __array);
void parseString(parseIstream & input, JSON::String & __string);
void parseNumber(parseIstream & input, JSON::Number & __number);
void parseStruct(parseIstream & input, JSON::Struct & __struct);
void parseReference(parseIstream & input, JSON::Reference & __reference);

void parseArray(parseIstream & input, JSON::Array & __array) {
	char c;
	c = parseWhitespace(input);
//	printf("> %c >", c);
	if ( c == '[') {
		while (1) {
			c = parseWhitespace(input);
//			do { input.get(c); } while (c == ' ' || c == '\t' || c == '\n');
//			printf("> %c >", c);
			if (c == ']') break;				// for allowing of empty arrays
			input.unget();
			if (c == '@') {
				JSON::Reference * new_reference = __array.createReference();
				if (new_reference == NULL) parseError(input, "Schema doesn't allow reference to be item of this array");
				parseReference(input, *new_reference);

			} else if (c == '{') {
//				printf("\nParsing struct item...\n"); 
				JSON::Struct * new_struct = __array.createStruct();
				if (new_struct == NULL) parseError(input, "Schema doesn't allow struct to be item of this array");
				parseStruct(input, *new_struct);
//				printf("\nEnd of parse...\n");
			} else if (c == '[') {
//				printf("\nParsing array item...\n"); 
				JSON::Array * new_array = __array.createArray();
				if (new_array == NULL) parseError(input, "Schema doesn't allow array to be item of this array");
				parseArray(input, *new_array);
//				printf("\nEnd of parse...\n");
			} else if (c == '"') {
//				printf("\nParsing string item...\n"); 
				JSON::String * new_string = __array.createString();
				if (new_string == NULL) parseError(input, "Schema doesn't allow string to be item of this array");
				parseString(input, *new_string);
//				printf("\nEnd of parse...\n");
			} else if ((c >= '0' && c <= '9') || (c == '.') || (c == '-')) {
//				printf("\nParsing number item...\n"); 
				JSON::Number * new_number = __array.createNumber();
				if (new_number == NULL) parseError(input, "Schema doesn't allow number to be item of this array");
				parseNumber(input, *new_number);
//				printf("\nEnd of parse...\n");
			} else {
				parseError(input, "Misformatted input file. Unidentified character inside array definition!");
			}
			c = parseWhitespace(input);
//			do { input.get(c); } while (c == ' ' || c == '\t' || c == '\n');
//			printf("> %c >", c);
			if (c == ']') break;
			if (c != ',') { parseError(input, "Misformatted array record. Expecting , or }!"); }
		}	
	} else parseError(input, "Internal error: expected array record, input contains something else");
	std::string message;
	if (!__array.validate(message)) { parseError(input, message); }
}

void parseReference(parseIstream & input, JSON::Reference & __reference) {
	char c;
	std::string value;
	c = parseWhitespace(input);
	bool isWhiteSpace = false;
	do {
		input.get(c);
		if (c == ' ' || c == ',' || c == ']' || c == '}') isWhiteSpace = true;
		if (!isWhiteSpace) value += c;
	} while (!isWhiteSpace);
	ReferenceManager::addReference(value, &__reference);
	return;
}

void parseString(parseIstream & input, JSON::String & __string) {
	char c;
	std::string value;
	c = parseWhitespace(input);
//	do { input.get(c);	} while (c == ' ' || c == '\t' || c == '\n');
	if (c != '"') { parseError(input, "BLA string \""); }
	do {
		input.get(	c);
//		printf("> %c >", c);
		if (c != '"') value += c;
	} while (c != '"');
	__string.setValue(value);
	std::string message;
	if (!__string.validate(message)) { parseError(input, message); } //"Schema does not allow string content \"" + __string.getValue() + "\" here!"); }
	return;
		
}

void parseNumber(parseIstream & input, JSON::Number & __number) {
	char c;
	int sign = 1;
	int integral = 0;
	int fractional = 0;
	int f_size = 0;
	do {
		input.get(c);
		if (c >= '0' && c <= '9') integral = integral * 10 + (c - '0');
		else if (c == '-' && sign == 1) sign *= -1;
		else break;
	} while (1);
	if (c == '.') {
		do {
			input.get(c);
			if (c >= '0' && c <= '9') { fractional = fractional * 10 + (c - '0'); f_size = (f_size == 0 ? 10 : f_size * 10); }
			else break;
		} while (1);
		input.unget();
	} else input.unget();
	float retNum;
	if (f_size == 0) retNum = (float) sign * (float) integral;
	else  retNum = (float) sign * ((float) integral + ((float) fractional / (float) f_size));
	__number.setValue(retNum);
	std::string message;
	if (!__number.validate(message)) { parseError(input, message); } //"Number does not validate against schema!"); }
//	printf("Parsed number is %f\n", retNum);
	return;
}

void parseStruct(parseIstream & input, JSON::Struct & __struct) {
//	printf("\nENTERING PARSE_STRUCT\n");
	char c;
	c = parseWhitespace(input);
//	do { input.get(c); } while (c == ' ' || c == '\t' || c == '\n');
	if ( c == '{') {
		while (1) {
			std::string property_name;
			c = parseWhitespace(input);
//			do { input.get(c); } while (c == ' ' || c == '\t' || c == '\n');
//			printf("> %c >", c);
			if ( c == '}') break;
			else if (c == '"') {
				do {
					input.get(c);
					if (c != '"') property_name += c;
//					printf("> %c >", c);
				} while (c != '"');
				c = parseWhitespace(input);
//				do { input.get(c); } while (c == ' ' || c == '\t' || c == '\n');
//				printf("> %c >", c);
				if (c != ':') { parseError(input, "Misformatted input file. Missing colon after property name `" + property_name + "`!"); }
				c = parseWhitespace(input);
//				do { input.get(c); } while (c == ' ' || c == '\t' || c == '\n');
//				printf("> %c >", c);
				input.unget();
				if (c == '{') {
//					printf("\nParsing nested struct \"%s\"...\n", property_name.c_str()); 
					JSON::Struct * new_struct = __struct.createStruct(property_name);
					if (new_struct == NULL) parseError(input, std::string("Schema doesn't allow struct property '" + property_name + "' here"));
					parseStruct(input, *new_struct);
//					printf("\nEnd of nested parse...\n");
				} else if (c == '[') {
//					printf("\nParsing nested array \"%s\"...\n", property_name.c_str()); 
					JSON::Array * new_array = __struct.createArray(property_name);
					if (new_array == NULL) parseError(input, std::string("Schema doesn't allow array property '" + property_name + "' here"));
					parseArray(input, *new_array);
//					printf("\nEnd of nested parse...\n");
				} else if (c == '"') {
//					printf("\nParsing nested string \"%s\"...\n", property_name.c_str()); 
					JSON::String * new_string = __struct.createString(property_name);
					if (new_string == NULL) parseError(input, std::string("Schema doesn't allow string property '" + property_name + "' here"));
					parseString(input, *new_string);
//					printf("\nEnd of nested parse...\n");
				} else if ((c >= '0' && c <= '9') || (c == '.') || ( c == '-')) {
//					printf("\nParsing nested number \"%s\"...\n", property_name.c_str()); 
					JSON::Number * new_number = __struct.createNumber(property_name);
					if (new_number == NULL) parseError(input, std::string("Schema doesn't allow numeric property '" + property_name + "' here"));
					parseNumber(input, *new_number);
//					printf("\nEnd of nested parse...\n");
				} else {
					parseError(input, "Misformatted input file. Unidentified character after semicolon in property definition!");
				}
				c = parseWhitespace(input);
//				do { input.get(c); } while (c == ' ' || c == '\t' || c == '\n');
//				printf("> %c >", c);
				if (c == '}') break;
				if (c != ',') { parseError(input, "Misformatted struct record. Expecting , or }!"); }
			}
		}
	} else parseError(input, "Misformatted struct record. Expecting { !");
	std::string message;
	if (!__struct.validate(message)) { parseError(input, message); } //"Struct does not respect schema!\n"); }
}

bool JSON::parse(const std::string & filename, JSON::Struct * root) {
	parseIstream input(filename.c_str());
	if (!input.is_open()) {
		return false;
	}
	parseStruct(input, *root);

	input.close();
	return true;
}

