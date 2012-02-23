#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
