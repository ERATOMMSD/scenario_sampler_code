#ifndef UNKNOWN_H
#define UNKNOWN_H

/*
 * This library is for handling unknown values. by A. Yamada
 * Usage:
 *	unknown<int> foo;
 *	assert( foo.is_unknown() );
 *	foo.know(3);
 *	assert( foo.is_known() && foo.is_known(3) && *foo == 3 );
 *	foo = known(5);
 *	assert( foo.is_known(5) );
 */


#include<cstdio>// for NULL

template< class T >
class unknown {
	T* value;
public:
	unknown() : value(NULL) {}
	unknown( T const& v ) : value( new T(v) ) {}
	bool is_unknown() const {
		return value == NULL;
	}
	bool is_known() const {
		return value != NULL;
	}
	bool is_known( T const& v ) const {
		return value != NULL && *value == v;
	}
	void know( T const& v ) {
		if( value == NULL ) {
			value = new T(v);
		} else {
			*value = v;
		}
	}
	void forget() {
		if( value != NULL ) {
			delete value;
			value = NULL;
		}
	}
	T const& operator*() const {
		return *value;
	}
    T& operator*() {
        return *value;
    }
	~unknown() {
		forget();
	}
};

template<>
class unknown<bool> {
    bool value;// couldn't be a bit field. Hope for compiler optimization...
    bool known : 1;
public:
	unknown() : known(false) {}
	unknown( bool b ) : value(b), known(true) {}
	bool is_unknown() const {
		return !known;
	}
	bool is_known() const {
		return known;
	}
	bool is_known( bool b ) const {
		return known && value == b;
	}
	void know( bool b ) {
        value = b;
        known = true;
	}
	void forget() {
		known = false;
	}
	bool const& operator*() const {
		return value;
	}
    bool& operator*() {
        return value;
    }
};

template< class T >
unknown<T> known( T const& v ) {
	return unknown<T>(v);
}
#endif
