namespace blub
{
namespace procedural
{
    namespace voxel
    {
        class data;
        namespace tile
        {
            template <class classType>
            class base;
            template <class voxelType>
            class container;
            template <class voxelType>
            class accessor;
            template <class voxelType>
            class renderer;
            template <class voxelType>
            class surface;
        }
        namespace simple
        {
            template <class dataType>
            class base;
            namespace container
            {
                namespace utils
                {
                    template <class voxelType>
                    class database;
                    enum class tileState;
                    template <class voxelType>
                    class tile;
                }
                template <class voxelType>
                class base;
                template <class voxelType>
                class inMemory;
                template <class voxelType>
                class database;
            }
            template <class voxelType>
            class accessor;
            template <class voxelType>
            class renderer;
            template <class voxelType>
            class surface;
        }
        namespace terrain
        {
            template <class dataType>
            class base;
            template <class voxelType>
            class accessor;
            template <class voxelType>
            class renderer;
            template <class voxelType>
            class surface;
        }
        namespace edit
        {
            template <class voxelType>
            class axisAlignedBox;
            template <class voxelType>
            class base;
            template <class voxelType>
            class box;
            template <class voxelType>
            class mesh;
            template <class voxelType>
            class noise;
            template <class voxelType>
            class sphere;
        }
    }
}
}
