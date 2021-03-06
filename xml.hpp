#ifndef _XML_HPP_
#define _XML_HPP_

#include<iostream>
#include<string>
#include"util.hpp"
#include"tree.hpp"

namespace xml {
    using namespace std;

    class node;
    typedef tree<node> tree;

    void parse_inner( istream& is, tree& xml );
};

typedef std::vector<xml::tree> xmls;

std::ostream& operator<<( std::ostream& os, xmls const& xmls );

std::istream& operator>>( std::istream& is, xml::tree& xml );
std::istream& operator>>( std::istream& is, xmls& xmls );

class xml::node {
private:
	enum type { Element, Text, Special } type;
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
    bool is_special() const {
        return type == Special;
    }
    void set_element() {
        type = Element;
    }
    void set_text() {
        type = Text;
    }
    void set_special() {
        type = Special;
    }
    string const* find_attribute( char const* name ) const;
    string* find_attribute( char const* name );
    string const& get_attribute( char const* name, string const& def ) const;
    friend ostream& ::operator<<( std::ostream& os, xmls const& xmls );
    friend void parse_inner( istream& is, tree& xml );
    friend istream& ::operator>>( std::istream& is, xmls& xmls );
};


#endif
