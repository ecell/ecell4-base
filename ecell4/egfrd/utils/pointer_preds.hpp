#ifndef POINTER_FUN_HPP
#define POINTER_FUN_HPP

namespace ecell4
{
namespace egfrd
{

template < class T >
struct pointee_greater
{
    bool operator()( T x, T y ) const { return *y < *x; }
};


template < class T >
struct pointee_less
{
    bool operator()( T x, T y ) const { return *y > *x; }
};

} // egfrd
} // ecell4
#endif /* POINTER_PREDS */
