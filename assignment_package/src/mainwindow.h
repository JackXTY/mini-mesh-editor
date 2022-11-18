#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();
    void on_actionCamera_Controls_triggered();
    void slot_addRootToTreeWidget(QTreeWidgetItem *i);
    void slot_removeRootToTreeWidget();
    void slot_updateJointInfo();
    void slot_setComboBox();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
