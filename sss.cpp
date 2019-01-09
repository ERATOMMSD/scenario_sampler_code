#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <list>
#include "util.hpp"
#include "xml.hpp"

using namespace std;

//command line parsing
static void split_arg( char* str, char** arg ) {
    for( int i = 0;; i++ ) {
        if( str[i] == ':' ) {
            str[i] = '\0';
            *arg = &str[i+1];
            return;
        }
        if( str[i] == '\0' ) {
            *arg = NULL;
            return;
        }
    }
}
static char const* force_arg( char const* opt, char const* arg, char const* def = NULL ) {
    if( arg != NULL ) {
        return arg;
    }
    if( def != NULL ) {
        return def;
    }
    ERR( "option " << opt << " requires an argument" );
}
static void forbid_arg( char const* opt, char const* arg ) {
    if( arg == NULL ) {
        return;
    }
    ERR( "extra argument for option " << opt );
}

/* We see a combinatorial test (CT) model as a list whose i-th element is
 * the number of values the i-th parameter has.
 */
typedef list<size_t> ct_model;

/* A test case (row) is such that i-th element denotes the index of the value for
 * the i-th parameter.
 */
typedef vector<size_t> ct_row;

// turns an xml list into a CT model, where each <choice> becomes a CT parameter
static void ct_model_of_xmls_inner( xmls const& xs, ct_model& m ) {
    FOREACH(x,xs) {
        if( x->is_element() ) {
            if( x->value == "choice" ) {
                m.push_back( x->children.size() );
            } else {
                ct_model_of_xmls_inner( x->children, m );
            }
        }
    }
}
ct_model ct_model_of_xmls( xmls const& xs ) {
    ct_model ret;
    ct_model_of_xmls_inner( xs, ret );
    return ret;
}

// gets a xml list where <choice>s are resolved according to a CT row
static void apply_ct_row( xmls const& xs, ct_row::const_iterator& vit, xmls& ret ) {
    FOREACH( x, xs ) {
        if( x->is_element() ) {
            if( x->value == "choice" ) {
                ret.push_back( x->children[*vit] );
                vit++;
            } else {
                ret.push_back(xml::node(*x));
                apply_ct_row( x->children, vit, ret.back().children );
            }
        } else {
            ret.push_back(*x);
        }
    }
}
xmls xmls_of_ct_row( xmls const& xs, ct_row const& row ) {
    xmls ret;
    COPY( vit, row.begin() );
    apply_ct_row( xs, vit, ret );
    return ret;
}

// translates a CT model into the PICT language
void output_pict( ostream& os, ct_model& m ) {
    unsigned int i = 0;
    FOREACH( p, m ) {
        os << 'p' << i << ": 0";
        if( verbosity & V_DEB ) {
            cerr << 'p' << i << ": 0";
        }
        for( size_t v = 1; v < *p; v++ ) {
            os << ", " << v;
            if( verbosity & V_DEB ) {
                cerr <<  ", " << v;
            }
        }
        os << endl;
        if( verbosity & V_DEB ) {
            cerr << endl;
        }
        i++;
    }
}

// translates a PICT output into CT rows
void get_rows_pict( FILE* fp, list<ct_row>& rows ) {
	char c;
    bool any = false;
    ct_row row;
    size_t v = 0;
	size_t i = 0;
	while( !feof(fp) ) {
		c = fgetc(fp);
		if( c == EOF ) {
            if( any ) {
                row.push_back(v);
                rows.push_back(row);
                row.clear();
                v = 0;
                any = false;
            }
			return;
        } else if( c == '\n' ) {
            if( any ) {
                row.push_back(v);
                rows.push_back(row);
                row.clear();
                v = 0;
                any = false;
            }
        } else if( c == '\r' ) {
			if( ( c = fgetc(fp) ) != '\n' ) {
				ungetc(c,fp);
			}
            if( any ) {
                row.push_back(v);
                rows.push_back(row);
                row.clear();
                v = 0;
                any = false;
            }
		} else if( isnumber(c) ) {
			v = v * 10 + c-'0';
            any = true;
        } else if( c == ',' || c == '\t' ) {
            row.push_back(v);
            v = 0;
        } else {
            ERR( "unexpected PICT output '" << c << "'" );
        }
	}
}

