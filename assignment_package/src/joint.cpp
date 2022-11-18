#include "joint.h"
#include <iostream>

Joint::Joint(OpenGLContext* context):
    Drawable(context), name(""), parent(nullptr), pos(glm::vec3()), rot(glm::quat()), id(0)
{
    bind = glm::inverse(getOverallTransformation());
    setText(0, name);
}

Joint::Joint(OpenGLContext* context, QString inputName, Joint* parentJoint, glm::vec3 position, glm::quat rotation, int idx):
    Drawable(context), name(inputName), parent(parentJoint), pos(position), rot(rotation), id(idx)
{
    bind = glm::inverse(getOverallTransformation());
    setText(0, name);
}

Joint::~Joint()
{
    Drawable::~Drawable();
    QTreeWidgetItem::~QTreeWidgetItem();
}

glm::mat4 Joint::getLocalTransformation()
{
    return glm::toMat4(rot) * glm::translate(glm::mat4(), pos);
}

glm::mat4 Joint::getOverallTransformation()
{
    if(parent == nullptr)
    {
        return getLocalTransformation();
    }
    else
    {
        return parent->getOverallTransformation() * getLocalTransformation();
    }
}

void Joint::create()
{
    std::vector<glm::vec4> posVec {};
    std::vector<glm::vec4> colVec {};
    std::vector<GLuint> idxVec;

    float size = 0.4f;
    float deg = 2.f * 3.14159f / 12.f;

    glm::mat4 trans = getOverallTransformation();
    glm::vec4 c = glm::vec4(0, 0, 0, 1);
    glm::vec4 col0 = glm::vec4(0.f, 0.f, 1.f, 1.f);
    glm::vec4 col1 = glm::vec4(0.f, 1.f, 0.f, 1.f);
    glm::vec4 col2 = glm::vec4(1.f, 0.f, 0.f, 1.f);
    if(selected)
    {
        col1 = glm::vec4(0.3f, 0.3f, 0.75f, 1.f);
        col2 = glm::vec4(0.3f, 0.75f, 0.3f, 1.f);
        col0 = glm::vec4(0.75f, 0.3f, 0.3f, 1.f);
    }

    for(int i = 0; i < 12; i++)
    {
        posVec.push_back(trans * (glm::vec4(size * std::cos(deg * i), size * std::sin(deg * i), 0.f, 0.f) + c));
        colVec.push_back(col0);
        posVec.push_back(trans * (glm::vec4(size * std::cos(deg * i), 0.f, size * std::sin(deg * i), 0.f) + c));
        colVec.push_back(col1);
        posVec.push_back(trans * (glm::vec4(0.f, size * std::cos(deg * i), size * std::sin(deg * i), 0.f) + c));
        colVec.push_back(col2);
    }

    for(int i = 0; i < 11; i++)
    {
        int tiv[6] = {3 * i, 3 * i + 3, 3 * i + 1, 3 * i + 4, 3 * i + 2, 3 * i + 5};
        for(int j = 0; j < 6; j++)
            idxVec.push_back(tiv[j]);
    }

    int iv[6] = {33, 0, 34, 1, 35, 2};
    for(int i = 0; i < 6; i++)
        idxVec.push_back(iv[i]);

    int parentId = posVec.size();
    posVec.push_back(trans * c);
    colVec.push_back(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

    for(int i = 0; i < children.size(); i++)
    {
        idxVec.push_back(parentId);
        idxVec.push_back(posVec.size());
        posVec.push_back(children[i]->getOverallTransformation() * glm::vec4(0, 0, 0, 1));
        colVec.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    }

    count = idxVec.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxVec.size() * sizeof(GLuint), idxVec.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, posVec.size() * sizeof(glm::vec4), posVec.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, colVec.size() * sizeof(glm::vec4), colVec.data(), GL_STATIC_DRAW);
}

GLenum Joint::drawMode()
{
    return GL_LINES;
}
