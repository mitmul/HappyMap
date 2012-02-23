#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CapCambus.h"
#include "OSCSender.h"
#include "OSCReceiver.h"

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

private:
    Ui::MainWindow *ui;
    CapCambus cap_cambus;
    OSCSender sender;
    OSCReceiver receiver;
};

#endif // MAINWINDOW_H
