#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <blub/core/triangle.hpp>

namespace blub
{

template <typename vertexType>
class geometry
{
public:
    geometry()
    {;}

    void addVertices(const blub::list<vertexType> *vert);
    void addVertices(const vertexType *vert, blub::uint32 num);
    void addTriangles(const blub::list<blub::triangle> *tri);
    void addTriangles(const blub::triangle *indices, blub::uint32 num);
    void removeTriangles(const blub::list<blub::triangle> *tri);
    void removeTriangles(const blub::triangle *tri, blub::uint32 num);

    blub::list<vertexType> *vertices() {return &m_vertices;}
    blub::list<blub::triangle> *triangles() {return &m_triangles;}
private:
    blub::list<vertexType> m_vertices;
    blub::list<blub::triangle> m_triangles;
};

}

#endif // GEOMETRY_HPP
