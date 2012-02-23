#ifndef CAPCAMBUS_H
#define CAPCAMBUS_H

#include <vector>
#include <opencv.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace cv;
using namespace boost;

class CapCambus
{
public:
    CapCambus();
    ~CapCambus();

    void startCap();
    void startPlay(const string video);
    Mat getCambus();
    void capStop();

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

    VideoCapture cap;
    Mat cambus;

    vector<Point> cambus_rect;
    Mat pers_mat;
    Size pers_size;
};

#endif // CAPCAMBUS_H
