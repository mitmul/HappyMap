#ifndef DEPTH_H
#define DEPTH_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/format.hpp>
#include <XnCppWrapper.h>
#include <opencv.hpp>

using namespace std;
using namespace boost;
using namespace xn;
using namespace cv;

class KinectControl
{
public:
    KinectControl();

    void Init();

    Mat getDepthImage();
    XnPoint3D getHandPos();

    XnDepthPixel* getDepth();
    std::vector<std::vector<XnPoint3D> > getSkeleton();

    // コールバック関数
    XnCallbackHandle user_callbacks;
    void NewUser(UserGenerator& generator, XnUserID nId);
    void LostUser(UserGenerator& generator, XnUserID nId);

    XnCallbackHandle start_callback, end_callback, pose_callback;
    void CalibrationStart(SkeletonCapability& skeleton, XnUserID user);
    void CalibrationEnd(SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus);
    void PoseDetected(PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user);

    XnCallbackHandle hand_callback;
    void HandCreate(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime);
    void HandUpdate(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime);
    void HandDestroy(HandsGenerator& generator, XnUserID nId, XnFloat fTime);

    void GestureRecognized(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition);
    void GestureProcess(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress);

private:
    Context context;
    XnMapOutputMode map_mode;
    DepthGenerator depth_generator;
    UserGenerator user_generator;
    HandsGenerator hand_generator;
    GestureGenerator gest_generator;

    XnBool needPose;
    XnChar strPose[20];

    XnPoint3D hand_pos;

    void CHECK_RC(XnStatus nRetVal, const char* what);
};


#endif // DEPTH_H