// for future use for the ACTS tool
static bool get_line_acts( istream& is, char* s, size_t size ) {
	char c;
	size_t i = 0;
	while( i < size - 1 && !is.eof() ) {
		c = is.get();
		if( c == '\n' || c == EOF ) {
			break;
		} else if( c == '\r' ) {
			if( is.peek() == '\n' ) {
				is.ignore();
			}
			break;
		} else if( c == '"' ) {
			// quotation mark is ignored
		} else {
			s[i] = c;
			i++;
		}
	}
	s[i] = '\0';
	return i < size;
}

// expands all <text>s
void expand_text( xmls const& xs, xmls& out ) {
    FOREACH(x,xs) {
        if( x->is_element() ) {
            if( x->value == "text" ) {
                expand_text( x->children, out );
            } else {
                out.push_back( xml::node(*x) );
                expand_text( x->children, out.back().children );
            }
        } else {
            out.push_back(*x);
        }
    }
}

// expands all <repeat>s uniformly randomly
void expand_repeat( xmls const& xs, xmls& out ) {
	FOREACH( x, xs ) {
		if( x->is_element() ) {
            if( x->value == "repeat" ) {
                // parsing min and max
                string const* p_min = x->find_attribute( "minOccurs" );
                unsigned int min = p_min == NULL ? 0 : stoi(*p_min);
                string const* p_max = x->find_attribute( "maxOccurs" );
                unsigned int max = p_max == NULL ? min + 10 : stoi(*p_max);
                unsigned int times = min + (rand() % (max - min));
                string const* p_delim = x->find_attribute( "delim" );
                DEB( min << " <= " << times << " <= " << max );
                for( unsigned int i = min; ; i++ ) {
                    expand_repeat( x->children, out );
                    if( i >= times ) {
                        break;
                    }
                    if( p_delim != NULL ) {
                        out.push_back(xml::node(*p_delim));
                    }
                }
            } else {
                out.push_back(xml::node(*x));
                expand_repeat( x->children, out.back().children );
            }
        } else {
            out.push_back(*x);
        }
    }
}

// expands all <choice>s uniformly randomly
void expand_choice( xmls const& xs, xmls& out ) {
	FOREACH( x, xs ) {
		if( x->is_element() ) {
            xml::tree const* xx;
			if( x->value == "choice" ) {
				size_t size = x->children.size(); 
				unsigned int choice = (double)rand() / RAND_MAX * size;
				xx = &(x->children[choice]);
			} else {
				xx = &*x;
			}
            out.push_back( xml::node(*xx) );
			expand_choice( xx->children, out.back().children );
        } else {
            out.push_back(*x);
        }
    }
}

// expands all <random>s uniformly randomly
void expand_random( xmls const& xs, xmls& out ) {
    FOREACH(x,xs) {
        if( x->is_element() ) {
            if( x->value == "random" ) {
                // parsing min and max
                string const* p_min = x->find_attribute( "min" );
                if( p_min == NULL ) {
                    ERR( "missing attribute 'min' in <random>" );
                }
                double min = stof(*p_min);
                string const* p_max = x->find_attribute( "max" );
                if( p_max == NULL ) {
                    ERR( "missing attribute 'max' in <random>" );
                }
                double max = stof(*p_max);
                double val = min + (double)rand()/RAND_MAX * (max - min);
                DEB( min << " <= " << val << " <= " << max );
                out.push_back( xml::node(to_string(val)) );
            } else {
                out.push_back( xml::node(*x) );
                expand_random( x->children, out.back().children );
            }
        } else {
         out.push_back(*x);
        }
    }
}

