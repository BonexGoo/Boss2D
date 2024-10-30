TARGET = helloworld
TEMPLATE = app

QT_ENABLE_GRAPHICS = ok #ok #no
QT_ADD_PLUGINS_FIREBASE = no #ok #no
QT_PACKAGE_RPATH = no #ok #no

!include(../../Boss2D/project/boss2d.pri) {
    error("Couldn't find the boss2d.pri file...")
}

wasm{
    QT -= bluetooth
    QT -= websockets
    QMAKE_LFLAGS += -s TOTAL_MEMORY=65536000
    QMAKE_LFLAGS += -s ASYNCIFY=1 -lidbfs.js
}

INCLUDEPATH += ../source-gen
HEADERS += ../source-gen/boss_config.h
HEADERS += ../source-gen/boss_assets.hpp
HEADERS += ../source-gen/boss_buildtag.hpp

INCLUDEPATH += ../source
SOURCES += ../source/helloworld.cpp
HEADERS += ../source/helloworld.hpp
SOURCES += ../source/helloworld_widget.cpp
HEADERS += ../source/helloworld_widget.hpp
SOURCES += ../source/main.cpp
SOURCES += ../source/resource.cpp
HEADERS += ../source/resource.hpp

ASSETS_IMAGE.files += ../assets/image
ASSETS_IMAGE.path = /assets
ASSETS_WIDGET.files += ../assets/widget
ASSETS_WIDGET.path = /assets

win32{
    RC_ICONS += ../common/windows/main.ico
}

macx{
    QMAKE_BUNDLE_DATA += ASSETS_IMAGE
    QMAKE_BUNDLE_DATA += ASSETS_WIDGET
    QMAKE_INFO_PLIST = $$PWD/../common/macx/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/../common/macx/Assets.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = AppIcon
}

ios{
    QMAKE_BUNDLE_DATA += ASSETS_IMAGE
    QMAKE_BUNDLE_DATA += ASSETS_WIDGET
    QMAKE_INFO_PLIST = $$PWD/../common/ios/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/../common/ios/Assets.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = AppIcon
}

android{
    INSTALLS += ASSETS_IMAGE
    INSTALLS += ASSETS_WIDGET
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../common/android
}
