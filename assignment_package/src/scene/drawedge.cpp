#include "drawedge.h"

DrawEdge::DrawEdge(OpenGLContext* context):
    Drawable(context), edge(nullptr)
{

}

void DrawEdge::create()
{
    if(edge == nullptr)
    {
        count = 0;
        return;
    }
    HalfEdge* prev = edge->FindPrevious();

    std::vector<glm::vec4> pos {glm::vec4(edge->vert->pos, 1), glm::vec4(prev->vert->pos, 1)};

    std::vector<glm::vec4> col {glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 1)};

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);

    count = 2;
}

void DrawEdge::updateEdge(HalfEdge* inputEdge)
{
    edge = inputEdge;
}

GLenum DrawEdge::drawMode()
{
    return GL_LINES;
}
