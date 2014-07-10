#ifndef OCTREE_CONTAINER_HPP
#define OCTREE_CONTAINER_HPP

#include "blub/core/hashList.hpp"
#include "blub/core/hashMap.hpp"
#include "blub/core/list.hpp"
#include "blub/core/sharedPointer.hpp"
#include "blub/math/axisAlignedBoxInt32.hpp"
#include "blub/math/vector3.hpp"
#include "blub/math/vector3int.hpp"


namespace blub
{
namespace octree
{


template <typename dataType>
class container
{
public:
    class node
    {
    public:
        typedef node* t_nodePtr;

        node(const vector3int32& position, const vector3int32& size)
            : m_position(position)
            , m_size(size)
        {
            for (int8 ind = 0; ind < 8; ++ind)
            {
                m_nodes[ind] = nullptr;
            }
        }

        t_nodePtr getNode(const int32& index) const
        {
            BASSERT(index >= 0);
            BASSERT(index < 8);

            return m_nodes[index];
        }

        bool isInside(const vector3int32& pos) const
        {
            return pos >= m_position && pos < (m_position + m_size);
        }

        const vector3int32& getPosition(void) const
        {
            return m_position;
        }

        const vector3int32& getSize(void) const
        {
            return m_size;
        }

        axisAlignedBoxInt32 getBoundingBox(void) const
        {
            return axisAlignedBoxInt32(m_position, m_position + m_size);
        }

    protected:
        friend class container;

        t_nodePtr m_nodes[8];
        vector3int32 m_position;
        vector3int32 m_size;
    };

    class leaf : public node
    {
    public:
        typedef hashList<dataType> t_dataList;

        leaf(const vector3int32& position, const vector3int32& size)
            : node(position, size)
        {
            ;
        }

        const t_dataList& getData() const
        {
            return m_data;
        }

    protected:
        friend class container;

        bool insert(const dataType& toAdd)
        {
            BASSERT(m_data.find(toAdd) == m_data.cend());
            m_data.insert(toAdd);
            return true;
        }

        void remove(const dataType toRemove)
        {
            typename t_dataList::const_iterator it = m_data.find(toRemove);
            BASSERT(it != m_data.cend());
            m_data.erase_return_void(it);
        }

        t_dataList m_data;

    };

    typedef node* t_nodePtr;
    typedef leaf* t_leafPtr;
    typedef dataType t_data;
    typedef hashList<t_leafPtr> t_leafList;
    typedef hashMap<t_data, t_leafPtr> t_dataLeafMap;
    typedef hashMap<t_data, vector3int32> t_dataCoordsMap;
    typedef hashMap<vector3int32, t_leafPtr> t_coordLeafMap;
    typedef hashList<t_data> t_dataList;
    typedef hashList<t_nodePtr> t_nodeList;


    container(const vector3int32& minNodeSize)
        : m_minNodeSize(minNodeSize)
    {
        m_root = new node(-m_minNodeSize, m_minNodeSize*2);
    }

    ~container()
    {
        destroyNodeAndChildren(m_root);
        m_root = nullptr;
    }

    bool insert(const dataType& data, const vector3& pos)
    {
        return insert(data, convertAbsolutVector3PositionToVectorInt32(pos));
    }

