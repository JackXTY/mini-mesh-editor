#ifndef JOINT_H
#define JOINT_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <QTreeWidgetItem>
#include "drawable.h"

class Joint : public Drawable, public QTreeWidgetItem
{
public:
    Joint(OpenGLContext* context);
    Joint(OpenGLContext* context, QString inputName, Joint* parentJoint, glm::vec3 position, glm::quat rotation, int idx);
    ~Joint();

    QString name;
    Joint* parent;
    std::vector<Joint*> children;
    glm::vec3 pos;
    glm::quat rot;
    glm::mat4 bind;
    bool selected = false;
    int id;

    glm::mat4 getLocalTransformation();
    glm::mat4 getOverallTransformation();

    void create() override;
    GLenum drawMode() override;
};

#endif // JOINT_H
