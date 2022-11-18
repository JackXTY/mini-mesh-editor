#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include <scene/mesh.h>
#include "camera.h"
#include "scene/drawvertex.h"
#include "scene/drawedge.h"
#include "scene/drawface.h"
#include "joint.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_skeleton;
    Mesh mesh;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;

    DrawVertex m_drawVertex;
    class DrawEdge m_drawEdge;
    DrawFace m_drawFace;

    std::vector<Joint*> joints;

    int selection = -1;

public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    std::array<QString, 2> getSelectedVertexJointNames();
    bool checkJointSkinning();

    QListWidget* vertsListWidget;
    QListWidget* halfEdgesListWidget;
    QListWidget* facesListWidget;

    Joint* selectedJoint = nullptr;
    double selectedWeight = .0;
    int selectedJointIdx = 0;

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent* event);
    Joint* readSkeletonJson(QJsonObject obj, Joint* parent);

public slots:
    void on_actionLoadOBJ_triggered();
    void onListItemClicked(QListWidgetItem* item);
    void onSplitEdgeButtonClicked();
    void onTriangulateButtonClicked();
    void onSubdivideButtonClicked();
    void slot_setPosX(double);
    void slot_setPosY(double);
    void slot_setPosZ(double);
    void slot_setFaceRed(double);
    void slot_setFaceGreen(double);
    void slot_setFaceBlue(double);
    void onExtrudeButtonClicked();
    void onSharpButtonClicked();
    void onSkeletonButtonClicked();
    void onSkinningButtonClicked();
    void slot_setSelectedNode(QTreeWidgetItem *i);
    void onRotXButtonClicked();
    void onRotYButtonClicked();
    void onRotZButtonClicked();
    void setJointPosX(double);
    void setJointPosY(double);
    void setJointPosZ(double);
    void setJointWeight(double);
    void replaceJoint();
    void setJointReplaceSelection(int);

signals:
    void sig_sendRootNode(QTreeWidgetItem*);
    void sig_removeRootNode();
    void sig_sendSelectedJoint();
    void sig_sendSelectedVertex();
};

glm::vec3 SpiltString(std::string str);

void GetFaceIdx(std::vector<int>& idx, std::string str);

glm::vec3 RandomColor();

bool CheckIntersection(std::array<glm::vec3, 3>, std::array<glm::vec3, 2>, float& t, glm::vec3& intersection);

float GetArea(glm::vec3, glm::vec3, glm::vec3);

#endif // MYGL_H
