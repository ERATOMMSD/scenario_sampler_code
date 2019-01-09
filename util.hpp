#ifndef UTIL_H_
#define UTIL_H_

#include<cstdio>
#include<cstdlib>
#include<iostream>

/* COPY(N,V) copies V as N.
 * This is inefficient if V is a complex object. Use only for simple types
 * or iterators.
 * LET(N,V) lets N alias V. V must be a left-hand-side value.
 */
#if __cplusplus < 201103L
	#define COPY(N,V) typeof(V) N = V
	#define LET(N,V) typeof(V)& N = V
#else
	#define COPY(N,V) auto N = V
	#define LET(N,V) auto& N = V
#endif

// macro for repeat over a list, a vector, etc.
#define FOREACH(IT,L)\
for( COPY(IT,(L).begin()); IT != (L).end(); IT++ )
#define WHILE_IN(IT,L)\
for( COPY(IT, (L).begin()); IT != (L).end(); )


#if __cplusplus < 201103L
#include<string>
namespace std {
	string to_string( int n );// int to string
}
#endif

// wrapper for tempnam
#if __MINGW32__
#define DIRSEP '\\'
// we must ignore dumb backslash
#define TMPNAM(V)                    \
char _ ## V[L_tmpnam];                \
tmpnam(_ ## V);                        \
char* V;                            \
if( _ ## V[0] == DIRSEP ) {            \
V = _ ## V + 1;                    \
} else {                            \
V = _ ## V;                        \
}
#else
#define DIRSEP '/'
#define TMPNAM(V)\
char V[L_tmpnam];\
tmpnam(V)
#endif


#define V_ERR	0x01
#define V_WARN	0x02
#define V_MSG	0x04
#define V_LOG	0x08
#define V_DEB	0x10
#define V_DEB2	0x20

extern unsigned int verbosity;

double get_time();

// this is for logging
struct time_counter_t {
protected:
	double initial;
public:
	time_counter_t() : initial(get_time()) {}
	double elapsed() {
		return get_time() - initial;
	}
};

extern time_counter_t global_clock;

#define BUG(ARG) do {\
	std::cerr << __FILE__ << ':' << __LINE__ << ": "\
	<< ARG << '!' << std::endl;\
	exit(-1);\
} while(0)
#define MSG(ARG) do if( verbosity & V_MSG ) {\
	fprintf( stderr, "# %.3f: ", global_clock.elapsed() );\
	std::cerr << ARG << std::endl;\
} while(0)
#define WARN(ARG) do {\
	if( verbosity & V_WARN ) {\
		std::cerr << "Warning: " << ARG << '!' << std::endl;\
	}\
} while(0)
#define ERR(ARG) do {\
	if( verbosity & V_ERR ) {\
		std::cerr << "Error: " << ARG << '!' << std::endl;\
	}\
	exit(-1);\
} while(0)

#define ASSERT(ARG) do {\
	if(!(ARG)) { BUG( "assertion failed: " #ARG ); }\
} while(0)

#if SSS_LOG
	// macro for debug output
	#define LOG(ARG) do if( verbosity & V_LOG ) {\
		fprintf( stderr, "# %.3f: ", global_clock.elapsed() );\
		std::cerr << ARG << std::endl;\
	} while(0)
	#define DEB(ARG) do if( verbosity & V_DEB ) {\
		std::cerr << __FILE__ << ':' << __LINE__ << ": "\
		<< ARG << std::endl;\
	} while(0)
	#define DEB2(ARG) do if( verbosity & V_DEB2 ) {\
		std::cerr << __FILE__ << ':' << __LINE__ << ": "\
		<< ARG << std::endl;\
	} while(0)
#else
	#define LOG(ARG)
	#define DEB(ARG)
	#define DEB2(ARG)
#endif
#endif
