#ifndef PROCEDURAL_VOXEL_TERRAIN_ACCESSOR_HPP
#define PROCEDURAL_VOXEL_TERRAIN_ACCESSOR_HPP

#include "blub/core/globals.hpp"
#include "blub/core/vector.hpp"
#include "blub/procedural/voxel/terrain/base.hpp"


namespace blub
{
namespace procedural
{
namespace voxel
{
namespace terrain
{


/**
 * @brief The accessor class contains a custom amount of level of details of type simple::accessor.
 *
 */
template <class voxelType>
class accessor : public base<sharedPointer<tile::accessor<voxelType> > >
{
public:
    typedef base<sharedPointer<tile::accessor<voxelType> > > t_base;

    typedef simple::accessor<voxelType> t_simple;
    typedef simple::container::base<voxelType> t_simpleContainer;

    /**
     * @brief accessor constructor
     * @param worker May get called by multiple threads.
     * @param voxels The voxel-container to get the data from and to sync with.
     * @param numLod Count of level of details.
     */
    accessor(blub::async::dispatcher &worker,
             t_simpleContainer &voxels,
             const uint32& numLod)
        : m_voxels(voxels)
    {
        for (uint32 indLod = 0; indLod < numLod; ++indLod)
        {
            t_simple* lod = new t_simple(worker, voxels, indLod);
            t_base::m_lods.push_back(lod);
        }
    }

    /**
     * @brief ~accessor destructor
     */
    virtual ~accessor()
    {
        for (auto lod : t_base::m_lods)
        {
            delete lod;
        }
    }

    /**
     * @brief getVoxelContainer returns the voxel-container to get the data from and to sync with.
     * @return
     */
    t_simpleContainer &getVoxelContainer() const
    {
        return m_voxels;
    }

private:
    t_simpleContainer &m_voxels;

};


}
}
}
}


#endif // PROCEDURAL_VOXEL_TERRAIN_ACCESSOR_HPP
