#ifndef _XML_HPP_
#define _XML_HPP_

#include<iostream>
#include<string>
#include"util.hpp"
#include"tree.hpp"
#include"unknown.hpp"

namespace xml {
    using namespace std;

    class node;
    typedef tree<node> tree;
};

typedef std::vector<xml::tree> xmls;

std::ostream& operator<<( std::ostream& os, xmls const& xmls );

class xml::node {
private:
	enum type { Element, Text } type;
    map<string,string> attr;
public:
    string value;
    node() {}
    node( char const* str ) : type(Text), value(str) {}
    node( string const& v, enum type t = Text ) : type(t), value(v) {}
    bool is_element() const {
        return type == Element;
    }
    bool is_text() const {
        return type == Text;
    }
    void set_element() {
        type = Element;
    }
    void set_text() {
        type = Text;
    }
    unknown<string> const find_attribute( char const* name ) const;
    unknown<string> find_attribute( char const* name );
    string const get_attribute_default( char const* name, string const& def ) const;
    friend void parse( istream& is, xml::tree& xml );
    friend void parse_xmls( istream& is, xmls& xmls );
    friend ostream& ::operator<<( ostream& os, xmls const& xmls );
};


#endif
