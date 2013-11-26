namespace blub
{
namespace procedural
{
    class plane;
    class sphere;
    namespace voxel
    {
        class data;
        namespace tile
        {
            template <class classType>
            class base;
            class container;
            class accessor;
            class renderer;
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
                    enum class tileState;
                    class tile;
                }
                class base;
                class inMemory;
                class database;
            }
            class accessor;
            class renderer;
            class surface;
        }
        namespace terrain
        {
            template <class dataType>
            class base;
            class accessor;
            class renderer;
            class surface;
        }
        namespace edit
        {
            class base;
            class sphere;
            class box;
        }
    }
}
}
