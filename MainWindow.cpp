#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    kinectStop(false),
    node(1000)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    cap_cambus.capStop();
    delete ui;
}

void MainWindow::on_pushButton_CameraStart_clicked()
{
    cap_cambus.startCap();
}

void MainWindow::on_pushButton_VideoStart_clicked()
{
    cap_cambus.startPlay("drawing.avi");
}

void MainWindow::on_pushButton_KinectStart_clicked()
{
    boost::thread kinect_th(bind(&MainWindow::getKinect, this));
}

void MainWindow::getKinect()
{
    string host = ui->lineEdit_OSCServerIP->text().toStdString();
    int port = ui->lineEdit_OSCServerPort->text().toInt();
    sender.setServer(host, port);

    while(!kinectStop)
    {
        Mat depth_img = kinect.getDepthImage();
        XnPoint3D hand_pos = kinect.getHandPos();

        circle(depth_img,Point(hand_pos.X, hand_pos.Y), 5, Scalar(255, 255, 255), -1);
        imshow("depth", depth_img);

        sender.setAddress("/s_new");
        sender.addString("newRing");
        sender.addInt(node);
        sender.addInt(0);
        sender.addInt(1);
        sender.send();
    }
}
