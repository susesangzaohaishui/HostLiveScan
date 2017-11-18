#-------------------------------------------------
#
# Project created by QtCreator 2016-01-17T12:25:02
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = TestHostScan
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
SOURCES += main.cpp
DEFINES +=__DEBUG__
#设置动态库路径标准的做法是在/etc/ld.conf.d 下创建一个*.conf文件
#并将动态库路径放在该文件中，并使用root 执行ldconfig
unix:!macx: LIBS = -L$$PWD/../lib/scan_host -lHostLive
INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
