#ifndef OGRETILE_HPP
#define OGRETILE_HPP

#include "blub/async/predecl.hpp"
#include "blub/procedural/voxel/tile/renderer.hpp"

#include <OGRE/OgreMesh.h>


class OgreTile : public blub::procedural::voxel::tile::renderer
{
public:   
    typedef blub::procedural::voxel::tile::renderer t_base;

    static pointer create(Ogre::SceneManager *sc,
                          Ogre::String materialName,
                          blub::async::dispatcher *graphicDispatcher, // needed because only Ogre3d thread may call ogre3d methods
                          const blub::int32 &lod,
                          const blub::vector3int32 &id);
    ~OgreTile();

    // the following methods get called by (multiple) worker threads --> have to get dispatched to graphic-thread
    void setTileData(t_tileData *convertToRenderAble, const blub::axisAlignedBox &aabb) override;

    void setVisible(const bool& vis) override;
    void setVisibleLod(const blub::uint16& indLod, const bool& vis) override;

protected:
    OgreTile(Ogre::SceneManager *sc,
             Ogre::String materialName,
             blub::async::dispatcher &graphicDispatcher,
             const blub::int32 &lod,
             const blub::vector3int32 &id);
    void initialise();

    void createMeshGraphic();
    void setTileDataGraphic(OgreTile::t_tileData *convertToRenderAble, const blub::axisAlignedBox &aabb);
    void setVisibleGraphic(const bool& vis);
    void setVisibleLodGraphic(const blub::uint16& indLod, const bool& vis);
    static void destroyAllGraphic(Ogre::SceneManager* scene, Ogre::MeshPtr mesh_, Ogre::Entity *entity_, Ogre::SceneNode *node_);

private:
    blub::async::dispatcher &m_graphicDispatcher;

    Ogre::String m_materialName;
    Ogre::SceneManager* m_scene;
    Ogre::MeshPtr m_mesh;
    Ogre::Entity* m_entity;
    Ogre::SceneNode* m_node;

    blub::int32 m_indexLodSubMesh[6];
};



#endif // OGRETILE_HPP
