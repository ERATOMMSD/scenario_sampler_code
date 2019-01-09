#include<iostream>
#include "xml.hpp"

using namespace std;

void ignorespace( istream& is ) {
    while( !is.eof() && isspace(is.peek()) ) {
        is.get();
    }
}

using namespace xml;
string const* node::find_attribute( char const* name ) const {
    COPY( it, attr.find(name) );
    if( it == attr.end() ) {
        return NULL;
    } else {
        return &it->second;
    }
}
string* node::find_attribute( char const* name ) {
    COPY( it, attr.find(name) );
    if( it == attr.end() ) {
        return NULL;
    } else {
        return &it->second;
    }
}
string const& node::get_attribute( char const* name, string const& def ) const {
    string const* p = find_attribute(name);
    if( p == NULL ) {
        return def;
    } else {
        return *p;
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

void xml::parse_inner( istream& is, xml::tree& xml ) {
	char c;
	xml.set_element();
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
		is >> xml.children;
	} else if( c == '/' ) {
		is.ignore();
		if( (c = is.get()) != '>' ) {
			ERR( "unexpected: /" << c );
		}
	}
}

istream& operator>>( istream& is, xmls& xmls ) {
	bool text = false;
    bool any = false;
	char c;
    xml::tree cur("");
	while( (c = is.get()) != EOF ) {
		if( c == '<' ) {
            if( text ) {// place current text into the result list
                xmls.push_back(xml::tree(""));
                swap(xmls.back(),cur);
            } else {// only spaces so far, ignore them
                cur.value.clear();
            }
            c = is.peek();
            if( c == '/' ) { // </...>
                is.ignore();
                while( is.get() != '>' ) {
                    if( is.eof() ) {
                        ERR( "unexpected EOF in tag close" );
                    }
                }
                return is;
            }
            xmls.push_back(xml::tree());
            LET(x,xmls.back());
            text = false;
            if( c == '?' ) { // <? ... ?>
                x.set_special();
                is.ignore();
                for(;;) {
                    c = is.get();
                    if( c == '?' ) {
                        c = is.get();
                        if( c == '>' ) {// the closing "?>"
                            break;
                        }
                        x.value.push_back('?');
                    }
                    if( c == EOF ) {
                        ERR( "unexpected EOF in mata tag" );
                    }
                    x.value.push_back(c);
                }
            } else {
                parse_inner(is,x);
            }
		} else {
            if( !isspace(c) ) {
                text = true;
            }
			cur.value.push_back(c);
		}
	}
    if( text ) {
        xmls.push_back(xml::tree());
        swap(xmls.back(),cur);
    }
    return is;
}

ostream& operator<<( ostream& os, xmls const& xmls ) {
	FOREACH( x, xmls ) {
        switch( x->type ) {
            case xml::node::Element:
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
            case xml::node::Text:
                os << x->value;
                break;
            case xml::node::Special:
                os << "<?" << x->value << "?>";
                break;
        }
    }
    return os;
}
