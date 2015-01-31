#ifndef ENABLESHAREDFROMTHIS_HPP
#define ENABLESHAREDFROMTHIS_HPP

#include "blub/core/sharedPointer.hpp"

#include <memory>


namespace blub
{


template <typename classType>
class enableSharedFromThis : public std::enable_shared_from_this<classType>
{
public:
    typedef std::enable_shared_from_this<classType> t_base;
    typedef blub::sharedPointer<classType const> t_thisPtrConst;
    typedef blub::sharedPointer<classType> t_thisPtr;

    t_thisPtrConst getSharedThisPtr() const
    {
        return blub::sharedPointer<classType const>(t_base::shared_from_this());
    }
    t_thisPtr getSharedThisPtr()
    {
        return blub::sharedPointer<classType>(t_base::shared_from_this());
    }
};


}


#endif // ENABLESHAREDFROMTHIS_HPP
