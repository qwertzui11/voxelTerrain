#ifndef ANY_HPP
#define ANY_HPP

#include <boost/any.hpp>


namespace blub
{


class any : public boost::any
{
public:
    typedef boost::any t_base;

    any()
        : t_base()
    {
        ;
    }

    template<typename ValueType>
    any(const ValueType & value)
      : t_base(value)
    {
    }

    any(const any & other)
      : t_base(other)
    {
        ;
    }
};


}


#endif // ANY_HPP
