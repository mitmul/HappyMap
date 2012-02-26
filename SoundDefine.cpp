#include "SoundDefine.h"

SoundDefine::SoundDefine(QWidget *parent)
    : QWidget(parent),
      openning_stop(false),
      beat_start(false)
{
}

SoundDefine::~SoundDefine()
{
    openning_stop = true;
}

void SoundDefine::setHostPort(const string host, const int port)
{
    sender.setServer(host, port);
}

void SoundDefine::setNear(const double near)
{
    user_near = near;
}

void SoundDefine::setFar(const double far)
{
    user_far = far;
}

void SoundDefine::setLeft(const double left)
{
    user_left = left;
}

void SoundDefine::setRight(const double right)
{
    user_right = right;
}

void SoundDefine::playOpenning()
{
    try
    {
        sender.setAddress("/openning");
        sender.send();

        boost::thread openning_th(bind(&SoundDefine::openning, this));
    }
    catch(std::exception &e)
    {
        cerr << "SoundDefine::playOpenning:" << endl
             << e.what() << endl;
    }
}

void SoundDefine::setOpenningRLPF(const int freq)
{
    try
    {
        sender.setAddress("/openning_freq");
        sender.addString("freq");
        sender.addInt(freq);
        sender.send();
    }
    catch(std::exception &e)
    {
        cerr << "SoundDefine::setOpenningRLPF:" << endl
             << e.what() << endl;
    }
}

void SoundDefine::startRhythm()
{
    beat_start = true;
    sender.setAddress("/beat1");
    sender.send();
}

void SoundDefine::resonzRhythm(const double freq, const double rate)
{
    sender.setAddress("/filter_freq_rate");
    sender.addFloat(freq);
    sender.addFloat(rate);
    sender.send();
}

void SoundDefine::panRhythm(const double pan)
{
    sender.setAddress("/beat1_pan");
    sender.addFloat(pan);
    sender.send();
}

void SoundDefine::receiveUserPosition(Point3d pos)
{
    try
    {
//        mutex::scoped_lock lock(thread_sync);

        user_pos = pos;

        if(beat_start)
        {
            // Resonance
            double freq = getUserDistance(300, 3000);
            resonzRhythm(freq, 0.1);

            // Pan
            double pan = getUserHorizontalPos();
            panRhythm(pan);
        }
    }
    catch(std::exception &e)
    {
        cerr << "SoundDefine::receiveUserPosition:" << endl
             << e.what() << endl;
    }
}

double SoundDefine::getUserDistance(const int norm_min, const int norm_max)
{
    try
    {
        double range = user_far - user_near;
        double userp = user_pos.z - user_near;
        userp /= range;
        userp *= norm_max;
        userp += norm_min;

        return userp;
    }
    catch(std::exception &e)
    {
        cerr << "SoundDefine::getUserDistance:" << endl
             << e.what() << endl;
    }
}

double SoundDefine::getUserHorizontalPos()
{
    try
    {
        double wide = user_right - user_left;
        double pan_pos = user_pos.x - user_left;
        pan_pos /= wide;
        pan_pos *= 2.0;
        pan_pos -= 1.0;

        return pan_pos;
    }
    catch(std::exception &e)
    {
        cerr << "SoundDefine::getUserHorizontalPos:" << endl
             << e.what() << endl;
    }
}

void SoundDefine::openning()
{
    try
    {
        int time = 0;

        while(!openning_stop && time < 5000)
        {
            double userp = getUserDistance(10, 550);

            if(userp < 10)
                userp = 10;
            if(userp > 550)
                userp = 550;

            if(userp == 550)
                ++time;
            else
            {
                cout << "time reset" << endl;
                time = 0;
            }

            setOpenningRLPF((int)userp);

            usleep(3333);
        }

        // フェードアウト
        sender.setAddress("/openning_stop");
        sender.send();

        // ビート開始
        startRhythm();
    }
    catch(std::exception &e)
    {
        cerr << "SoundDefine::openning:" << endl
             << e.what() << endl;
    }
}