    bool insert(const dataType& data, const vector3int32& pos)
    {
        typename t_dataLeafMap::const_iterator it = m_data.find(data);
        if (it != m_data.cend())
        {
            return false;
        }

        BASSERT(m_dataCoords.find(data) == m_dataCoords.cend());
        m_dataCoords.insert(data, pos);

        const vector3int32 leafCoords(convertCoordToLeafCoord(pos));
        // is root large enough?
        {
            const vector3int32 leafPosMinAbs(leafCoords*m_minNodeSize);
            if (!m_root->isInside(leafPosMinAbs))
            {
                while (!m_root->isInside(leafPosMinAbs))
                {
                    t_nodePtr rootCopy(m_root);
                    m_root = new node(rootCopy->getPosition()*2, rootCopy->getSize()*2);
                    for (int32 ind = 0; ind < 8; ++ind)
                    {
                        if (rootCopy->m_nodes[ind] != nullptr)
                        {
                            insertNodeInNode(rootCopy->m_nodes[ind], m_root);
                        }
                    }
                    delete rootCopy;
                }
            }
        }

        const vector3int32& leafCoord(leafCoords);
        t_leafPtr workLeaf;
        {
            typename t_coordLeafMap::const_iterator itLeaf(m_leafs.find(leafCoord));
            if (itLeaf != m_leafs.cend())
            {
                workLeaf = itLeaf->second;
            }
            else
            {
                // leaf didn't get found - create it
                workLeaf = new leaf(leafCoord*m_minNodeSize, m_minNodeSize);
                insertNodeInNode(workLeaf, m_root);
                m_leafs.insert(leafCoord, workLeaf);
            }
        }
        BASSERT(workLeaf != nullptr);

        workLeaf->insert(data);
        m_data.insert(data, workLeaf);

        return true;
    }

    bool remove(const dataType data)
    {
        auto it_range = m_data.equal_range(data);
        if (it_range.first == it_range.second)
        {
            return false;
        }
        for (auto it = it_range.first; it != it_range.second; ++it)
        {
            it->second->remove(data);
        }
        m_data.remove(data);
        m_dataCoords.remove(data);

        return true;
    }
    bool update(const dataType& data, const vector3int32& updateCoord)
    {
        typename t_dataLeafMap::const_iterator it = m_data.find(data);
        if (it == m_data.cend())
        {
            return false;
        }
        const t_leafPtr currentLeaf(it->second);
        if (currentLeaf->isInside(updateCoord)) // same leaf
        {
            return false;
        }
        if (!remove(data))
        {
            return false;
        }
        return insert(data, updateCoord);
    }
    bool update(const dataType& data, const vector3& updateCoord)
    {
        return update(data, convertAbsolutVector3PositionToVectorInt32(updateCoord));
    }

    t_nodeList getNodes(const dataType& data)
    {
        auto it_range = m_data.equal_range(data);
        if (it_range.first == it_range.second)
        {
            return t_nodeList();
        }
        t_nodeList result;
        for (auto it = it_range.first; it != it_range.second; ++it)
        {
            result.insert(it->second);
        }
        return result;
    }

    void compact()
    {
        ; // TODO: impl this meth.
    }

    t_nodePtr getRootNode() const
    {
        return m_root;
    }

    t_leafPtr isLeaf(t_nodePtr nd) const
    {
        if (nd->getSize() == getMinNodeSize())
        {
            return static_cast<t_leafPtr>(nd);
        }
        else
        {
            return nullptr;
        }
    }

    const vector3int32& getMinNodeSize() const
    {
        return m_minNodeSize;
    }

protected:
    void destroyNodeAndChildren(t_nodePtr toDestory)
    {
        BASSERT(toDestory != nullptr);

        for (int8 ind = 0; ind < 8; ++ind)
        {
            if (toDestory->m_nodes[ind] != nullptr)
            {
                destroyNodeAndChildren(toDestory->m_nodes[ind]);
                toDestory->m_nodes[ind] = nullptr;
            }
        }
        leaf *lf(isLeaf(toDestory));
        if (lf)
        {
            while (!lf->m_data.empty())
            {
                #ifdef BLUB_DEBUG
                const bool resultRemove =
                #endif
                        remove(*lf->m_data.cbegin());
                BASSERT(resultRemove);
            }
            delete lf;
        }
        else
        {
            delete toDestory;
        }
    }

    vector3int32 convertCoordToLeafCoord(const vector3int32& coord) const
    {
        return convertCoordToLeafCoord(coord, m_minNodeSize);
    }
    static vector3int32 convertCoordToLeafCoord(const vector3int32& coord, const vector3int32& size)
    {
        vector3int32 result(coord / size);
        if (coord.x < 0 && coord.x%size.x != 0)
        {
            --result.x;
        }
        if (coord.y < 0 && coord.y%size.y != 0)
        {
            --result.y;
        }
        if (coord.z < 0 && coord.z%size.z != 0)
        {
            --result.z;
        }
        return result;
    }
    vector3int32 convertAbsolutVector3PositionToVectorInt32(const vector3& pos) const
    {
        const vector3int32 posResult(pos.getFloor());
        return posResult;
    }

