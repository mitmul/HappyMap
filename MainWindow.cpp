#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    kinect_stop(false),
    node(1000)
{
    ui->setupUi(this);

    qRegisterMetaType<Point3d>("Point3d");
    connect(this, SIGNAL(sendUserPosition(Point3d)), &sound, SLOT(receiveUserPosition(Point3d)));

    qRegisterMetaType<Point2i>("Point2i");
    connect(&cap_cambus, SIGNAL(sendPixel(int,int,int,Point2i)), this, SLOT(receivePixel(int,int,int,Point2i)));
}

MainWindow::~MainWindow()
{
    kinect_stop = true;
    kinect.~KinectControl();

    sound.~SoundDefine();

    cap_cambus.capStop();
    cap_cambus.~CapCambus();

    delete ui;
}

void MainWindow::on_pushButton_CameraStart_clicked()
{
    cap_cambus.startCap();
}

void MainWindow::on_pushButton_VideoStart_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Open Video File", QDir::currentPath(), "Movie file(*.mov)");
    if(!file.isEmpty())
        cap_cambus.startPlay(file.toStdString());
}

void MainWindow::on_pushButton_KinectStart_clicked()
{
    boost::thread kinect_th(bind(&MainWindow::getKinect, this));
}

void MainWindow::getKinect()
{
    while(!kinect_stop)
    {
        try
        {
            mutex::scoped_lock lock(thread_sync);

            Mat depth_img = kinect.getDepthImage();

            skeleton = kinect.getSkeleton();
            for(int i = 0; i < skeleton.size(); ++i)
            {
                circle(depth_img, Point(skeleton.at(i).at(3).X, skeleton.at(i).at(3).Y), 5, Scalar(255, 255, 255), -1);
                putText(depth_img, lexical_cast<string>(i), Point(skeleton.at(i).at(3).X, skeleton.at(i).at(3).Y), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255));
            }
            ui->lineEdit_UserNum->setText(QString::number(skeleton.size()));

            // ユーザまでの距離
            if(!skeleton.empty())
            {
                XnPoint3D real = kinect.getRealValue(skeleton.at(ui->lineEdit_UserID->text().toInt()).at(3));
                user_pos.x = real.X;
                user_pos.y = real.Y;
                user_pos.z = real.Z;
                emit sendUserPosition(user_pos);
            }

            imshow("depth", depth_img);
        }
        catch(std::exception &e)
        {
            cerr << "MainWindow::getKinect:" << endl
                 << e.what() << endl;
        }
    }
}

void MainWindow::sendHSV()
{
    int h = ui->horizontalSlider_H->value();
    int s = ui->horizontalSlider_S->value();
    int v = ui->horizontalSlider_V->value();

    emit sendHSVThreshold(h, s, v);
}

void MainWindow::on_horizontalSlider_Brightness_valueChanged(int value)
{
    cap_cambus.setBrightness(value);
}

void MainWindow::on_horizontalSlider_Gain_valueChanged(int value)
{
    cap_cambus.setGain(value);
}

void MainWindow::on_horizontalSlider_Exposure_valueChanged(int value)
{
    cap_cambus.setExposure(value);
}

void MainWindow::on_pushButton_ServerSet_clicked()
{
    string host = ui->lineEdit_OSCServerIP->text().toStdString();
    int port = ui->lineEdit_OSCServerPort->text().toInt();

    sound.setHostPort(host, port);
}

void MainWindow::on_pushButton_Openning_clicked()
{
    sound.playOpenning();
}

void MainWindow::on_pushButton_SetNear_clicked()
{
    user_near = user_pos.z;
    ui->lineEdit_UserNear->setText(QString::number(user_near));

    sound.setNear(user_near);
}

void MainWindow::on_pushButton_SetFar_clicked()
{
    user_far = user_pos.z;
    ui->lineEdit_UserFar->setText(QString::number(user_far));

    sound.setFar(user_far);
}

void MainWindow::on_pushButton_SetBackground_clicked()
{
    cap_cambus.setBackground();
}

void MainWindow::on_pushButton_SetLeft_clicked()
{
    user_left = user_pos.x;
    ui->lineEdit_UserLeft->setText(QString::number(user_left));

    sound.setLeft(user_left);
}

void MainWindow::on_pushButton_SetRight_clicked()
{
    user_right = user_pos.x;
    ui->lineEdit_UserRight->setText(QString::number(user_right));

    sound.setRight(user_right);
}

void MainWindow::receivePixel(int h, int s, int v, Point2i pos)
{

}

void MainWindow::on_horizontalSlider_H_valueChanged(int value)
{
    sendHSV();
}

void MainWindow::on_horizontalSlider_S_valueChanged(int value)
{
    sendHSV();
}

void MainWindow::on_horizontalSlider_V_valueChanged(int value)
{
    sendHSV();
}