int main( int argc, char** argv ) {
    char* arg;
    string s;
    istream* pis = &cin;
    unsigned int n_rep = 1;
    unsigned int n_choice = 1;// 0 means using PICT!
    unsigned int n_unif = 1;
    unsigned int seed = time(NULL);

    for( int i = 1; i < argc; i++ ) {
        split_arg( argv[i], &arg );
        if( argv[i][0] == '-' ) {
            switch( argv[i][1] ) {
                case 'v':
                    s = force_arg( argv[i], arg );
                    if( s == "0" ) {
                        verbosity = 0;
                    } else if( s == "1" ) {
                        verbosity = V_ERR;
                    } else if( s == "2" ) {
                        verbosity = V_ERR | V_WARN;
                    } else if( s == "3" ) {
                        verbosity = V_ERR | V_WARN | V_MSG;
                    } else if( s == "4" ) {
                        verbosity = V_ERR | V_WARN | V_MSG | V_LOG;
                    } else if( s == "5" ) {
                        verbosity = V_ERR | V_WARN | V_MSG | V_LOG | V_DEB;
                    } else if( s == "6" ) {
                        verbosity = V_ERR | V_WARN | V_MSG | V_LOG | V_DEB | V_DEB2;
                    } else {
                        ERR( "Unknown verbosity '" << s << "'" );
                    }
                    break;
                case 'r':
                    s = force_arg( argv[i], arg );
                    n_rep = stoi(s);
                    break;
                case 'c':
                    s = force_arg( argv[i], arg, "0" );
                    n_choice = stoi(s);
                    break;
                case 's':
                    s = force_arg( argv[i], arg );
                    seed = stoi(s);
                    break;
                case 'u':
                    s = force_arg( argv[i], arg );
                    n_unif = stoi(s);
                    break;
                default:
                    ERR( "Unknown option '" << argv[i] <<"'" );
            }
        } else {
            ifstream* pifs = new ifstream(argv[i]);
            if( !pifs->is_open() ) {
                ERR( "Cannot open '" << argv[i] << "'" );
            }
            pis = pifs;
        }
    }
    // main procesure
    srand(seed);
    xmls xs0, xs1;
    *pis >> xs0;
    expand_text( xs0, xs1 );
    list<xmls> xss2, xss3, xss4;
    DEB( xs1 << endl << "====" );
    // expand <repeat>
    for( unsigned int i = 0; i < n_rep; i++ ) {
        xss2.push_back( xmls() );
        expand_repeat( xs1, xss2.back() );
        DEB( xss2.back() );
    }
    // expand <choice>
    FOREACH(xs2,xss2) {
        if( n_choice > 0 ) {// random testing
            for( unsigned int i = 0; i < n_choice; i++ ) {
                xss3.push_back( xmls() );
                expand_choice( *xs2, xss3.back() );
            }
        } else {// combinatorial testing
            list<ct_row> rows;
            ct_model m = ct_model_of_xmls(*xs2);
            // calling PICT
            {
                TMPNAM(tmp);
                ofstream os( tmp, ofstream::trunc );
                output_pict(os,m);
                stringstream cmd;
                cmd << "pict \"" << tmp << "\"";
                MSG( "Running: " << cmd.str() );
                double pict_time = -global_clock.elapsed();
                FILE* pout = popen( cmd.str().c_str(), "r" );
                char buf[65536];
                fgets( buf, sizeof buf, pout );//ignore first line
                get_rows_pict(pout,rows);
                pict_time += global_clock.elapsed();
                MSG( " ...done. PICT time: " << pict_time );
                pclose(pout);
                remove(tmp);
            }
            if( verbosity & V_DEB ) {
                cerr << "pict rows:";
                FOREACH(row,rows) {
                    cerr << endl;
                    FOREACH(v,*row) {
                        cerr << '\t' << *v;
                    }
                }
                cerr << endl;
            }
            FOREACH(row,rows) {
                xss3.push_back( xmls_of_ct_row( *xs2, *row ) );
            }
        }
    }
    // expand <random>
    FOREACH(xs3,xss3) {
        for( int i = 0; i < n_unif; i++ ) {
            DEB( *xs3 << "----" );
            xss4.push_back(xmls());
            expand_random( *xs3, xss4.back() );
        }
    }
    FOREACH(xs4,xss4) {
        cout << *xs4 << endl;
    }
	exit(0);
}

