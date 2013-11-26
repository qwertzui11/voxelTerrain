#ifndef OCTREE_SEARCH_HPP
#define OCTREE_SEARCH_HPP

#include "blub/math/axisAlignedBox.hpp"
#include "blub/math/octree/container.hpp"
#include "blub/math/sphere.hpp"

#include <boost/bind.hpp>
#include <boost/function/function1.hpp>


namespace blub
{
namespace octree
{


template<typename containerType>
class search
{
public:
    typedef search<containerType> t_thisClass;

    typedef container<containerType> t_container;
    typedef typename t_container::t_leafPtr t_leafPtr;
    typedef typename t_container::t_nodePtr t_nodePtr;
    typedef typename t_container::t_leafList t_leafList;
    typedef typename t_container::t_data t_data;
    typedef typename t_container::t_dataList t_dataList;


    static t_leafList getLeafesBySphere(const t_container& toSearchIn, const sphere& insideSphere)
    {
        return getLeafsByUserDefinedFunction(toSearchIn, boost::bind(&t_thisClass::getLeafsBySphereSearchFunction, insideSphere, _1));
    }

    static t_dataList getDataBySphere(const t_container& toSearchIn, const sphere& insideSphere)
    {
        t_dataList result;

        t_leafList leafs(getLeafesBySphere(toSearchIn, insideSphere));

        for (t_leafPtr leaf : leafs)
        {
            for (t_data data : leaf->getData())
            {
                result.insert(data);
            }
        }

        return result;
    }

    static t_leafList getLeafsByUserDefinedFunction(const t_container& toSearchIn, const boost::function<bool (t_nodePtr)>& isInside)
    {
        t_leafList result;
        getLeafsByUserDefinedFunctionRecursively(toSearchIn, toSearchIn.getRootNode(), isInside, &result);

        return result;
    }

    static t_dataList getDataByUserDefinedFunction(const t_container& toSearchIn, const boost::function<bool (t_nodePtr)>& isInside)
    {
        t_dataList result;

        t_leafList leafs(getLeafsByUserDefinedFunction(toSearchIn, isInside));

        for (t_leafPtr leaf : leafs)
        {
            for (t_data data : leaf->getData())
            {
                result.insert(data);
            }
        }

        return result;
    }

private:
    static void getLeafsByUserDefinedFunctionRecursively(   const t_container& tree,
                                                            const t_nodePtr& toSearchIn,
                                                            const boost::function<bool (t_nodePtr)>& isInside,
                                                            t_leafList* result)
    {
        if (isInside(toSearchIn))
        {
            t_leafPtr lf(tree.isLeaf(toSearchIn));
            if (lf)
            {
                result->insert(lf);
                return;
            }
            for (int8 child = 0; child < 8; ++child)
            {
                t_nodePtr childNode(toSearchIn->getNode(child));
                if (childNode != nullptr)
                {
                    getLeafsByUserDefinedFunctionRecursively(tree, childNode, isInside, result);
                }
            }
        }

    }

    static bool getLeafsBySphereSearchFunction(const sphere& sphere_, t_nodePtr node)
    {
        return sphere_.intersects(node->getBoundingBox());
    }


};


}
}


#endif // OCTREE_SEARCH_HPP
