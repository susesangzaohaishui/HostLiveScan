QT += core
QT -= gui

TARGET = HostLive
CONFIG += console
CONFIG -= app_bundle

#DEFINES += __DEBUG__

LIBS += -lpthread

TEMPLATE = lib

SOURCES += main.cpp \
    host_scan.cpp \
    clock.cpp \
    ctx.cpp \
    data_queue.cpp \
    epoll.cpp \
    err.cpp \
    global_val.cpp \
    io_object.cpp \
    io_thread.cpp \
    ip.cpp \
    mailbox.cpp \
    net_utils.cpp \
    object.cpp \
    options.cpp \
    poller_base.cpp \
    random.cpp \
    reaper.cpp \
    select.cpp \
    session.cpp \
    signaler.cpp \
    thread.cpp \
    tinystr.cpp \
    tinyxml.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp \
    sock_op.cpp


HEADERS += \
    host_scan.h \
    array.hpp \
    atomic_counter.hpp \
    atomic_ptr.hpp \
    clock.hpp \
    command.hpp \
    config.hpp \
    ctx.hpp \
    data_queue.hpp \
    epoll.hpp \
    err.hpp \
    fd.hpp \
    global_val.h \
    i_linkinfo_t.h \
    io_object.hpp \
    io_thread.hpp \
    ip.hpp \
    i_poll_events.hpp \
    i_queue_service.hpp \
    likely.hpp \
    mailbox.hpp \
    msg_define.hpp \
    mutex.hpp \
    net_common.h \
    net_utils.h \
    object.hpp \
    options.hpp \
    platform.hpp \
    poller.hpp \
    poller_base.hpp \
    random.hpp \
    reaper.hpp \
    select.hpp \
    session.hpp \
    signaler.hpp \
    stdint.hpp \
    tcp.hpp \
    thread.hpp \
    tinystr.h \
    tinyxml.h \
    windows.hpp \
    ypipe.hpp \
    ypipe_base.hpp \
    yqueue.hpp \
    sock_op.h

