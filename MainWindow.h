#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <boost/thread.hpp>
#include "CapCambus.h"
#include "OSCSender.h"
#include "OSCReceiver.h"
#include "KinectControl.h"
#include "GLWidget.h"

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
    void on_pushButton_CameraStart_clicked();
    void on_pushButton_VideoStart_clicked();
    void on_pushButton_KinectStart_clicked();

private:
    bool kinectStop;
    void getKinect();

    Ui::MainWindow *ui;
    CapCambus cap_cambus;
    OSCSender sender;
    OSCReceiver receiver;
    KinectControl kinect;
    GLWidget viewer;

    int node;
};

#endif // MAINWINDOW_H
