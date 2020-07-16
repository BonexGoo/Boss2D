TARGET = zayeditor
TEMPLATE = app

QT_ENABLE_GRAPHICS = ok #ok #no
QT_ADD_PLUGINS_FIREBASE = no #ok #no
QT_PACKAGE_RPATH = no #ok #no

!include(../../Boss2D/project/boss2d.pri) {
    error("Couldn't find the boss2d.pri file...")
}

INCLUDEPATH += ../source
HEADERS += ../source/boss_config.h
SOURCES += ../source/zaybox.cpp
HEADERS += ../source/zaybox.hpp
SOURCES += ../source/zayeditor.cpp
HEADERS += ../source/zayeditor.hpp
SOURCES += ../source/main.cpp
SOURCES += ../source/resource.cpp
HEADERS += ../source/resource.hpp

ASSETS_IMAGE.files += ../assets/image
ASSETS_IMAGE.path = /assets

win32{
    RC_ICONS += ../common/windows/main.ico
}

macx{
    QMAKE_BUNDLE_DATA += ASSETS_IMAGE
    QMAKE_INFO_PLIST = $$PWD/../common/macx/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/../common/macx/Assets.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = AppIcon
}

ios{
    QMAKE_BUNDLE_DATA += ASSETS_IMAGE
    QMAKE_INFO_PLIST = $$PWD/../common/ios/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/../common/ios/Assets.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = AppIcon
}

android{
    INSTALLS += ASSETS_IMAGE
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../common/android
}
