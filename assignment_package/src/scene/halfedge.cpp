#include "halfedge.h"
#include <QString>

HalfEdge::HalfEdge() :
    next(nullptr), sym(nullptr), face(nullptr), vert(nullptr), id(newId), sharp(false)
{
    setText(QString::fromStdString(std::to_string(id)));
    newId++;
}


HalfEdge::HalfEdge(HalfEdge* nextEdge, HalfEdge* symEdge, Face* facePtr, Vertex* nextVert) :
    next(nextEdge), sym(symEdge), face(facePtr), vert(nextVert), id(newId), sharp(false)
{
    setText(QString::fromStdString(std::to_string(id)));
    newId++;
}

HalfEdge::~HalfEdge()
{}

HalfEdge* HalfEdge::FindPrevious()
{
    HalfEdge* now = this;
    do{
        now = now->next;
    }while(now->next != this);

    return now;
}

int HalfEdge::newId = 0;

Face::Face() :
    col(glm::vec3()), edge(nullptr), id(newId), sharp(false)
{
    setText(QString::fromStdString(std::to_string(id)));
    newId++;
}

Face::Face(glm::vec3 faceColor, HalfEdge* edgePtr):
    col(faceColor), edge(edgePtr), id(newId), sharp(false)
{
    setText(QString::fromStdString(std::to_string(id)));
    newId++;
}

Face::~Face()
{}

int Face::newId = 0;

glm::vec3 Face::getNormal()
{
    return glm::normalize(glm::cross(edge->next->vert->pos - edge->vert->pos, edge->next->next->vert->pos - edge->vert->pos));
}

Vertex::Vertex():
    pos(glm::vec3()), edge(nullptr), id(newId), sharp(false)
{
    setText(QString::fromStdString(std::to_string(id)));
    newId++;
}

Vertex::Vertex(glm::vec3 vertPos, HalfEdge* edgePtr):
    pos(vertPos), edge(edgePtr), id(newId), sharp(false)
{
    setText(QString::fromStdString(std::to_string(id)));
    newId++;
}

Vertex::~Vertex()
{}

int Vertex::newId = 0;
