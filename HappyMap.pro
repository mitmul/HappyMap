#-------------------------------------------------
#
# Project created by QtCreator 2012-02-24T06:46:14
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = HappyMap
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    OSCReceiver.cpp \
    OSCSender.cpp \
    KinectControl.cpp \
    GLWidget.cpp \
    CapCambus.cpp \
    SoundDefine.cpp

HEADERS  += MainWindow.h \
    OSCReceiver.h \
    OSCSender.h \
    KinectControl.h \
    GLWidget.h \
    CapCambus.h \
    SoundDefine.h

FORMS    += MainWindow.ui

mac {

# OpenCV
INCLUDEPATH += /usr/local/include/opencv2
LIBS += -L/usr/local/lib \
    -lopencv_contrib \
    -lopencv_core \
    -lopencv_flann \
    -lopencv_gpu \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lopencv_legacy \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_stitching \
    -lopencv_ts \
    -lopencv_video

# OpenNI & NITE
INCLUDEPATH += /usr/include/ni \
    /usr/include/nite
LIBS += -L/usr/lib \
    -lnimCodecs \
    -lnimMockNodes \
    -lnimRecorder \
    -lOpenNI \
    -lOpenNI.jni \
    -lXnVNITE.jni \
    -lXnVNite_1_5_2 \
    -lXnVCNITE_1_5_2 \
    -lXnVFeatures_1_5_2 \
    -lXnVHandGenerator_1_5_2

# boost
INCLUDEPATH += /opt/local/include
LIBS += -L/opt/local/lib \
    -lboost_prg_exec_monitor-mt \
    -lboost_program_options-mt \
    -lboost_random-mt \
    -lboost_serialization-mt \
    -lboost_signals-mt \
    -lboost_unit_test_framework-mt \
    -lboost_wave-mt \
    -lboost_wserialization-mt \
    -lboost_math_c99l-mt \
    -lboost_math_c99f-mt \
    -lboost_math_c99-mt \
    -lboost_math_tr1l-mt \
    -lboost_math_tr1f-mt \
    -lboost_math_tr1-mt \
    -lboost_chrono-mt \
    -lboost_date_time-mt \
    -lboost_filesystem-mt \
    -lboost_graph-mt \
    -lboost_iostreams-mt \
    -lboost_regex-mt \
    -lboost_system-mt \
    -lboost_math_c99l \
    -lboost_prg_exec_monitor \
    -lboost_program_options \
    -lboost_random \
    -lboost_serialization \
    -lboost_signals \
    -lboost_thread-mt \
    -lboost_unit_test_framework \
    -lboost_wave \
    -lboost_wserialization \
    -lboost_math_c99f \
    -lboost_math_c99 \
    -lboost_math_tr1l \
    -lboost_math_tr1f \
    -lboost_math_tr1 \
    -lboost_graph \
    -lboost_iostreams \
    -lboost_regex \
    -lboost_chrono \
    -lboost_date_time \
    -lboost_filesystem \
    -lboost_system

# liblo
INCLUDEPATH += /usr/local/include/lo
LIBS += -L/usr/local/lib \
    -llo

# Eigen
INCLUDEPATH += /opt/local/include/eigen3

}
