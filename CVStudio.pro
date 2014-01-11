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

QT       += core gui xmlpatterns xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CVStudio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    helpers/launcherpanel.cpp \
    objdetect/classifiertrainerproject.cpp \
    objdetect/gui/classifiertrainergui.cpp \
    objdetect/gui/addsectiondialoggui.cpp \
    objdetect/gui/mouseselecteventfilter.cpp \
    objdetect/gui/newclassifiertrainerpromptgui.cpp

HEADERS  += mainwindow.h \
    helpers/launcherpanel.h \
    includes.h \
    objdetect/classifiertrainerproject.h \
    objdetect/section.h \
    objdetect/gui/mouseselecteventfilter.h \
    objdetect/gui/classifiertrainergui.h \
    objdetect/gui/addsectiondialoggui.h \
    objdetect/gui/newclassifiertrainerpromptgui.h

FORMS += \
    objdetect/gui/newclassifiertrainerprompt.ui \
    helpers/launcherpanel.ui \
    objdetect/gui/addsectiondialog.ui \
    objdetect/gui/classifiertrainergui.ui

RESOURCES += \
    image_assets.qrc


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

