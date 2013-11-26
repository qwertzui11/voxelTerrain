#ifndef DEQUE_HPP
#define DEQUE_HPP

#include <deque>


namespace blub
{


template<typename elementType>
class deque : public std::deque<elementType>
{
public:
    typedef std::deque<elementType> t_base;

    deque()
    {
        ;
    }


};


}


#endif // DEQUE_HPP
