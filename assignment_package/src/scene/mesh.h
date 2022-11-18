#ifndef MESH_H
#define MESH_H

#include "halfedge.h"
#include "drawable.h"
#include <memory>
#include <vector>

class Mesh : public Drawable
{
public:
    Mesh(OpenGLContext* context);
    std::vector<std::unique_ptr<Face>> faces;
    std::vector<std::unique_ptr<HalfEdge>> edges;
    std::vector<std::unique_ptr<Vertex>> verts;

    void create() override;

    void splitEdge(HalfEdge* edge, glm::vec3 newPos);
    void triangulate(Face* face);
    void extrude(Face* face);

    bool skinning = false;
};

#endif // MESH_H
