#include "CapCambus.h"

CapCambus::CapCambus()
    : cap_stop(false)
{
}

CapCambus::~CapCambus()
{
    cap.release();
}

void CapCambus::startCap()
{
    cap.open(0);
    if(cap.isOpened())
        boost::thread cap_th(bind(&CapCambus::capture, this));
}

void CapCambus::startPlay(const string video)
{
    cap.open(video);
    if(cap.isOpened())
    {
        boost::thread vid_th(bind(&CapCambus::player, this));
    }
}

Mat CapCambus::getCambus()
{
    if(!cambus.empty())
        return cambus;
}

void CapCambus::capStop()
{
    cap_stop = true;
}

void CapCambus::capture()
{
    namedWindow("Capture");
    cvSetMouseCallback("Capture", _click, this);
    while(1)
    {
        Mat img;
        cap >> img;
        resize(img, img, Size(), 0.5, 0.5);

        if(!pers_mat.empty())
        {
            // 切り出した画像
            warpPerspective(img, cambus, pers_mat, pers_size);
            resize(cambus, cambus, Size(325, 550));
            imshow("Cambus", cambus);

            // 処理
            interCambus();
        }

        // 長方形描画
        if(cambus_rect.size() == 4)
        {
            line(img, cambus_rect.at(0), cambus_rect.at(1), Scalar(255, 0, 0), 1);
            line(img, cambus_rect.at(1), cambus_rect.at(2), Scalar(255, 0, 0), 1);
            line(img, cambus_rect.at(2), cambus_rect.at(3), Scalar(255, 0, 0), 1);
            line(img, cambus_rect.at(3), cambus_rect.at(0), Scalar(255, 0, 0), 1);
        }

        imshow("Capture", img);

        if(cap_stop)
            break;
    }
}

void CapCambus::player()
{
    while(1)
    {
        Mat img;
        cap >> img;
        imshow("Video", img);

        if(cap_stop)
            break;
    }
}

void CapCambus::click(const int event, const int x, const int y, const int flags)
{
    if(event == CV_EVENT_LBUTTONDOWN)
    {
        if(cambus_rect.size() < 4)
        {
            Point pos(x, y);
            cambus_rect.push_back(pos);

            if(cambus_rect.size() == 4)
            {
                // 入力された4点
                Point2f src[4];
                src[0] = Point2f(cambus_rect.at(0).x, cambus_rect.at(0).y);
                src[1] = Point2f(cambus_rect.at(1).x, cambus_rect.at(1).y);
                src[2] = Point2f(cambus_rect.at(2).x, cambus_rect.at(2).y);
                src[3] = Point2f(cambus_rect.at(3).x, cambus_rect.at(3).y);

                // 変換後の4点
                float w = ((src[1].x - src[0].x) + (src[2].x - src[3].x));
                float h = ((src[3].y - src[0].y) + (src[2].y - src[1].y));
                Point2f dst[4];
                dst[0] = Point2f(0.0, 0.0);
                dst[1] = Point2f(w, 0.0);
                dst[2] = Point2f(w, h);
                dst[3] = Point2f(0.0, h);

                // 透視投影変換
                pers_mat = getPerspectiveTransform(src, dst);
                pers_size = Size((int)w, (int)h);
            }
        }
        else
        {
            cambus_rect.clear();

        }
    }
}

void CapCambus::interCambus()
{
    // CIEL*a*b*
    Mat lab;
    cvtColor(cambus, lab, CV_BGR2Lab);

    calcZipf();
}

void CapCambus::calcZipf()
{
}
