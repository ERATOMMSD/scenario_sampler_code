#include<time.h>
#include"util.hpp"

#if __cplusplus < 201103L
std::string std::to_string( int n ) {
	string ret;
	if( n == 0 ) {
		ret = "0";
	} else {
		int d = 1;
		for( ; d <= n; d *= 10 );
		for( d /= 10; d > 0; d /= 10 ) {
			ret += ( '0' + n / d );
			n = n % d;
		}
	}
	return ret;
}
#endif


unsigned int verbosity = V_ERR | V_WARN | V_MSG;


#if defined(__MINGW32__)
	#define GETTIME(p) do ; while(0)
	double get_time() {
		return 0;
	}
#else
#if defined(CLOCK_MONOTONIC)
	#define GETTIME(p) clock_gettime( CLOCK_MONOTONIC, p )
#else
	#include <mach/clock.h>
	#include <mach/mach.h>

	static clock_serv_t cclock;
	static int fool =
		( host_get_clock_service( mach_host_self(), CALENDAR_CLOCK, &cclock ),
		0 );

	int GETTIME( struct timespec* p ) {
		mach_timespec_t mts;
		clock_get_time( cclock, &mts );
		p->tv_sec = mts.tv_sec;
		p->tv_nsec = mts.tv_nsec;
		return 0;
	}
#endif

double get_time() {
	double res = 0;
	struct timespec t;
	if( !GETTIME( &t ) ) {
		res += t.tv_sec + 1e-9 * t.tv_nsec;
	}
	return res;
}

#endif

time_counter_t global_clock;
