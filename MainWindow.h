#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <boost/thread.hpp>
#include "CapCambus.h"
#include "KinectControl.h"
#include "GLWidget.h"
#include "SoundDefine.h"

using namespace Eigen;

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
    void on_horizontalSlider_Brightness_valueChanged(int value);
    void on_horizontalSlider_Gain_valueChanged(int value);
    void on_horizontalSlider_Exposure_valueChanged(int value);
    void on_pushButton_ServerSet_clicked();
    void on_pushButton_Openning_clicked();
    void on_pushButton_SetNear_clicked();
    void on_pushButton_SetFar_clicked();
    void on_pushButton_SetBackground_clicked();
    void on_pushButton_SetLeft_clicked();
    void on_pushButton_SetRight_clicked();
    void receivePixel(int h, int s, int v, Point2i pos);
    void on_horizontalSlider_H_valueChanged(int value);
    void on_horizontalSlider_S_valueChanged(int value);
    void on_horizontalSlider_V_valueChanged(int value);

signals:
    void sendUserPosition(Point3d pos);
    void sendHSVThreshold(int h, int s, int v);

private:
    bool kinect_stop;
    void getKinect();

    void sendHSV();

    Ui::MainWindow *ui;
    CapCambus cap_cambus;
    KinectControl kinect;
    GLWidget viewer;
    SoundDefine sound;

    mutex thread_sync;

    int node;

    vector<vector<XnPoint3D> > skeleton;
    Point3d user_pos;

    double user_near;
    double user_far;
    double user_left;
    double user_right;
};

#endif // MAINWINDOW_H
