#ifndef _XML_HPP_
#define _XML_HPP_

#include<iostream>
#include<string>
#include"util.hpp"
#include "tree.hpp"

struct node {
	enum type { Element, Text } type;
	std::string value;
	std::map<std::string, std::string> attr;
    node() {}
    node( char const* str ) : type(Text), value(str) {}
    node( std::string const& v, enum type t = Text ) : type(t), value(v) {}
};

typedef tree<node> xml;
typedef std::vector<xml> xmls;

void parse_xml( std::istream& is, xmls& xmls );
std::ostream& operator<<( std::ostream& os, xmls const& xmls );

#endif
