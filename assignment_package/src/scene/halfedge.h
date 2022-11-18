#ifndef HALFEDGE_H
#define HALFEDGE_H

#include <glm/glm.hpp>
#include <QListWidgetItem>
#include "joint.h"

class Face;
class Vertex;

class HalfEdge : public QListWidgetItem
{
public:
    HalfEdge();
    HalfEdge(HalfEdge* nextEdge, HalfEdge* symEdge, Face* facePtr, Vertex* nextVert);
    ~HalfEdge();

    HalfEdge* next;
    HalfEdge* sym;
    Face* face;
    Vertex* vert;
    int id;
    bool sharp;

    HalfEdge* FindPrevious();

private:
    static int newId;
};

class Face : public QListWidgetItem
{
public:
    Face();
    Face(glm::vec3 faceColor, HalfEdge* edgePtr);
    ~Face();

    glm::vec3 getNormal();

    glm::vec3 col;
    HalfEdge* edge;
    int id;
    bool sharp;

private:
    static int newId;
};

class Vertex : public QListWidgetItem
{
public:
    Vertex();
    Vertex(glm::vec3 vertPos, HalfEdge* edgePtr);
    ~Vertex();

    glm::vec3 pos;
    HalfEdge* edge;
    int id;
    bool sharp;
    std::array<int, 2> jointIds = {-1, -1};
    std::array<float, 2> jointInfluence;

private:
    static int newId;
};

#endif // HALFEDGE_H
