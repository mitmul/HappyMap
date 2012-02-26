#ifndef SOUNDDEFINE_H
#define SOUNDDEFINE_H

#include <QWidget>
#include <boost/thread.hpp>
#include <opencv.hpp>
#include "OSCSender.h"
#include "OSCReceiver.h"

using namespace boost;
using namespace cv;

class SoundDefine : public QWidget
{
    Q_OBJECT

public:
    explicit SoundDefine(QWidget *parent = 0);
    ~SoundDefine();

    void setHostPort(const string host, const int port);
    void setNear(const double near);
    void setFar(const double far);
    void setLeft(const double left);
    void setRight(const double right);

    // Openning
    void playOpenning();
    void setOpenningRLPF(const int freq);

    // Rhythm
    void startRhythm();
    void resonzRhythm(const double freq, const double rate);
    void panRhythm(const double pan);

public slots:
    void receiveUserPosition(Point3d pos);

signals:


private:
    double getUserDistance(const int norm_min, const int norm_max);
    double getUserHorizontalPos();

    mutex thread_sync;
    bool openning_stop;
    void openning();

    OSCSender sender;
    OSCReceiver receiver;

    Point3d user_pos;
    double user_near;
    double user_far;
    double user_left;
    double user_right;

    bool beat_start;
};

#endif // SOUNDDEFINE_H
