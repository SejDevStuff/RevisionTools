QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    flashcards.cpp \
    main.cpp \
    mainwindow.cpp \
    pairedsetcreator.cpp \
    pairedsetwriter.cpp \
    textmemorise.cpp \
    textmemorise_createset.cpp

HEADERS += \
    flashcards.h \
    mainwindow.h \
    pairedsetcreator.h \
    pairedsetwriter.h \
    textmemorise.h \
    textmemorise_createset.h

FORMS += \
    flashcards.ui \
    mainwindow.ui \
    pairedsetcreator.ui \
    textmemorise.ui \
    textmemorise_createset.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
