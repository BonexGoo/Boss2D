TARGET = helloapps
TEMPLATE = app

QT_ENABLE_GRAPHICS = ok #ok #no
QT_ADD_PLUGINS_FIREBASE = no #ok #no
QT_PACKAGE_RPATH = no #ok #no

!include(../../Boss2D/project/boss2d.pri) {
    error("Couldn't find the boss2d.pri file...")
}

wasm{
    QMAKE_LFLAGS += -s TOTAL_MEMORY=26214400
}

INCLUDEPATH += ../source-gen
HEADERS += ../source-gen/boss_config.h
HEADERS += ../source-gen/boss_assets.hpp
HEADERS += ../source-gen/boss_buildtag.hpp

INCLUDEPATH += ../source
SOURCES += ../source/helloapps.cpp
HEADERS += ../source/helloapps.hpp
SOURCES += ../source/main.cpp
SOURCES += ../source/resource.cpp
HEADERS += ../source/resource.hpp

ASSETS_FONT.files += ../assets/font
ASSETS_FONT.path = /assets
ASSETS_HTML.files += ../assets/html
ASSETS_HTML.path = /assets
ASSETS_IMAGE.files += ../assets/image
ASSETS_IMAGE.path = /assets
ASSETS_SOUND.files += ../assets/sound
ASSETS_SOUND.path = /assets
ASSETS_WIDGET.files += ../assets/widget
ASSETS_WIDGET.path = /assets

win32{
    RC_ICONS += ../common/windows/main.ico
}

macx{
    QMAKE_BUNDLE_DATA += ASSETS_FONT
    QMAKE_BUNDLE_DATA += ASSETS_HTML
    QMAKE_BUNDLE_DATA += ASSETS_IMAGE
    QMAKE_BUNDLE_DATA += ASSETS_SOUND
    QMAKE_BUNDLE_DATA += ASSETS_WIDGET
    QMAKE_INFO_PLIST = $$PWD/../common/macx/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/../common/macx/Assets.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = AppIcon
}

ios{
    QMAKE_BUNDLE_DATA += ASSETS_FONT
    QMAKE_BUNDLE_DATA += ASSETS_HTML
    QMAKE_BUNDLE_DATA += ASSETS_IMAGE
    QMAKE_BUNDLE_DATA += ASSETS_SOUND
    QMAKE_BUNDLE_DATA += ASSETS_WIDGET
    QMAKE_INFO_PLIST = $$PWD/../common/ios/Info.plist
    QMAKE_ASSET_CATALOGS += $$PWD/../common/ios/Assets.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = AppIcon
}

android{
    INSTALLS += ASSETS_FONT
    INSTALLS += ASSETS_HTML
    INSTALLS += ASSETS_IMAGE
    INSTALLS += ASSETS_SOUND
    INSTALLS += ASSETS_WIDGET
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../common/android
}
