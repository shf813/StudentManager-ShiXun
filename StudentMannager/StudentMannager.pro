QT       += core gui sql widgets charts concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 禁用过时API
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += main.cpp \
    loginwidget.cpp \
    registerdialog.cpp \
    mainwindow.cpp \
    exportthread.cpp \
    stylehelper.cpp

HEADERS += commondef.h \
    loginwidget.h \
    registerdialog.h \
    mainwindow.h \
    exportthread.h \
    stylehelper.h

FORMS += loginwidget.ui \
    registerdialog.ui \
    mainwindow.ui

TRANSLATIONS += \
    StudentManager_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    StudentManager_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    StudentManager_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    StudentManager_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
