#ifndef CAPCAMBUS_H
#define CAPCAMBUS_H

#include <QWidget>
#include <vector>
#include <opencv.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#define SHOW_SCALE 0.5

using namespace std;
using namespace cv;
using namespace boost;

class CapCambus : public QWidget
{
    Q_OBJECT

public:
    CapCambus(QWidget *parent = 0);
    ~CapCambus();

    void startCap();
    void startPlay(const string video);
    Mat getCambus();
    void capStop();

    void setBrightness(const int bright);
    void setGain(const int gain);
    void setExposure(const int exposure);

    void setBackground();

private:
    bool cap_stop;
    void capture();
    void player();

    static void _click(int event, int x, int y, int flags, void* param)
    {
        ((CapCambus*)param)->click(event, x, y, flags);
    }
    void click(const int event, const int x, const int y, const int flags);

    void interCambus();
    void calcZipf();
    void calcMask();

    mutex thread_sync;
    VideoCapture cap;
    Mat cambus;
    Mat back;
    Mat maskimg;

    vector<Point> cambus_rect;
    Mat pers_mat;
    Size pers_size;

signals:
    void sendPixel(int h, int s, int v, Point2i pos);

public slots:
    void receiveHSVThreshold(int h, int s, int v);

};

#endif // CAPCAMBUS_H
