#include "CapCambus.h"
CapCambus::CapCambus(QWidget *parent) :
    QWidget(parent),
    cap_stop(false)
{
}
CapCambus::~CapCambus()
{
    cap.release();
}
void CapCambus::startCap()
{
    try
    {
        cap.open(0);
        if(cap.isOpened())
            boost::thread cap_th(bind(&CapCambus::capture, this));
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::startCap:" << endl
             << e.what() << endl;
    }
}
void CapCambus::startPlay(const string video)
{
    try
    {
        cap.open(video);
        if(cap.isOpened())
            boost::thread vid_th(bind(&CapCambus::player, this));
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::startPlay:" << endl
             << e.what() << endl;
    }
}
Mat CapCambus::getCambus()
{
    try
    {
        mutex::scoped_lock lock(thread_sync);

        if(!cambus.empty())
            return cambus;
    }
    catch(std::exception &e)
    {
        cerr<< "CapCambus::getCambus:" << endl
            << e.what() << endl;
    }
}
void CapCambus::capStop()
{
    cap_stop = true;
}
void CapCambus::setBrightness(const int bright)
{
    try
    {
        mutex::scoped_lock lock(thread_sync);

        if(cap.isOpened())
        {
            // BRIGHTNESS: -128 to 128
            if(bright >= -128 && bright <= 128)
                cap.set(CV_CAP_PROP_BRIGHTNESS, bright);
        }
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::setBrightness:" << endl
             << e.what() << endl;
    }
}
void CapCambus::setGain(const int gain)
{
    try
    {
        mutex::scoped_lock lock(thread_sync);

        if(cap.isOpened())
        {
            // GAIN: -128 to 128
            if(gain >= -128 && gain <= 128)
                cap.set(CV_CAP_PROP_GAIN, gain);
        }
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::setGain:" << endl
             << e.what() << endl;
    }
}
void CapCambus::setExposure(const int exposure)
{
    try
    {
        mutex::scoped_lock lock(thread_sync);

        if(cap.isOpened())
        {
            // EXPOSURE: -128 to 128
            if(exposure >= -128 && exposure <= 128)
                cap.set(CV_CAP_PROP_EXPOSURE, exposure);
        }
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::setExposure:" << endl
             << e.what() << endl;
    }
}
void CapCambus::setBackground()
{
    try
    {
        mutex::scoped_lock lock(thread_sync);

        if(!cambus.empty())
        {
            back = cambus.clone();
            imshow("Background", back);
        }
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::setBackgound:" << endl
             << e.what() << endl;
    }
}
void CapCambus::capture()
{
    namedWindow("Capture");
    cvSetMouseCallback("Capture", _click, this);

    while(1)
    {
        try
        {
            mutex::scoped_lock lock(thread_sync);

            Mat img, resized;
            cap >> img;
            cv::resize(img, resized, Size(), SHOW_SCALE, SHOW_SCALE);

            if(!pers_mat.empty())
            {
                // 切り出した画像
                Size size(pers_size.width / SHOW_SCALE, pers_size.height / SHOW_SCALE);
                Size mini(pers_size.width * SHOW_SCALE, pers_size.height * SHOW_SCALE);
                warpPerspective(img, cambus, pers_mat, size);
                cv::resize(cambus, cambus, mini);
                imshow("Cambus", cambus);

                // 処理
                interCambus();
            }

            // 長方形描画
            if(cambus_rect.size() == 4)
            {
                line(resized, cambus_rect.at(0), cambus_rect.at(1), Scalar(255, 0, 0), 1);
                line(resized, cambus_rect.at(1), cambus_rect.at(2), Scalar(255, 0, 0), 1);
                line(resized, cambus_rect.at(2), cambus_rect.at(3), Scalar(255, 0, 0), 1);
                line(resized, cambus_rect.at(3), cambus_rect.at(0), Scalar(255, 0, 0), 1);
            }
            if(maskimg.empty())
                imshow("Capture", resized);
            if(cap_stop)
                break;
        }
        catch(std::exception &e)
        {
            cerr << "CapCambus::capture:" << endl
                 << e.what() << endl;
        }
    }
}
void CapCambus::player()
{
    while(1)
    {
        mutex::scoped_lock lock(thread_sync);

        Mat img;
        cap >> img;
        imshow("Video", img);

        if(cap_stop)
            break;
    }
}

