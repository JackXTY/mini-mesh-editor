#ifndef DRAWVERTEX_H
#define DRAWVERTEX_H

#include "drawable.h"
#include "scene/halfedge.h"

class DrawVertex : public Drawable
{
public:
    DrawVertex(OpenGLContext* context);

    void create() override;

    void updateVertex(Vertex*);

    GLenum drawMode() override;

    Vertex *vert;
};

#endif // DRAWVERTEX_H
