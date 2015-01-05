TEMPLATE = app
CONFIG += console
CONFIG -= qt

default_deployment.pkg_prerules -= pkg_depends_webkit
default_deployment.pkg_prerules -= pkg_depends_

SOURCES += ServerSession.cpp \
    MiniLog.cpp \
    MiniCMD.cpp \
    Config.cpp

HEADERS += \
    TCommand.h \
    ServerSession.h \
    RequestCmd.h \
    Parameter.h \
    MiniLog.h \
    MiniCMD.h \
    Constant.h \
    Config.h \
    Common.h

LIBS += \
    -lefsrv \
    -lestor \
    -lws32 \
    -lapgrfx \
    -lapparc \
    -laknnotify \
    -lbafl \
    -lSWInstCli

TARGET.CAPABILITY += All -TCB

INCLUDEPATH += C:/QtSDK/Symbian/SDKs/SymbianSR1Qt474/epoc32/include/
INCLUDEPATH += C:/QtSDK/Symbian/SDKs/SymbianSR1Qt474/epoc32/include/platform/
INCLUDEPATH += C:/QtSDK/Symbian/SDKs/SymbianSR1Qt474/epoc32/include/platform/mw/
INCLUDEPATH += C:/QtSDK/Symbian/SDKs/SymbianSR1Qt474/epoc32/include/mw/
INCLUDEPATH += C:/QtSDK/Symbian/SDKs/SymbianSR1Qt474/epoc32/include/bafl/

QMAKE_CXXFLAGS += -std=c++0x
CONFIG += c++11 #no_icon 

MMP_RULES += "OPTION gcce -O3"
MMP_RULES += "OPTION gcce -march=armv6"
MMP_RULES += "OPTION gcce -mfpu=vfp"
MMP_RULES += "OPTION gcce -mfloat-abi=softfp"
MMP_RULES += "OPTION gcce -marm"
MMP_RULES += "OPTION gcce -fpermissive"
#MMP_RULES += "OPTION gcce -w"
MMP_RULES += "OPTION gcce -ffast-math"
MMP_RULES += "OPTION gcce -Wno-unused-parameter"
MMP_RULES += "OPTION gcce -std=gnu++0x"
MMP_RULES += "DEBUGGABLE"


