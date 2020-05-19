#ifndef ECELL4_EGFRD_UTILS_RANDOM_HPP
#define ECELL4_EGFRD_UTILS_RANDOM_HPP

#include <boost/range/size.hpp>
#include <boost/range/size_type.hpp>
#include <algorithm>

namespace ecell4
{
namespace egfrd
{

template<typename Trng_, typename Tracntnr_>
inline void shuffle(Trng_& rng, Tracntnr_& cntnr)
{
    typedef typename boost::range_size<Tracntnr_>::type size_type;
    for (size_type i = boost::size(cntnr); i > 0;)
    {
        --i;
        const size_type j(rng.uniform_int(0, i));
        std::swap(cntnr[i], cntnr[j]);
    }
}

} // egfrd
} // ecell4
#endif /* UTILS_RANDOM_HPP */
