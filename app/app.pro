QT += widgets

CONFIG += c++17
QMAKE_PROJECT_DEPTH = 0
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QT += core gui multimedia multimediawidgets
QT += core gui multimedia
SOURCES += \
    alcoholengine.cpp \
    barwidget.cpp \
    drivingwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    policewidget.cpp \
    videocutscenewidget.cpp

HEADERS += \
    alcoholengine.h \
    barwidget.h \
    drivingwidget.h \
    itimeprovider.h \
    mainwindow.h \
    mocktimeprovider.h \
    policewidget.h \
    realtimeprovider.h \
    videocutscenewidget.h

FORMS += \
    barwidget.ui \
    drivingwidget.ui \
    mainwindow.ui \
    policewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
