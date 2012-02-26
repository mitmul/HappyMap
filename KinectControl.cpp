#include "KinectControl.h"

KinectControl::KinectControl()
    : needPose(false)
{
    Init();
}

KinectControl::~KinectControl()
{
    context.Shutdown();
}

static void XN_CALLBACK_TYPE _NewUser(UserGenerator &generator, XnUserID nId, void* pCookie)
{
    ((KinectControl*)pCookie)->NewUser(generator, nId);
}

void KinectControl::NewUser(UserGenerator &generator, XnUserID nId)
{
    cout << "New User: " << nId << endl;

    if(needPose)
    {
        user_generator.GetPoseDetectionCap().StartPoseDetection(strPose, nId);
    }
    else
    {
        user_generator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}

static void XN_CALLBACK_TYPE _LostUser(UserGenerator &generator, XnUserID nId, void* pCookie)
{
    ((KinectControl*)pCookie)->LostUser(generator, nId);
}

void KinectControl::LostUser(UserGenerator &generator, XnUserID nId)
{
    cout << "Lost User: " << nId << endl;
}

static void XN_CALLBACK_TYPE _CalibrationStart(SkeletonCapability &skeleton, XnUserID user, void* pCookie)
{
    ((KinectControl*)pCookie)->CalibrationStart(skeleton, user);
}

void KinectControl::CalibrationStart(SkeletonCapability &skeleton, XnUserID user)
{
    cout << "Calibration started for user " << user << endl;
}

static void XN_CALLBACK_TYPE _CalibrationEnd(SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
    ((KinectControl*)pCookie)->CalibrationEnd(capability, nId, eStatus);
}

void KinectControl::CalibrationEnd(SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus)
{
    if(eStatus == XN_CALIBRATION_STATUS_OK)
    {
        cout << "Calibration complete, start tracking user " << nId << endl;
        user_generator.GetSkeletonCap().StartTracking(nId);
    }
    else
    {
        cerr << "Calibration failed for user " << nId << endl;
        if(eStatus == XN_CALIBRATION_STATUS_MANUAL_ABORT)
        {
            cerr << "Manual abort occured, stop attempting to calibrate!" << endl;
        }
        if(needPose)
        {
            user_generator.GetPoseDetectionCap().StartPoseDetection(strPose, nId);
        }
        else
        {
            user_generator.GetSkeletonCap().RequestCalibration(nId, TRUE);
        }
    }
}

static void XN_CALLBACK_TYPE _PoseDetected(PoseDetectionCapability& pose, const XnChar* strPose, XnUserID user, void* pCookie)
{
    ((KinectControl*)pCookie)->PoseDetected(pose, strPose, user);
}

void KinectControl::PoseDetected(PoseDetectionCapability &pose, const XnChar *strPose, XnUserID user)
{
    cout << "Pose " << strPose << " detected for user " << user << endl;
    user_generator.GetPoseDetectionCap().StopPoseDetection(user);
    user_generator.GetSkeletonCap().RequestCalibration(user, TRUE);
}

static void XN_CALLBACK_TYPE _HandCreate(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
    ((KinectControl*)pCookie)->HandCreate(generator, nId, pPosition, fTime);
}

void KinectControl::HandCreate(HandsGenerator &generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat fTime)
{
    cout << "New Hand: " << nId << " @ (" << pPosition->X << "," << pPosition->Y << "," << pPosition->Z << ")" << endl;

    hand_pos = *pPosition;
}

static void XN_CALLBACK_TYPE _HandUpdate(HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie)
{
    ((KinectControl*)pCookie)->HandUpdate(generator, nId, pPosition, fTime);
}

void KinectControl::HandUpdate(HandsGenerator &generator, XnUserID nId, const XnPoint3D *pPosition, XnFloat fTime)
{
//    cout << "Hand Update: " << nId << " @ (" << pPosition->X << "," << pPosition->Y << "," << pPosition->Z << ")" << endl;

    hand_pos = *pPosition;
}

static void XN_CALLBACK_TYPE _HandDestroy(HandsGenerator& generator, XnUserID nId, XnFloat fTime, void* pCookie)
{
    ((KinectControl*)pCookie)->HandDestroy(generator, nId, fTime);
}

void KinectControl::HandDestroy(HandsGenerator &generator, XnUserID nId, XnFloat fTime)
{
    cout << "Lost Hand: " << nId << endl;
}

static void XN_CALLBACK_TYPE _GestureRecognized(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D*	pIDPosition, const XnPoint3D*	pEndPosition, void* pCookie)
{
    ((KinectControl*)pCookie)->GestureRecognized(generator, strGesture, pIDPosition, pEndPosition);
}

void KinectControl::GestureRecognized(GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pIDPosition, const XnPoint3D *pEndPosition)
{
    cout << "Gesture recognized: " << strGesture << endl;
    hand_generator.StartTracking(*pEndPosition);
}

static void XN_CALLBACK_TYPE _GestureProcess(GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat	fProgress, void* pCookie)
{
    ((KinectControl*)pCookie)->GestureProcess(generator, strGesture, pPosition, fProgress);
}

void KinectControl::GestureProcess(GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pPosition, XnFloat fProgress)
{

}

void KinectControl::Init()
{
    XnStatus nRetVal;

    nRetVal = context.Init();
    CHECK_RC(nRetVal, "Context Init");

    map_mode.nFPS = 30;
    map_mode.nXRes = XN_VGA_X_RES;
    map_mode.nYRes = XN_VGA_Y_RES;

    // デプスを初期化
    nRetVal = depth_generator.Create(context);
    CHECK_RC(nRetVal, "Find depth generator");
    depth_generator.SetMapOutputMode(map_mode);

    // ユーザを初期化
    nRetVal = user_generator.Create(context);
    CHECK_RC(nRetVal, "Find user generator");

    // スケルトンを初期化
    if(!user_generator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
        cerr << "Supplied user generator doesn't support skeleton" << endl;

    // ユーザコールバック登録
    nRetVal = user_generator.RegisterUserCallbacks(_NewUser, _LostUser, this, user_callbacks);
    CHECK_RC(nRetVal, "Register to user callbacks");

    // キャリブレーションコールバック登録
    nRetVal = user_generator.GetSkeletonCap().RegisterToCalibrationStart(_CalibrationStart, this, start_callback);
    CHECK_RC(nRetVal, "Register to calibration start");

    nRetVal = user_generator.GetSkeletonCap().RegisterToCalibrationComplete(_CalibrationEnd, this, end_callback);
    CHECK_RC(nRetVal, "Register to calibration complete");

    // ポーズが必要な場合
    if(user_generator.GetSkeletonCap().NeedPoseForCalibration())
    {
        needPose = TRUE;
        if(!user_generator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        {
            cerr << "Pose required, but not supperted" << endl;
        }

        nRetVal = user_generator.GetPoseDetectionCap().RegisterToPoseDetected(_PoseDetected,this, pose_callback);
        CHECK_RC(nRetVal, "Register to Pose Detected");

        user_generator.GetSkeletonCap().GetCalibrationPose(strPose);
    }

    nRetVal = user_generator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    CHECK_RC(nRetVal, "Set skeleton profile");

    // ジェスチャ
    nRetVal = gest_generator.Create(context);
    CHECK_RC(nRetVal, "Unable to create GestureGenerator");

    nRetVal = gest_generator.RegisterGestureCallbacks(_GestureRecognized, _GestureProcess, this, hand_callback);
    CHECK_RC(nRetVal, "Unable to register gesture callbacks");

    gest_generator.AddGesture("Click", NULL);
    gest_generator.AddGesture("Wave", NULL);
    gest_generator.AddGesture("RaiseHand", NULL);

    // ハンドトラッキング
    nRetVal = hand_generator.Create(context);
    CHECK_RC(nRetVal, "Unable to create HandsGenerator");

    nRetVal = hand_generator.RegisterHandCallbacks(_HandCreate, _HandUpdate, _HandDestroy, this, hand_callback);
    CHECK_RC(nRetVal, "Unable to register hand callbacks");

    nRetVal = context.StartGeneratingAll();
    CHECK_RC(nRetVal, "Start Generating All");
}

Mat KinectControl::getDepthImage()
{
    XnDepthPixel *depth = getDepth();
    Mat img(map_mode.nYRes, map_mode.nXRes, CV_16UC1, depth);
    img.convertTo(img, CV_8U, 255.0 / 8192.0);

    return img;
}

XnPoint3D KinectControl::getHandPos()
{
    XnStatus nRetVal = context.WaitAndUpdateAll();
    CHECK_RC(nRetVal, "Update Data");

    XnPoint3D proj = hand_pos;
    depth_generator.ConvertRealWorldToProjective(1, &proj, &proj);

    return proj;
}

XnDepthPixel* KinectControl::getDepth()
{
    XnStatus nRetVal = context.WaitAndUpdateAll();
    CHECK_RC(nRetVal, "Update Data");

    DepthMetaData depthMD;
    depth_generator.GetMetaData(depthMD);

    return depthMD.WritableData();
}

std::vector<std::vector<XnPoint3D> > KinectControl::getSkeleton()
{
    SkeletonCapability skeleton = user_generator.GetSkeletonCap();

    XnUserID user_id[15];
    XnUInt16 user_count = sizeof(user_id) / sizeof(user_id[0]);

    XnStatus nRetVal = user_generator.GetUsers(user_id, user_count);
    CHECK_RC(nRetVal, "Get Users");

    // 全ユーザ
    std::vector<std::vector<XnPoint3D> > skeletons;

    for(int i = 0; i < user_count; ++i)
    {
        // i番目のユーザがトラッキングされていたら
        if(skeleton.IsTracking(user_id[i]))
        {
            // 個別ユーザ
            std::vector<XnPoint3D> user_skeleton;

            // 関節は全てで24個
            XnSkeletonJointPosition joint[24];

            // [0]はCenter of Mass
            XnPoint3D pt[25];

            for(int j = 1; j < 25; ++j)
            {
                skeleton.GetSkeletonJointPosition(user_id[i], (XnSkeletonJoint)(j), joint[j - 1]);
                pt[j] = joint[j - 1].position;
            }

            user_generator.GetCoM(user_id[i], pt[0]);
            depth_generator.ConvertRealWorldToProjective(25, pt, pt);

            for(int j = 0; j < 25; ++j)
                user_skeleton.push_back(pt[j]);

            skeletons.push_back(user_skeleton);
        }
    }

    return skeletons;
}

XnPoint3D KinectControl::getRealValue(const XnPoint3D src)
{
    XnPoint3D real = src;
    depth_generator.ConvertProjectiveToRealWorld(1, &real, &real);

    return real;
}

void KinectControl::CHECK_RC(XnStatus nRetVal, const char *what)
{
    if(nRetVal != XN_STATUS_OK)
    {
        cerr << boost::format("%s failed: %s") % what % xnGetStatusString(nRetVal) << endl;
        throw runtime_error(string(what) + ":" + xnGetStatusString(nRetVal));
    }
}

