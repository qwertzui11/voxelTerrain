#ifndef BLUB_PROCEDURAL_PREDECL_HPP
#define BLUB_PROCEDURAL_PREDECL_HPP


namespace blub
{
namespace procedural
{
    namespace voxel
    {
        class config;
        class data;
        struct vertex;
        namespace tile
        {
            template <class tileType>
            class base;
            template <class configType = config>
            class container;
            template <class configType = config>
            class accessor;
            template <class configType = config>
            class renderer;
            template <class configType = config>
            class surface;
        }
        namespace simple
        {
            template <class tileType>
            class base;
            namespace container
            {
                namespace utils
                {
                    template <class configType = config>
                    class database;
                    enum class tileState;
                    template <class tileType>
                    class tile;
                }
                template <class configType = config>
                class base;
                template <class configType = config>
                class inMemory;
                template <class configType = config>
                class database;
            }
            template <class voxelType = config>
            class accessor;
            template <class voxelType = config>
            class renderer;
            template <class voxelType = config>
            class surface;
        }
        namespace terrain
        {
            template <class simpleType>
            class base;
            template <class configType = config>
            class accessor;
            template <class configType = config>
            class renderer;
            template <class configType = config>
            class surface;
        }
        namespace edit
        {
            template <class configType = config>
            class axisAlignedBox;
            template <class configType>
            class base;
            template <class configType = config>
            class box;
            template <class configType = config>
            class mesh;
            template <class configType = config>
            class noise;
            template <class configType = config>
            class sphere;
        }
    }
}
}


#endif // BLUB_PROCEDURAL_PREDECL_HPP

