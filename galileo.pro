TEMPLATE = app
TARGET = galileo
DEPENDPATH += .
INCLUDEPATH += .
QT += widgets network
QMAKE_INFO_PLIST = galileo.plist
unix:LIBS += -lz
win32:LIBS += $$PWD/zlibstat.lib
win32:RC_FILE += winicon.rc
macx:ICON = icon.icns

# Input
HEADERS += zlib.h zconf.h galileo.h mapview.h steam.h assets.h players.h planets.h
SOURCES += main.cpp galileo.cpp mapview.cpp steam.cpp assets.cpp players.cpp planets.cpp

desktopfile.path = /usr/share/applications
desktopfile.files = galileo.desktop
pixmapfile.path = /usr/share/pixmaps
pixmapfile.files = galileo.png galileo.xpm
target.path = /usr/bin
INSTALLS += desktopfile pixmapfile target
