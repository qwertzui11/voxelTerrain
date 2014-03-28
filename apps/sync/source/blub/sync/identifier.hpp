#ifndef NETWORK_SYNC_IDENTIFIER_HPP
#define NETWORK_SYNC_IDENTIFIER_HPP

#include "blub/core/enableSharedFromThis.hpp"
#include "blub/core/sharedPointer.hpp"


namespace blub {
namespace sync {


class identifier : public enableSharedFromThis<identifier>
{
public:
    typedef sharedPointer<identifier> pointer;

    static pointer create()
    {
        return pointer(new identifier());
    }
    virtual ~identifier() {;}

protected:
    identifier()
    {;}

};


}
}

#endif // NETWORK_SYNC_IDENTIFIER_HPP
