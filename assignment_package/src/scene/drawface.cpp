#include "drawface.h"
#include <iostream>

DrawFace::DrawFace(OpenGLContext* context):
    Drawable(context), face(nullptr)
{

}

void DrawFace::create()
{
    if(face == nullptr)
    {
        count = 0;
        return;
    }

    std::vector<glm::vec4> pos {};

    std::vector<glm::vec4> col {};

    std::vector<glm::vec4> nor {};

    std::vector<GLuint> idx {};

    HalfEdge* beginEdge = face->edge;
    HalfEdge* e = beginEdge;

    glm::vec4 edgeColor = glm::vec4(glm::vec3(1, 1, 1) - face->col, 1);

    do{
        glm::vec3 norVec = glm::normalize(camPos - e->vert->pos);
        pos.push_back(glm::vec4(e->vert->pos, 1));
        nor.push_back(glm::vec4(norVec, 0));
        col.push_back(edgeColor);
        e = e->next;
    }while(e != beginEdge);

    for(int i = 0; i < pos.size()-1; i++)
    {
        idx.push_back(i);
        idx.push_back(i + 1);
    }

    idx.push_back(pos.size() - 1);
    idx.push_back(0);

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);
}

void DrawFace::updateFace(Face* newFace)
{
    face = newFace;
}

GLenum DrawFace::drawMode()
{
    return GL_LINES;
}
