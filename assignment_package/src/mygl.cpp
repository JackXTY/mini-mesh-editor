#include "mygl.h"
#include "scene/mesh.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>
#include <unordered_set>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this), prog_skeleton(this),
      m_glCamera(), mesh(this),
      m_drawVertex(this), m_drawEdge(this), m_drawFace(this),
      joints(std::vector<Joint*>())
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    mesh.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    m_geomSquare.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    prog_skeleton.create(":/glsl/skeleton.vert.glsl", ":/glsl/skeleton.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    prog_skeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(m_glCamera.eye);
    m_progFlat.setModelMatrix(glm::mat4(1.f));
    prog_skeleton.setViewProjMatrix(m_glCamera.getViewProj());
    prog_skeleton.setCamPos(m_glCamera.eye);

     //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.

    glm::mat4 model =glm::mat4();

    m_progLambert.setModelMatrix(model);
    prog_skeleton.setModelMatrix(model);
    //Draw the example sphere using our lambert shader

    if(mesh.faces.size() > 0)
    {
        if(mesh.skinning)
        {
            prog_skeleton.setJointInfo(joints);
            prog_skeleton.draw(mesh);
        }
        else
        {
            m_progLambert.draw(mesh);
        }
    }


    glDisable(GL_DEPTH_TEST);

    switch(selection)
    {
    case 0:
        m_drawVertex.create();
        if(m_drawVertex.elemCount() > 0)
        {
            m_progFlat.draw(m_drawVertex);
        }
        break;
    case 1:
        m_drawEdge.create();
        if(m_drawEdge.elemCount() > 0)
        {
            m_progFlat.draw(m_drawEdge);
        }
        break;
    case 2:
        m_drawFace.camPos = m_glCamera.eye;
        m_drawFace.create();
        if(m_drawFace.elemCount() > 0)
        {
            m_progFlat.draw(m_drawFace);
        }
        break;
     default:
        break;
    }

    for(int i = 0; i < joints.size(); i++)
    {
        Joint* j = joints[i];
        j->create();
        if(j->elemCount() > 0)
        {
            m_progFlat.draw(*j);
        }
    }

    glEnable(GL_DEPTH_TEST);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_N){
        if(selection == 1 && m_drawEdge.edge != nullptr)
        {
            m_drawEdge.updateEdge(m_drawEdge.edge->next);
            selection = 1;
            update();
        }
    } else if (e->key() == Qt::Key_M){
        if(selection == 1 && m_drawEdge.edge != nullptr)
        {
            if(m_drawEdge.edge->sym == nullptr)
            {
                std::cout << "selected edge doesn't have symmetrical edge!";
            }
            else
            {
                m_drawEdge.updateEdge(m_drawEdge.edge->sym);
                selection = 1;
                update();
            }
        }
    } else if (e->key() == Qt::Key_F){
        if(selection == 1 && m_drawEdge.edge != nullptr)
        {
            m_drawFace.updateFace(m_drawEdge.edge->face);
            selection = 2;
            update();
        }
    } else if (e->key() == Qt::Key_V){
        if(selection == 1 && m_drawEdge.edge != nullptr){
            m_drawVertex.updateVertex(m_drawEdge.edge->vert);
            selection = 0;
            emit sig_sendSelectedVertex();
            update();
        }
    } else if (e->key() == Qt::Key_H){
        if(e->modifiers() & Qt::ShiftModifier){
            if(selection == 2 && m_drawFace.face != nullptr)
            {
                if(m_drawFace.face->edge == nullptr)
                {
                    std::cout << "ERROR: selected face don't have edge!";
                }
                m_drawEdge.updateEdge(m_drawFace.face->edge);
                selection = 1;
                update();
            }
        } else {
            if(selection == 0 && m_drawVertex.vert != nullptr)
            {
                std::cout<<"get halfEdge of vertex " << m_drawVertex.vert->id << std::endl;
                if(m_drawVertex.vert->edge == nullptr)
                {
                    std::cout << "ERROR: selected vertex don't have edge!";
                }
                std::cout<<"update with edge " << m_drawVertex.vert->edge->id << std::endl;
                m_drawEdge.updateEdge(m_drawVertex.vert->edge);
                selection = 1;
                update();
            }
        }
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::on_actionLoadOBJ_triggered()
{
    m_drawVertex.updateVertex(nullptr);
    m_drawEdge.updateEdge(nullptr);
    m_drawFace.updateFace(nullptr);

    QString filter = "OBJ Files (*.obj)";
    QString fileName = QFileDialog::getOpenFileName(this, "Select a file...", QDir::currentPath() + "/..", filter);
    std::cout<<"load "<<fileName.toStdString()<<std::endl;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    mesh.edges.clear();
    mesh.faces.clear();
    mesh.verts.clear();

    std::vector<glm::vec3> posVec;
    std::unordered_map<std::string, HalfEdge*> map = std::unordered_map<std::string, HalfEdge*>();
    std::unordered_map<int, Vertex*> vertSet = std::unordered_map<int, Vertex*>();

    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {

        std::string str = line.toStdString();
        if(str[0]=='v' && str[1]==' ')
        {
            glm::vec3 pos = SpiltString(str.substr(2, str.size()-2));
            posVec.push_back(pos);
        }
        else if(str[0]=='f' && str[1]==' ')
        {
            std::vector<int> idx;
            GetFaceIdx(idx, str);
            int edgesSize = mesh.edges.size();

            mesh.faces.push_back(std::make_unique<Face>(Face()));
            facesListWidget->addItem(mesh.faces.back().get());

            for(int i = 0; i < idx.size(); i++)
            {
                mesh.edges.push_back(std::make_unique<HalfEdge>());

                Vertex* v;
                if(vertSet.find(idx[i]) == vertSet.end())
                {
                    mesh.verts.push_back(std::make_unique<Vertex>(posVec[idx[i]], mesh.edges.back().get()));
                    v = mesh.verts.back().get();
                    vertSet.insert({idx[i], v});
                    vertsListWidget->addItem(v);
                }
                else{
                    v = vertSet[idx[i]];
                }

                mesh.edges.back()->vert = v;
                mesh.edges.back()->face = mesh.faces.back().get();
                if(i>0){
                    mesh.edges[edgesSize + i - 1]->next = mesh.edges.back().get();
                }

                halfEdgesListWidget->addItem(mesh.edges.back().get());
            }
            mesh.edges.back()->next = mesh.edges[edgesSize].get();

            mesh.faces.back()->edge = mesh.edges.back().get();
            mesh.faces.back()->col = RandomColor();

            HalfEdge* now = mesh.edges.back().get();
            do{
                HalfEdge* nextEdge = now->next;
                auto sear = map.find(std::to_string(nextEdge->vert->id)+"/"+std::to_string(now->vert->id));
                if(sear != map.end())
                {
                    nextEdge->sym = sear->second;
                    sear->second->sym = nextEdge;
                }
                else
                {
                    map.insert({std::to_string(now->vert->id)+"/"+std::to_string(nextEdge->vert->id), nextEdge});
                }
                now = nextEdge;
            }while(now != mesh.edges.back().get());
        }

        line = in.readLine();
    }

    mesh.create();
    update();
}


void MyGL::onListItemClicked(QListWidgetItem* item)
{
    Vertex* vert = dynamic_cast<Vertex*>(item);
    if(vert != nullptr)
    {
        m_drawVertex.updateVertex(vert);
        selection = 0;
        emit sig_sendSelectedVertex();
    }

    HalfEdge* edge = dynamic_cast<HalfEdge*>(item);
    if(edge != nullptr)
    {
        m_drawEdge.updateEdge(edge);
        selection = 1;
    }

    Face* face = dynamic_cast<Face*>(item);
    if(face != nullptr)
    {
        m_drawFace.updateFace(face);
        selection = 2;
    }

    update();
}

void MyGL::mousePressEvent(QMouseEvent* event)
{
    float x = event->position().x();
    float y = event->position().y();

    glm::vec2 screenPos = glm::vec2(x/(float)size().width(), y/(float)size().height());
    screenPos = (screenPos - glm::vec2(0.5, 0.5)) * 2.0f;

    glm::vec3 pos = m_glCamera.ref +
            m_glCamera.V * (-screenPos.y) +
            m_glCamera.H * screenPos.x;

    Face* targetFace = nullptr;
    float min_t = -1;
    glm::vec3 inter;
    std::array<glm::vec3, 2> rayArr = {m_glCamera.eye, glm::normalize(pos - m_glCamera.eye)};

    for(std::unique_ptr<Face>& faceRef: mesh.faces)
    {
        Face* face = faceRef.get();
        HalfEdge* beginEdge = face->edge;
        HalfEdge* now = beginEdge;
        std::vector<glm::vec3> poss;
        do{
            poss.push_back(now->vert->pos);
            now = now->next;
        }while(now != beginEdge);

        for(int i = 1; i < poss.size() - 1; i++)
        {
            float t;
            glm::vec3 intersection;
            if(CheckIntersection({poss[0], poss[i], poss[i+1]}, rayArr, t, intersection))
            {
                if(t < min_t || min_t < 0)
                {
                    inter = intersection;
                    min_t = t;
                    targetFace = face;
                    break;
                }
            }
        }
    }

    if(targetFace)
    {
        bool findPoint = false;
        HalfEdge* beginEdge = targetFace->edge;
        HalfEdge* e = beginEdge;
        do{
            if(glm::length(e->vert->pos - inter) < 0.1)
            {
                findPoint = true;
                selection = 0;
                m_drawVertex.updateVertex(e->vert);
                update();
                emit sig_sendSelectedVertex();
                return;
            }
            e = e->next;
        }while(e != beginEdge);

        e = beginEdge;
        do{
            glm::vec3 d = inter - e->vert->pos;
            float c =glm::dot(glm::normalize(d), glm::normalize(e->next->vert->pos - e->vert->pos));
            float dis = glm::length(d) * std::sqrt(1 - c * c);

            if(dis < 0.1)
            {
                findPoint = true;
                selection = 1;
                m_drawEdge.updateEdge(e->next);
                update();
                return;
            }
            e = e->next;
        }while(e != beginEdge);

        selection = 2;
        m_drawFace.updateFace(targetFace);
        update();
    }
}

void MyGL::onSplitEdgeButtonClicked()
{
    if(selection == 1 && m_drawEdge.edge != nullptr)
    {
        glm::vec3 newPos = (m_drawEdge.edge->vert->pos + m_drawEdge.edge->FindPrevious()->vert->pos) / 2.0f;
        mesh.splitEdge(m_drawEdge.edge, newPos);
    }

    if(m_drawEdge.edge->sym != nullptr)
    {
        halfEdgesListWidget->addItem(mesh.edges[mesh.edges.size()-2].get());
    }
    vertsListWidget->addItem(mesh.verts.back().get());
    halfEdgesListWidget->addItem(mesh.edges.back().get());

    update();
}

void MyGL::onTriangulateButtonClicked()
{
    int fs = mesh.faces.size();
    int es = mesh.edges.size();
    if(selection == 2 && m_drawFace.face != nullptr)
    {
        mesh.triangulate(m_drawFace.face);
    }

    for(int i = fs; i < mesh.faces.size(); i++)
    {
        facesListWidget->addItem(mesh.faces[i].get());
    }
    for(int i = es; i < mesh.edges.size(); i++)
    {
        halfEdgesListWidget->addItem(mesh.edges[i].get());
    }
}

void MyGL::onSubdivideButtonClicked()
{
    if(mesh.faces.empty()){return;}

    std::unordered_map<Face*, Vertex*> map = std::unordered_map<Face*, Vertex*>();
    std::unordered_set<HalfEdge*> edgeSet = std::unordered_set<HalfEdge*>();
    std::unordered_set<Vertex*> vertSet = std::unordered_set<Vertex*>();
    int vSize = mesh.verts.size();
    int eSize = mesh.edges.size();
    int fSize = mesh.faces.size();

    // generate all central point for each face
    for(auto& f : mesh.faces)
    {
        glm::vec3 center;
        HalfEdge* e = f->edge;
        int count = 0;
        do{
            center += e->vert->pos;
            count++;
            e = e->next;
        }while(e != f->edge);
        center /= (float)count;
        mesh.verts.push_back(std::make_unique<Vertex>(center, nullptr));
        map.insert({f.get(), mesh.verts.back().get()});
    }

    // split every edge
    int s = mesh.edges.size();
    for(int i = 0; i < s; i++)
    {
        HalfEdge* e = mesh.edges[i].get();
        if(edgeSet.find(e) != edgeSet.end())
        {
            continue;
        }

        // calculate position of new vertex in original edge
        glm::vec3 newPos;
        if(e->sharp)
        {
            newPos = e->vert->pos
                    + e->FindPrevious()->vert->pos;
            newPos /= 2.0f;
        }
        else{
            newPos = e->vert->pos
                    + e->FindPrevious()->vert->pos
                    + map[e->face]->pos;
            if(e->sym == nullptr){
                newPos /= 3;
            }
            else{
                newPos = newPos + map[e->sym->face]->pos;
                newPos /= 4;
            }
        }


        edgeSet.insert(e);
        if(e->sym != nullptr)
        {
            edgeSet.insert(e->sym);
        }
        mesh.splitEdge(e, newPos);
        vertSet.insert(e->vert);
    }

    // adjust original vertex positions
    for(int i = 0; i < mesh.edges.size(); i++)
    {
        Vertex* v = mesh.edges[i]->vert;
        if(vertSet.find(v) != vertSet.end())
        {
            continue;
        }
        vertSet.insert(v);

        if(v->sharp)
        {
            continue;
        }

        glm::vec3 sumFacePos;
        glm::vec3 sumEdgePos;
        int count = 0;
        int sharpCount = 0;
        glm::vec3 sharpPos = glm::vec3();

        HalfEdge* now = v->edge;
        do{
            count++;
            if(now->sharp)
            {
                sharpCount++;
                if(sharpCount >= 3)
                {
                    break;
                }
                else
                {
                    sharpPos += now->sym->vert->pos * 0.125f;
                }
            }
            now = now->next;
            sumEdgePos += now->vert->pos;
            sumFacePos += map[now->face]->pos;
            now = now->sym;
        }while(now != v->edge);

        if(sharpCount == 2)
        {
            v->pos = sharpPos + v->pos * 0.75f;
        }
        else if(sharpCount >= 3)
        {
            continue;
        }
        else
        {
            v->pos *= ((float)(count - 2) / (float)count);
            v->pos += (sumEdgePos + sumFacePos) / (float)(count*count);
        }
    }

    // subdivide the original face into four small faces
    vertSet.clear();
    for(int i = 0; i < fSize; i++)
    {
        // only old edge in edgeSet
        Face* oriFace = mesh.faces[i].get();
        Vertex* centerVertex = map[oriFace];
        HalfEdge* beginEdge = oriFace->edge;
        int beginId = beginEdge->id;
        if(edgeSet.find(beginEdge) == edgeSet.end())
        {
            beginEdge = beginEdge->next;
        }

        HalfEdge* now = beginEdge;
        HalfEdge* lastOut = now->FindPrevious();
        HalfEdge* lastNew = nullptr;
        do{
            if(edgeSet.find(now) != edgeSet.end()) // select the old halfEdge
            {
                Face* f;
                if(now->id != beginId)
                {
                    // create new face if the edge is not the begin edge
                    mesh.faces.push_back(std::make_unique<Face>(oriFace->col, now));
                    f = mesh.faces.back().get();
                }
                else{
                    f = oriFace;
                }

                // set edges' information and create new edges for new faces
                HalfEdge* nextEdge = now->next;
                Vertex* v = now->vert;
                if(lastNew != nullptr)
                {
                    lastNew->face = f;
                }
                mesh.edges.push_back(std::make_unique<HalfEdge>(lastNew, nullptr, f, centerVertex));
                HalfEdge* e1 = mesh.edges.back().get();
                mesh.edges.push_back(std::make_unique<HalfEdge>(now->next, e1, nullptr, v));
                lastNew = mesh.edges.back().get();
                e1->sym = lastNew;
                now->next = e1;
                if(now->id == beginId)
                {
                    centerVertex->edge = e1;
                }
                now->face = f;
                lastOut->face = f;

                lastOut = now;
                now = nextEdge;
            }
            else{
                lastOut = now;
                now = now->next;
            }
        }while(now->id != beginId);
        beginEdge->next->next = lastNew;
        lastNew->face = oriFace;
    }

    // set list widget
    for(int i = fSize; i < mesh.faces.size(); i++)
    {
        facesListWidget->addItem(mesh.faces[i].get());
    }
    for(int i = eSize; i < mesh.edges.size(); i++)
    {
        halfEdgesListWidget->addItem(mesh.edges[i].get());
    }
    for(int i = vSize; i < mesh.verts.size(); i++)
    {
        vertsListWidget->addItem(mesh.verts[i].get());
    }

    // update openGL
    mesh.create();
    update();
}

void MyGL::onExtrudeButtonClicked()
{
    if(selection == 2 && m_drawFace.face)
    {
        mesh.extrude(m_drawFace.face);
        mesh.create();
        update();
    }
}

void MyGL::onSharpButtonClicked()
{
    if(selection == 0 && m_drawVertex.vert != nullptr)
    {
        m_drawVertex.vert->sharp = true;
    }
    else if(selection == 1 && m_drawEdge.edge != nullptr)
    {
        m_drawEdge.edge->sharp = true;
        if(m_drawEdge.edge->sym)
        {
            m_drawEdge.edge->sym->sharp = true;
        }
    }
    else if(selection == 2 && m_drawFace.face != nullptr)
    {
        HalfEdge* now = m_drawFace.face->edge;
        do{
            now->sharp = true;
            now->vert->sharp = true;
            now = now->next;
        }while(now != m_drawFace.face->edge);
    }
}

void MyGL::onSkeletonButtonClicked()
{
    if(joints.size() > 0)
    {
        emit sig_removeRootNode();
    }
    joints.clear();

    //std::cout << "load skeleton" << std::endl;
    QString filter = "Json Files (*.json)";
    QString fileName = QFileDialog::getOpenFileName(this, "Select a file...", QDir::currentPath() + "/..", filter);
    //std::cout<<"load "<<fileName.toStdString()<<std::endl;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString str = in.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    if(jsonObj.isEmpty())
        return;
    QJsonObject rootJson = jsonObj["root"].toObject();

    Joint* root = readSkeletonJson(rootJson, nullptr);

    emit sig_sendRootNode(root);
    mesh.skinning = false;
    update();
}

Joint* MyGL::readSkeletonJson(QJsonObject obj, Joint* parent)
{
    QJsonArray posArr = obj["pos"].toArray();
    QJsonArray rotArr = obj["rot"].toArray();
    glm::vec3 pos = glm::vec3(posArr[0].toDouble(), posArr[1].toDouble(), posArr[2].toDouble());
    glm::quat rot = glm::angleAxis((float)rotArr[0].toDouble(), glm::vec3(rotArr[1].toDouble(), rotArr[2].toDouble(), rotArr[3].toDouble()));
    QString name = obj["name"].toString();
    int id = joints.size();
    Joint* j = new Joint(this, name, parent, pos, rot, id);
    joints.push_back(j);
    Joint* now = joints.back();
    QJsonArray childrenArr = obj["children"].toArray();

    //std::cout << "build joint " << name << std::endl;

    for(const QJsonValue& val : childrenArr)
    {
        Joint* child = readSkeletonJson(val.toObject(), now);
        now->children.push_back(child);
        now->addChild(child);
    }

    return now;
}

void MyGL::onSkinningButtonClicked()
{
    if(mesh.faces.empty() || joints.empty())
        return;

    for(int i = 0; i < mesh.verts.size(); i++)
    {
        Vertex* vert = mesh.verts[i].get();
        for(int j = 0; j < joints.size(); j++)
        {
            Joint* joint = joints[j];
            glm::vec3 jointPos = glm::vec3(joint->getOverallTransformation() * glm::vec4(0, 0, 0, 1));
            float len = glm::length(vert->pos - jointPos);
            if(vert->jointIds[0] == -1 || len < vert->jointInfluence[0])
            {
                vert->jointIds[1] = vert->jointIds[0];
                vert->jointIds[0] = joint->id;
                vert->jointInfluence[1] = vert->jointInfluence[0];
                vert->jointInfluence[0] = len;
            }
            else if(vert->jointIds[1] == -1 || len < vert->jointInfluence[1])
            {
                vert->jointIds[1] = joint->id;
                vert->jointInfluence[1] = len;
            }
        }

        float sum = vert->jointInfluence[0] + vert->jointInfluence[1];
        vert->jointInfluence[0] = 1 - vert->jointInfluence[0] / sum;
        vert->jointInfluence[1] = 1 - vert->jointInfluence[1] / sum;
    }

    mesh.skinning = true;
    mesh.create();
    update();
    emit sig_sendSelectedVertex();
}

void MyGL::slot_setSelectedNode(QTreeWidgetItem *i) {
    if(selectedJoint != nullptr)
    {
        selectedJoint->selected = false;
    }
    selectedJoint = static_cast<Joint*>(i);
    selectedJoint->selected = true;
    selectedWeight = .0;
    emit sig_sendSelectedJoint();
    update();
}

void MyGL::slot_setPosX(double x)
{
    if(selection == 0)
    {
        m_drawVertex.vert->pos.x = x;
        mesh.create();
        update();
    }

}
void MyGL::slot_setPosY(double y)
{
    if(selection == 0)
    {
        m_drawVertex.vert->pos.y = y;
        mesh.create();
        update();
    }
}
void MyGL::slot_setPosZ(double z)
{
    if(selection == 0)
    {
        m_drawVertex.vert->pos.z = z;
        mesh.create();
        update();
    }
}
void MyGL::slot_setFaceRed(double r)
{
    if(selection == 2)
    {
        m_drawFace.face->col.x = r;
        mesh.create();
        update();
    }
}
void MyGL::slot_setFaceGreen(double g)
{
    if(selection == 2)
    {
        m_drawFace.face->col.y = g;
        mesh.create();
        update();
    }
}
void MyGL::slot_setFaceBlue(double b)
{
    if(selection == 2)
    {
        m_drawFace.face->col.z = b;
        mesh.create();
        update();
    }
}

void MyGL::onRotXButtonClicked()
{
    if(selectedJoint != nullptr)
    {
        selectedJoint->rot = glm::normalize(glm::rotate(selectedJoint->rot, 5.0f * 3.14159f / 180.f, glm::vec3(1, 0, 0)));
        update();
        emit sig_sendSelectedJoint();
    }
}
void MyGL::onRotYButtonClicked()
{
    if(selectedJoint != nullptr)
    {
        selectedJoint->rot = glm::normalize(glm::rotate(selectedJoint->rot, 5.0f * 3.14159f / 180.f, glm::vec3(0, 1, 0)));
        update();
        emit sig_sendSelectedJoint();
    }
}
void MyGL::onRotZButtonClicked()
{
    if(selectedJoint != nullptr)
    {
        selectedJoint->rot = glm::normalize(glm::rotate(selectedJoint->rot, 5.0f * 3.14159f / 180.f, glm::vec3(0, 0, 1)));
        update();
        emit sig_sendSelectedJoint();
    }
}
void MyGL::setJointPosX(double posX)
{
    if(selectedJoint != nullptr)
    {
        selectedJoint->pos.x = posX;
        update();
    }
}
void MyGL::setJointPosY(double posY)
{
    if(selectedJoint != nullptr)
    {
        selectedJoint->pos.y = posY;
        update();
    }
}
void MyGL::setJointPosZ(double posZ)
{
    if(selectedJoint != nullptr)
    {
        selectedJoint->pos.z = posZ;
        update();
    }
}

void MyGL::setJointWeight(double weight)
{
    selectedWeight = weight;
}

void MyGL::replaceJoint()
{
    if(selection == 0 && mesh.skinning)
    {
        std::cout << "replace joint" << std::endl;
        Vertex* vert = m_drawVertex.vert;
        vert->jointIds[selectedJointIdx] = selectedJoint->id;
        vert->jointInfluence[selectedJointIdx] = selectedWeight;
        vert->jointInfluence[1 - selectedJointIdx] = 1.0 - selectedWeight;
        mesh.create();
        update();
    }
}

void MyGL::setJointReplaceSelection(int idx)
{
    selectedJointIdx = idx;
}

std::array<QString, 2> MyGL::getSelectedVertexJointNames()
{
    if(selection != 0 || joints.empty() || !mesh.skinning)
    {
        return {"Unknown", "Unknown"};
    }
    return {joints[m_drawVertex.vert->jointIds[0]]->name, joints[m_drawVertex.vert->jointIds[1]]->name};
}

bool MyGL::checkJointSkinning()
{
    return mesh.skinning;
}

bool CheckIntersection(std::array<glm::vec3, 3> verts, std::array<glm::vec3, 2> rayArr, float& t, glm::vec3& intersection)
{
    glm::vec3 nor = glm::normalize(glm::cross(verts[1] - verts[0], verts[2] - verts[0]));

    glm::vec3 norCross = rayArr[0] - nor * glm::dot(verts[0] - rayArr[0], -nor);

    t = glm::length(norCross - rayArr[0]) / glm::dot(rayArr[1], -nor);
    if(t < 0 || std::isinf(t)){
        return false;
    }

    intersection = rayArr[0] + rayArr[1] * t;
    float s = GetArea(verts[0], verts[1], verts[2]);
    float s0 = GetArea(intersection, verts[1], verts[2]);
    float s1 = GetArea(intersection, verts[2], verts[0]);
    float s2 = GetArea(intersection, verts[0], verts[1]);

    if(s0 + s1 + s2 > s * 1.01)
    {
        return false;
    }

    return true;
}


glm::vec3 SpiltString(std::string str)
{
    int a = -1, b = -1;
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i]==' ')
        {
            if(a == -1)
            {
                a = i;
            }
            else
            {
                b = i;
                break;
            }
        }
    }
    glm::vec3 vec = glm::vec3(
                std::stof(str.substr(0, a)),
                std::stof(str.substr(a+1, b - a - 1)),
                std::stof(str.substr(b+1, str.length() - b - 1)));
    return vec;
}

void GetFaceIdx(std::vector<int>& idx, std::string str)
{
    int i = 0;
    while(i<str.length())
    {
        if(str[i] == ' ')
        {
            int j = i+1;
            while(str[j]!='/')
            {
                j++;
            }
            idx.push_back(std::stoi(str.substr(i+1, j - i - 1)) - 1);
            i = j;
        }
        else{
            i++;
        }
    }
}

glm::vec3 RandomColor()
{
    return glm::vec3(
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)
                );
}

float GetArea(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2)
{
    float val = 0.5f * glm::length(glm::cross(p0 - p1, p2 - p1));
    if(std::isnan(val))
    {
        return 0;
    }
    return val;
}
