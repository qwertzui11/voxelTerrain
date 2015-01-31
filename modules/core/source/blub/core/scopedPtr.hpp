#ifndef BLUB_CORE_SCOPEDPTR_HPP
#define BLUB_CORE_SCOPEDPTR_HPP

#include <memory>


namespace blub
{


template <class T>
using scopedPointer = std::unique_ptr<T>;


}


#endif // BLUB_CORE_SCOPEDPTR_HPP
