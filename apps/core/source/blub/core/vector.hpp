#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "blub/core/globals.hpp"

// #include <boost/container/vector.hpp>
#include <vector>


namespace blub
{

    template <typename T>
    class vector : public std::vector<T>
    {
    public:
        typedef std::vector<T> t_base;

        vector() {;}
        vector(const std::size_t& size)
            : t_base(size)
        {
        }
    };

}


#endif // VECTOR_HPP
