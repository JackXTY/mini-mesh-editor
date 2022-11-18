#ifndef DRAWFACE_H
#define DRAWFACE_H

#include "drawable.h"
#include "scene/halfedge.h"

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class DrawFace : public Drawable
{
public:
    DrawFace(OpenGLContext* context);

    void create() override;

    void updateFace(Face*);

    GLenum drawMode() override;

    Face* face;

    glm::vec3 camPos;
};


#endif // DRAWFACE_H
