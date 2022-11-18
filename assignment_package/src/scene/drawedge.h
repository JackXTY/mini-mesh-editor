#ifndef DRAWEDGE_H
#define DRAWEDGE_H

#include "drawable.h"
#include "scene/halfedge.h"

class DrawEdge : public Drawable
{
public:
    DrawEdge(OpenGLContext* context);

    void create() override;

    void updateEdge(HalfEdge*);
    void updateEdge(Vertex*, Vertex*);

    GLenum drawMode() override;

    HalfEdge* edge;
};

#endif // DRAWEDGE_H
