#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include "iostream"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    connect(ui->loadButton, SIGNAL(clicked()),
            ui->mygl, SLOT(on_actionLoadOBJ_triggered()));

    connect(ui->splitEdgeButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onSplitEdgeButtonClicked()));

    connect(ui->triangulateButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onTriangulateButtonClicked()));

    connect(ui->subdivideButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onSubdivideButtonClicked()));

    connect(ui->extrudeButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onExtrudeButtonClicked()));

    connect(ui->sharpButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onSharpButtonClicked()));

    ui->mygl->vertsListWidget = ui->vertsListWidget;
    ui->mygl->halfEdgesListWidget = ui->halfEdgesListWidget;
    ui->mygl->facesListWidget = ui->facesListWidget;

    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        ui->mygl, SLOT(onListItemClicked(QListWidgetItem*)));

    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        ui->mygl, SLOT(onListItemClicked(QListWidgetItem*)));

    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        ui->mygl, SLOT(onListItemClicked(QListWidgetItem*)));

    connect(ui->skeletonButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onSkeletonButtonClicked()));

    connect(ui->skinningButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onSkinningButtonClicked()));

    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setPosX(double)));
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setPosY(double)));
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setPosZ(double)));
    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setFaceRed(double)));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setFaceGreen(double)));
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_setFaceBlue(double)));

    connect(ui->mygl, SIGNAL(sig_sendRootNode(QTreeWidgetItem*)),
           this, SLOT(slot_addRootToTreeWidget(QTreeWidgetItem*)));

    connect(ui->mygl, SIGNAL(sig_removeRootNode()),
           this, SLOT(slot_removeRootToTreeWidget()));

    connect(ui->jointTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                ui->mygl, SLOT(slot_setSelectedNode(QTreeWidgetItem*)));

    connect(ui->mygl, SIGNAL(sig_sendSelectedJoint()),
            this, SLOT(slot_updateJointInfo()));

    connect(ui->rotXButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onRotXButtonClicked()));
    connect(ui->rotYButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onRotYButtonClicked()));
    connect(ui->rotZButton, SIGNAL(clicked()),
            ui->mygl, SLOT(onRotZButtonClicked()));
    connect(ui->tranXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(setJointPosX(double)));
    connect(ui->tranYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(setJointPosY(double)));
    connect(ui->tranZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(setJointPosZ(double)));

    connect(ui->weightSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(setJointWeight(double)));
    connect(ui->jointComboBox, SIGNAL(currentIndexChanged(int)),
            ui->mygl, SLOT(setJointReplaceSelection(int)));
    connect(ui->replaceJointButton, SIGNAL(clicked()),
            ui->mygl, SLOT(replaceJoint()));
    connect(ui->mygl, SIGNAL(sig_sendSelectedVertex()),
            this, SLOT(slot_setComboBox()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::slot_addRootToTreeWidget(QTreeWidgetItem *i) {
    ui->jointTreeWidget->addTopLevelItem(i);
}

void MainWindow::slot_removeRootToTreeWidget() {
    ui->jointTreeWidget->takeTopLevelItem(0);
}

void MainWindow::slot_updateJointInfo()
{
    //update translation spinbox & rotation label
    Joint* joint = ui->mygl->selectedJoint;
    if(joint != nullptr)
    {
        ui->tranXSpinBox->setValue(joint->pos.x);
        ui->tranYSpinBox->setValue(joint->pos.y);
        ui->tranZSpinBox->setValue(joint->pos.z);
        glm::vec3 angles = glm::eulerAngles(joint->rot);
        ui->rotXlabel->setText("RotX: " + QString::number(std::round(angles.x * 180.0f / 3.14169)));
        ui->rotYlabel->setText("RotY: " + QString::number(std::round(angles.y * 180.0f / 3.14169)));
        ui->rotZlabel->setText("RotZ: " + QString::number(std::round(angles.z * 180.0f / 3.14169)));
    }
}

void MainWindow::slot_setComboBox()
{
    if(ui->mygl->checkJointSkinning())
    {
        std::array<QString, 2> names = ui->mygl->getSelectedVertexJointNames();
        ui->jointComboBox->setItemText(0, names[0]);
        ui->jointComboBox->setItemText(1, names[1]);
    }
}
