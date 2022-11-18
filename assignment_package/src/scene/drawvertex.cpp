#include "drawvertex.h"

DrawVertex::DrawVertex(OpenGLContext* context)
    : Drawable(context), vert(nullptr)
{}

void DrawVertex::create()
{
    if(vert == nullptr)
    {
        count = 0;
        return;
    }

    std::vector<glm::vec4> pos {glm::vec4(vert->pos, 1)};

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    count = 1;
}

void DrawVertex::updateVertex(Vertex* newVert)
{
    vert = newVert;
}

GLenum DrawVertex::drawMode()
{
    return GL_POINTS;
}
