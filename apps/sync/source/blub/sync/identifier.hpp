#ifndef NETWORK_SYNC_IDENTIFIER_HPP
#define NETWORK_SYNC_IDENTIFIER_HPP

#include "blub/core/any.hpp"
#include "blub/core/globals.hpp"


namespace blub {
namespace sync {


class identifier
{
public:
    identifier()
    {;}
    identifier(const any &userInformation_)
        : userInformation(userInformation_)
    {;}
    virtual ~identifier() {;}

    any userInformation;
};


}
}

#endif // NETWORK_SYNC_IDENTIFIER_HPP
