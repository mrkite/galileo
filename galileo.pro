TEMPLATE = app
TARGET = galileo
DEPENDPATH += .
INCLUDEPATH += .
QT += widgets network concurrent
QMAKE_INFO_PLIST = galileo.plist
unix:LIBS += -lz
win32:LIBS += $$PWD/zlibstat.lib
win32:RC_FILE += winicon.rc
macx:ICON = icon.icns

# Input
HEADERS += zlib.h zconf.h galileo.h mapview.h steam.h assets.h players.h planets.h json.h sbv.h bitreader.h worldcoordinate.h coordinate.h world.h worldmeta.h btdb.h sbsf.h worldstructure.h variant.h sectorcache.h tilesector.h sectorloader.h tile.h renderer.h
SOURCES += main.cpp galileo.cpp mapview.cpp steam.cpp assets.cpp players.cpp planets.cpp json.cpp sbv.cpp bitreader.cpp worldcoordinate.cpp coordinate.cpp world.cpp worldmeta.cpp btdb.cpp sbsf.cpp worldstructure.cpp variant.cpp sectorcache.cpp tilesector.cpp sectorloader.cpp tile.cpp renderer.cpp

unix {
desktop.path = /usr/share/applications
desktop.files = galileo.desktop
icon48.path = /usr/share/icons/hicolor/16x16/apps
icon48.files = icons/48x48/galileo.png
icon256.path = /usr/share/icons/hicolor/256x256/apps
icon256.files = icons/256x256/galileo.png
target.path = /usr/bin
INSTALLS += desktop icon48 icon256 target
}
