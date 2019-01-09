#ifndef _TREE_HPP_
#define _TREE_HPP_
#include<list>
#include<vector>
#include<map>
/*
 * template for trees
 */
template< class T > class tree;
template< class T > void swap( tree<T>& t1, tree<T>& t2 );

template< class T >
class tree : public T {
public:
	std::vector< tree<T> > children;
	tree<T>(){}
	tree<T>( T const& v ) : T(v) {}
	tree<T>& operator<<( tree<T> const& t ) {
		children.push_back(t);
		return *this;
	}
	tree<T>& nest( T const& node );
	tree<T>* path( std::vector<size_t> const& p );
	friend void swap<T>( tree<T>& t1, tree<T>& t2 );
};

template< class T >
tree<T>& tree<T>::nest( T const& node ) {
	tree<T> t( node );
	t << *this;
	swap( *this, t );
	return *this;
}

template< class T >
void swap( tree<T>& t1, tree<T>& t2 ) {
	swap( t1.children, t2.children );
	std::swap<T>( t1, t2 );
}

template< class T >
tree<T>* tree<T>::path( std::vector<size_t> const& p ) {
	typeof(this) ret = this;
	typeof(p.begin()) it = p.begin();
	while( it != p.end() ) {
		typeof(it) prev = it;
		it++;
		ret = children[*prev].path(it);
	}
	return ret;
}

#endif
