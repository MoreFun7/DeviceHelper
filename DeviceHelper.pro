QT       += core gui serialport network

TRANSLATIONS =  :/lan/language/Translate_CN.ts \
                :/lan/language/Translate_EN.ts \
                :/lan/language/Translate_RU.ts

# TRANSLATIONS =  E:/Object/mylanguage/Translate_CN.ts \
#                 E:/Object/mylanguage/Translate_EN.ts \
#                 E:/Object/mylanguage/Translate_RU.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quickwidgets

CONFIG += c++17

VERSION = 2.1

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    devicehelper.cpp

HEADERS += \
    devicehelper.h

FORMS += \
    devicehelper.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc \
    language.qrc \
    qml.qrc

RC_ICONS = res/ProgramIcon.ico

# DISTFILES += \
#     qml/titlebar.qml \