void CapCambus::click(const int event, const int x, const int y, const int flags)
{
    try
    {
        mutex::scoped_lock lock(thread_sync);

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
                    src[0] = Point2f(cambus_rect.at(0).x / SHOW_SCALE, cambus_rect.at(0).y / SHOW_SCALE);
                    src[1] = Point2f(cambus_rect.at(1).x / SHOW_SCALE, cambus_rect.at(1).y / SHOW_SCALE);
                    src[2] = Point2f(cambus_rect.at(2).x / SHOW_SCALE, cambus_rect.at(2).y / SHOW_SCALE);
                    src[3] = Point2f(cambus_rect.at(3).x / SHOW_SCALE, cambus_rect.at(3).y / SHOW_SCALE);

                    // 変換後の4点
                    float w = ((src[1].x - src[0].x) + (src[2].x - src[3].x)) / SHOW_SCALE;
                    float h = ((src[3].y - src[0].y) + (src[2].y - src[1].y)) / SHOW_SCALE;
                    Point2f dst[4];
                    dst[0] = Point2f(0.0, 0.0);
                    dst[1] = Point2f(w, 0.0);
                    dst[2] = Point2f(w, h);
                    dst[3] = Point2f(0.0, h);

                    // 透視投影変換
                    pers_mat = getPerspectiveTransform(src, dst);
                    pers_size = Size((int)w * SHOW_SCALE, (int)h * SHOW_SCALE);
                }
            }
            else
            {
                cambus_rect.clear();
            }
        }
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::click:" << endl
             << e.what() << endl;
    }
}

void CapCambus::interCambus()
{
    calcZipf();
    calcMask();

    try
    {
        if(!maskimg.empty())
        {
//            // HSV変換
//            Mat hsv, masked;
//            cvtColor(cambus, hsv, CV_BGR2HSV);

//            imshow("hsv", hsv);

//            hsv.copyTo(masked, maskimg);

//            for(int i = 0; i < hsv.rows; ++i)
//            {
//                for(int j = 0; j < hsv.cols; ++j)
//                {
//                    if(maskimg.at<uchar>(i, j) != 0)
//                    {
//                        Vec3b p = hsv.at<Vec3b>(i, j);
//                        emit sendPixel(p[0], p[1], p[2], Point2i(j, i));
//                    }
//                }
//            }
        }
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::interCambus:" << endl
             << e.what() << endl;
    }
}

void CapCambus::calcZipf()
{
}

void CapCambus::calcMask()
{
    try
    {
        if(!back.empty() && !cambus.empty())
        {
            Mat back_gray, cambus_gray;
            cvtColor(back, back_gray, CV_BGR2GRAY);
            cvtColor(cambus, cambus_gray, CV_BGR2GRAY);

            //GaussianBlur(back_gray, back_gray, Size(5, 5), 0);
            //GaussianBlur(cambus_gray, cambus_gray, Size(5, 5), 0);

            bitwise_xor(cambus_gray, back_gray, maskimg);
            threshold(maskimg, maskimg, 0, 255, CV_THRESH_OTSU);
            //erode(maskimg, maskimg, Mat());
            //dilate(maskimg, maskimg, Mat());
            imshow("Mask", maskimg);

            destroyWindow("Capture");
        }
    }
    catch(std::exception &e)
    {
        cerr << "CapCambus::calcMask:" << endl
             << e.what() << endl;
    }
}

void CapCambus::receiveHSVThreshold(int h, int s, int v)
{
    if(!maskimg.empty())
    {
        // HSV変換
        Mat hsv, masked;
        cvtColor(cambus, hsv, CV_BGR2HSV);

        vector<Mat> hsv_split;
        split(hsv, hsv_split);

        vector

        imshow("hsv", hsv);
    }
}
