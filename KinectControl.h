#ifndef DEPTH_H
#define DEPTH_H

// std
#include <iostream>
#include <string>
#include <stdexcept>

// boost
#include <boost/format.hpp>

// OpenNI + NITE
#include <XnCppWrapper.h>
#include <XnVNite.h>

using namespace std;
using namespace boost;
using namespace xn;

class KinectControl
{
public:
    KinectControl();

    void Init();

    // コールバック関数
    XnCallbackHandle user_callbacks;
    void NewUser(UserGenerator& generator, XnUserID nId);
    void LostUser(UserGenerator& generator, XnUserID nId);

    XnDepthPixel* getDepth();
    std::vector<std::vector<XnPoint3D> > getSkeleton();

private:
    Context context;
    DepthGenerator depth_generator;
    XnMapOutputMode map_mode;
    UserGenerator user_generator;
    HandsGenerator hand_generator;
    SkeletonCapability skeleton;
    PoseDetectionCapability pose;

    void CHECK_RC(XnStatus nRetVal, const char* what);
};


#endif // DEPTH_H
