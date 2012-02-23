#include "KinectControl.h"

KinectControl::KinectControl()
    : skeleton(0),
      pose(0)
{
    Init();
}

static void XN_CALLBACK_TYPE _NewUser(UserGenerator &generator, XnUserID nId, void* pCookie)
{
    ((KinectControl*)pCookie)->NewUser(generator, nId);
}

static void XN_CALLBACK_TYPE _LostUser(UserGenerator &generator, XnUserID nId, void* pCookie)
{
    ((KinectControl*)pCookie)->LostUser(generator, nId);
}

static void calibrationStart(xn::SkeletonCapability &skeleton, XnUserID nId, void* pCookie)
{

}

static void calibrationEnd(xn::SkeletonCapability&, XnUserID, XnBool, void*);


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

    // コールバック登録
    nRetVal = user_generator.RegisterUserCallbacks(_NewUser, _LostUser, this, user_callbacks);
    CHECK_RC(nRetVal, "Register User Callbacks");

    // スケルトンを初期化
    if(!user_generator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
        cerr << "Supplied user generator doesn't suppoert skeleton tracking" << endl;
    else
    {
        skeleton = user_generator.GetSkeletonCap();
        skeleton.SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    }

    if(!user_generator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
        cerr << "Supplied user generator doesn't suppoert user detection" << endl;
    else
    {
        pose = user_generator.GetPoseDetectionCap();
    }
    nRetVal = context.StartGeneratingAll();
    CHECK_RC(nRetVal, "Start Generating All");
}

void KinectControl::NewUser(UserGenerator &generator, XnUserID nId)
{
    cout << "New User: " << nId << endl;

    pose.StartPoseDetection(skeleton.GetCalibrationPose());
    XnStatus nRetVal = skeleton.StartTracking(nId);
    CHECK_RC(nRetVal, "Start Tracking");
}

void KinectControl::LostUser(UserGenerator &generator, XnUserID nId)
{
    cout << "Lost User: " << nId << endl;

    XnStatus nRetVal = skeleton.StopTracking(nId);
    CHECK_RC(nRetVal, "End Tracking");
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

void KinectControl::CHECK_RC(XnStatus nRetVal, const char *what)
{
    if(nRetVal != XN_STATUS_OK)
    {
        cerr << boost::format("%s failed: %s") % what % xnGetStatusString(nRetVal) << endl;
        throw runtime_error(string(what) + ":" + xnGetStatusString(nRetVal));
    }
}
