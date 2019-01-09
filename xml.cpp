#include<iostream>
#include "xml.hpp"

using namespace std;

void ignorespace( istream& is ) {
	while( !is.eof() && isspace(is.peek()) ) {
		is.get();
	}
}

static void parse_attr( istream& is, map<string,string>& attr ) {
	for(;;) {
		ignorespace(is);
		char c = is.peek();
		if( c == EOF ) {
			ERR( "unexpected EOF while parsing tag" );
		}
		if( c == '/' || c == '>' ) {
			return;
		}
		string name;
		string val;
		is.ignore();
		while( c != '=' ) {
			name.push_back(c);
			c = is.get();
			if( c == EOF ) {
				ERR( "unexpected EOF while parsing attribute" );
			}
		}
		c = is.peek();
		if( c == '"' ) {
			is.ignore();
			c = is.get();
			while( c != '"' ) {
				val.push_back(c);
				c = is.get();
				if( c == EOF ) {
					ERR( "unexpected EOF while parsing string" );
				}
			}
		} else {
			while( isalnum(c) ) {
				val.push_back(c);
				is.ignore();
				c = is.peek();
				if( c == EOF ) {
					ERR( "unexpected EOF while parsing value" );
				}
			}
		}
		attr[name] = val;
	}
}

static void parse_element( istream& is, xml& xml ) {
	char c;
	xml.type = node::Element;
	// parsing tag
	for(;;) {
		c = is.peek();
		if( c == EOF ) {
			ERR( "unexpected EOF while parsing tag name" );
		}
		if( !isalnum(c) ) {
			break;
		}
		xml.value.push_back(c);
		is.ignore();
	}
	parse_attr( is, xml.attr );
	c = is.peek();
	if( c == '>' ) {
		is.ignore();
		parse_xml( is, xml.children );
	} else if( c == '/' ) {
		is.ignore();
		if( (c = is.get()) != '>' ) {
			ERR( "unexpected: /" << c );
		}
	}
}

void parse_xml( istream& is, xmls& xmls ) {
	bool text = false;
    bool any = false;
	char c;
    xml cur("");
	while( (c = is.get()) != EOF ) {
		if( c == '<' ) {
            if( text ) {// place current text into the result list
                xmls.push_back(xml(""));
                swap(xmls.back(),cur);
            } else {// only spaces so far, ignore them
                cur.value.clear();
            }
			if( is.peek() == '/' ) {
				is.ignore();
				while( is.get() != '>' ) {
					if( is.eof() ) {
						ERR( "unexpected EOF in tag close" );
					}
				}
				return;
			}
            xmls.push_back(xml());
			parse_element( is, xmls.back() );
			text = false;
		} else {
            if( !isspace(c) ) {
                text = true;
            }
			cur.value.push_back(c);
		}
	}
    if( text ) {
        xmls.push_back(xml());
        swap(xmls.back(),cur);
    }
}

std::ostream& operator<<( ostream& os, xmls const& xmls ) {
	FOREACH( x, xmls ) {
		switch( x->type ) {
		case node::Element:
			os << '<' << x->value;
			FOREACH( it, x->attr ) {
				os << ' ' << it->first << "=\"" << it->second << '"';
			}
			if( x->children.empty() ) {
				os << "/>";
			} else {
				os << '>' << x->children << "</" << x->value << '>';
			}
			break;
		case node::Text:
			os << x->value;
			break;
		}
	}
    return os;
}
