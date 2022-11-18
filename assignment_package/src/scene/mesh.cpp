#include "mesh.h"
#include <iostream>

Mesh::Mesh(OpenGLContext* context):
    Drawable(context)
{}

void Mesh::create()
{
    GLuint id = 0;
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> nor;
    std::vector<glm::vec4> col;
    std::vector<glm::ivec2> bindIDs;
    std::vector<glm::vec2> jointWeights;

    for(std::unique_ptr<Face>& face : faces)
    {
        int count = 0;
        HalfEdge* now = face.get()->edge;
        HalfEdge* beginEdge = now;
        glm::vec3 norVec = face.get()->getNormal();
        do{
            pos.push_back(glm::vec4(now->vert->pos, 1));
            col.push_back(glm::vec4(face->col, 1));
            nor.push_back(glm::vec4(norVec, 0));
            if(skinning)
            {
                bindIDs.push_back(glm::ivec2(now->vert->jointIds[0], now->vert->jointIds[1]));
                jointWeights.push_back(glm::vec2(now->vert->jointInfluence[0], now->vert->jointInfluence[1]));
            }
            now = now->next;
            count++;
        }while(now != beginEdge);

        if(count >= 3)
        {
            for(int i = 1; i <= count - 2; i++)
            {
                idx.push_back(id);
                idx.push_back(id+i);
                idx.push_back(id+i+1);
            }
        }
        id += count;
    }

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

    if(skinning)
    {
        std::cout << "generate joint buffer" << std::endl;
        generateJointID();
        generateJointWeight();
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJointIdx);
        mp_context->glBufferData(GL_ARRAY_BUFFER, bindIDs.size() * sizeof(glm::ivec2), bindIDs.data(), GL_STATIC_DRAW);
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufJointWeight);
        mp_context->glBufferData(GL_ARRAY_BUFFER, jointWeights.size() * sizeof(glm::vec2), jointWeights.data(), GL_STATIC_DRAW);
    }
}

void Mesh::splitEdge(HalfEdge* edge, glm::vec3 newPos)
{
    HalfEdge* prev = edge->FindPrevious();

    edges.push_back(std::make_unique<HalfEdge>(edge->next, nullptr, edge->face, edge->vert));
    HalfEdge* newEdge = edges.back().get();

    edge->vert->edge = newEdge;
    verts.push_back(std::make_unique<Vertex>(newPos, edge));
    Vertex* newVert = verts.back().get();

    edge->vert = newVert;
    edge->next = newEdge;

    if(edge->sym != nullptr)
    {
        HalfEdge* symEdge = edge->sym;
        edges.push_back(std::make_unique<HalfEdge>(symEdge->next, nullptr, symEdge->face, symEdge->vert));
        HalfEdge* newSymEdge = edges.back().get();

        symEdge->vert = newVert;
        symEdge->next = newSymEdge;

        edge->sym = newSymEdge;
        newSymEdge->sym = edge;
        newEdge->sym = symEdge;
        symEdge->sym = newEdge;
    }

    if(edge->sharp)
    {
        edge->next->sharp = true;
        if(edge->sym != nullptr){
            edge->sym->sharp = true;
        }
    }
}

void Mesh::triangulate(Face* face)
{
    std::vector<HalfEdge*> faceEdges;
    HalfEdge* now = face->edge;
    do{
        faceEdges.push_back(now);
        now = now->next;
    }while(now != face->edge);

    if(faceEdges.size() <= 3){return;}

    HalfEdge* begin = face->edge->FindPrevious();
    edges.push_back(std::make_unique<HalfEdge>(faceEdges[0], nullptr, face, begin->vert));
    faceEdges[1]->next = edges.back().get();
    HalfEdge* lastNewEdge = edges.back().get();

    for(int i = 2; i < faceEdges.size()-2; i++)
    {
        faces.push_back(std::make_unique<Face>(face->col, faceEdges[i]));
        edges.push_back(std::make_unique<HalfEdge>(faceEdges[i], lastNewEdge, faces.back().get(), faceEdges[i-1]->vert));
        lastNewEdge->sym = edges.back().get();
        edges.push_back(std::make_unique<HalfEdge>(edges.back().get(), nullptr, faces.back().get(), begin->vert));

        lastNewEdge = edges.back().get();
        faceEdges[i]->next = lastNewEdge;
    }

    faces.push_back(std::make_unique<Face>(face->col, faceEdges[faceEdges.size()-2]));
    edges.push_back(std::make_unique<HalfEdge>(faceEdges[faceEdges.size()-2], lastNewEdge, faces.back().get(), faceEdges[faceEdges.size()-3]->vert));
    lastNewEdge->sym = edges.back().get();
    faceEdges.back()->next = edges.back().get();
}

void Mesh::extrude(Face* face)
{
    HalfEdge* now = face->edge;
    glm::vec3 nor = face->getNormal();

    std::unordered_map<HalfEdge*, std::array<HalfEdge*, 2>> edgeMap;
    do{
        std::array<HalfEdge*, 2> arr;
        edges.push_back(std::make_unique<HalfEdge>(nullptr, now, nullptr, nullptr));
        arr[0] = edges.back().get();
        edges.push_back(std::make_unique<HalfEdge>(nullptr, now->sym, nullptr, now->vert));
        arr[1] = edges.back().get();
        edgeMap.insert({now, arr});

        verts.push_back(std::make_unique<Vertex>(now->vert->pos + nor, now));

        now->vert = verts.back().get();
        now->sym->sym = arr[1];
        now->sym = arr[0];
        now = now->next;
    }while(now != face->edge);

    do{
        faces.push_back(std::make_unique<Face>(face->col, edgeMap[now][0]));
        Face* newFace = faces.back().get();
        edgeMap[now][0]->face = newFace;
        edgeMap[now][1]->face = newFace;
        edgeMap[now][0]->vert = now->FindPrevious()->vert;

        edges.push_back(std::make_unique<HalfEdge>(edgeMap[now][1], nullptr, newFace, edgeMap[now][1]->sym->vert));
        edgeMap[now][0]->next = edges.back().get();
        edges.push_back(std::make_unique<HalfEdge>(edgeMap[now][0], nullptr, newFace, edgeMap[now][0]->sym->vert));
        edgeMap[now][1]->next = edges.back().get();

        now = now->next;
    }while(now != face->edge);

    do{
        HalfEdge* e0 = now->next->sym->next;
        HalfEdge* e1 = now->sym->next->next->next;
        e0->sym = e1;
        e1->sym = e0;
        now = now->next;
    }while(now != face->edge);
}
