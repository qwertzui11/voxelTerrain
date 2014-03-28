#ifndef PAIR_HPP
#define PAIR_HPP

#include <utility>

namespace blub
{
    template <class A, class B>
    class pair : public std::pair<A, B>
    {
    public:
        pair()
            : std::pair<A, B>()
        {;}
        pair(const A &a, const B &b)
            : std::pair<A, B>(a, b)
        {;}
        pair(const std::pair<A, B> &copy)
            : std::pair<A, B>(copy)
        {;}
    };
}

#endif // PAIR_HPP
