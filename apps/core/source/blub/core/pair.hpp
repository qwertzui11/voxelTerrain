#ifndef PAIR_HPP
#define PAIR_HPP

#include <utility>

namespace blub
{
    template <class A, class B>
    class pair : public std::pair<A, B>
    {
    public:
        pair(void)
            : std::pair<A, B>()
        {;}
        pair(const A &a, const B &b)
            : std::pair<A, B>(a, b)
        {;}
    };
}

#endif // PAIR_HPP
