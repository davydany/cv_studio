#-------------------------------------------------
#
# Project created by QtCreator 2013-12-28T17:39:26
#
#-------------------------------------------------
#
# Required Libraries:
# 1. OpenCV
# 2. qjson
#

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CVStudio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    objdetect/classifiertrainer.cpp \
    objdetect/newclassifiertrainerprompt.cpp \
    helpers/launcherpanel.cpp \
    objdetect/positiveimagemodel.cpp

HEADERS  += mainwindow.h \
    objdetect/classifiertrainer.h \
    objdetect/newclassifiertrainerprompt.h \
    helpers/launcherpanel.h \
    includes.h \
    objdetect/positiveimagemodel.h


# OpenCV
macx: INCLUDEPATH += \
                /usr/local/Cellar/opencv/2.4.6.1/include/ \
                /usr/local/Cellar/opencv/2.4.6.1/include/opencv \
                /usr/local/Cellar/opencv/2.4.6.1/include/opencv2

macx: DEPENDPATH += \
                /usr/local/Cellar/opencv/2.4.6.1/include/ \
                /usr/local/Cellar/opencv/2.4.6.1/include/opencv \
                /usr/local/Cellar/opencv/2.4.6.1/include/opencv2

macx: LIBS += -L"/usr/local/Cellar/opencv/2.4.6.1/lib/" \
                -lopencv_calib3d \
                -lopencv_contrib \
                -lopencv_core \
                -lopencv_features2d \
                -lopencv_flann \
                -lopencv_gpu \
                -lopencv_highgui \
                -lopencv_imgproc \
                -lopencv_legacy \
                -lopencv_ml \
                -lopencv_nonfree \
                -lopencv_objdetect \
                -lopencv_ocl \
                -lopencv_photo \
                -lopencv_stitching \
                -lopencv_superres \
                -lopencv_ts \
                -lopencv_video \
                -lopencv_videostab


RESOURCES += \
    image_assets.qrc

FORMS += \
    objdetect/newclassifiertrainerprompt.ui \
    helpers/launcherpanel.ui