    static bool insertNodeInNode(t_nodePtr toInsert, t_nodePtr into)
    {
        BASSERT(toInsert != nullptr);
        BASSERT(into != nullptr);

        BASSERT(into->isInside(toInsert->getPosition()));
        BASSERT(into->isInside(toInsert->getPosition() + (toInsert->getSize()-vector3int32(1, 1, 1))));

        // BASSERT(into->getSize() > m_minNodeSize);
        BASSERT(into->getSize() > toInsert->getSize());

        // calc on witch node [0,8] it shall get inserted
        int32 index;
        vector3int32 posChild;
        const vector3int32 intoCenter(into->getPosition() + into->getSize() / 2);

        if (toInsert->getPosition().x <  intoCenter.x  &&
            toInsert->getPosition().y <  intoCenter.y &&
            toInsert->getPosition().z <  intoCenter.z)
        {
            index = 0;
            posChild = vector3int32(0, 0, 0);
        }
        if (toInsert->getPosition().x >= intoCenter.x  &&
            toInsert->getPosition().y <  intoCenter.y &&
            toInsert->getPosition().z <  intoCenter.z)
        {
            index = 1;
            posChild = vector3int32(1, 0, 0);
        }
        if (toInsert->getPosition().x <  intoCenter.x  &&
            toInsert->getPosition().y >= intoCenter.y &&
            toInsert->getPosition().z <  intoCenter.z)
        {
            index = 2;
            posChild = vector3int32(0, 1, 0);
        }
        if (toInsert->getPosition().x >= intoCenter.x  &&
            toInsert->getPosition().y >= intoCenter.y &&
            toInsert->getPosition().z <  intoCenter.z)
        {
            index = 3;
            posChild = vector3int32(1, 1, 0);
        }
        if (toInsert->getPosition().x <  intoCenter.x  &&
            toInsert->getPosition().y <  intoCenter.y &&
            toInsert->getPosition().z >= intoCenter.z)
        {
            index = 4;
            posChild = vector3int32(0, 0, 1);
        }
        if (toInsert->getPosition().x >= intoCenter.x  &&
            toInsert->getPosition().y <  intoCenter.y &&
            toInsert->getPosition().z >= intoCenter.z)
        {
            index = 5;
            posChild = vector3int32(1, 0, 1);
        }
        if (toInsert->getPosition().x <  intoCenter.x  &&
            toInsert->getPosition().y >= intoCenter.y &&
            toInsert->getPosition().z >= intoCenter.z)
        {
            index = 6;
            posChild = vector3int32(0, 1, 1);
        }
        if (toInsert->getPosition().x >= intoCenter.x  &&
            toInsert->getPosition().y >= intoCenter.y &&
            toInsert->getPosition().z >= intoCenter.z)
        {
            index = 7;
            posChild = vector3int32(1, 1, 1);
        }

        const vector3int32 sizeChild(into->getSize()/2);
        if (into->m_nodes[index] == nullptr)
        {
            if (sizeChild == toInsert->getSize())
            {
                into->m_nodes[index] = toInsert;
                return true;
            }
            into->m_nodes[index] = new node(into->getPosition() + posChild*sizeChild, sizeChild);
            return insertNodeInNode(toInsert, into->m_nodes[index]);
        }
        else
        {
            return insertNodeInNode(toInsert, into->m_nodes[index]);
        }
    }

private:
    const vector3int32 m_minNodeSize;

    t_nodePtr m_root;
    t_dataLeafMap m_data;
    t_dataCoordsMap m_dataCoords;
    t_coordLeafMap m_leafs;
};


}
}


#endif // OCTREE_CONTAINER_HPP
