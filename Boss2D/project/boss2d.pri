###########################################################
# QT Components
###########################################################
QT += core
equals(QT_ENABLE_GRAPHICS, "ok"){
    QT += gui
    QT += opengl
    QT += widgets
    DEFINES += QT_HAVE_GRAPHICS
} else {
    QT -= gui
    QT -= opengl
    QT -= widgets
    CONFIG += console
    CONFIG -= app_bundle
}
QT += network
qtHaveModule(serialport){
    QT += serialport
    DEFINES += QT_HAVE_SERIALPORT
}
ios: QT += gui-private
android: QT += androidextras
equals(QT_ENABLE_GRAPHICS, "ok"){
    qtHaveModule(webenginewidgets){
        QT += webengine
        QT += webenginewidgets
        DEFINES += QT_HAVE_WEBENGINEWIDGETS
    }
}
qtHaveModule(multimedia){
    QT += multimedia
    DEFINES += QT_HAVE_MULTIMEDIA
}
qtHaveModule(purchasing){
    QT += purchasing
    DEFINES += QT_HAVE_PURCHASING
}

###########################################################
# QT Build Flags
###########################################################
equals(QT_ENABLE_GRAPHICS, "ok"){
    message("QT_ENABLE_GRAPHICS : Included.")
} else {
    equals(QT_ENABLE_GRAPHICS, "no"){
        message("QT_ENABLE_GRAPHICS : Excluded.")
    } else {
        error("QT_ENABLE_GRAPHICS flag is required.")
    }
}
equals(QT_ADD_PLUGINS_FIREBASE, "ok"){
    include(../../Boss2D_plugins/firebase_cpp_sdk_4.5.1/firebase_cpp_sdk.pri){
        message("QT_ADD_PLUGINS_FIREBASE : The plug-in connected.")
    } else {
        message("QT_ADD_PLUGINS_FIREBASE : The plug-in is not ready and the module connection is canceled.")
    }
} else {
    equals(QT_ADD_PLUGINS_FIREBASE, "no"){
        message("QT_ADD_PLUGINS_FIREBASE : Excluded.")
    } else {
        error("QT_ADD_PLUGINS_FIREBASE flag is required.")
    }
}

###########################################################
# Native Build Options
###########################################################
CONFIG += c++11
CONFIG += mobility
CONFIG += warn_off
MOBILITY += systeminfo
win32-msvc*: QMAKE_CFLAGS += /bigobj
win32-msvc*: QMAKE_CXXFLAGS += /bigobj
win32-g++: LIBS += -lwsock32
win32-g++: LIBS += -lws2_32
win32-g++: LIBS += -lwinmm
win32-g++: LIBS += -lcomdlg32
win32-g++: LIBS += -luser32
win32-g++: LIBS += -lole32
win32-g++: LIBS += -lshell32
android: LIBS += -landroid
macx|ios: LIBS += -framework CoreFoundation
linux-g++: CONFIG += link_pkgconfig
linux-g++: LIBS += -ldl
equals(QT_ENABLE_GRAPHICS, "ok"){
    linux-g++: PKGCONFIG += gtk+-3.0
    # sudo apt-get install build-essential libgtk-3-dev
    # sudo apt-get install build-essential libgl1-mesa-dev
}
equals(QT_PACKAGE_RPATH, "ok"){
    linux-g++: QMAKE_LFLAGS_RPATH=
    linux-g++: QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib\'"
}
TOPPATH = $$PWD/../../Boss2D
INCLUDEPATH += $$TOPPATH/core
!android: INCLUDEPATH += $$TOPPATH/addon/trick_for_fakewin
INCLUDEPATH += $$TOPPATH
CONFIG(debug, debug|release){
    DEFINES += BOSS_NDEBUG=0
}
CONFIG(release, debug|release){
    DEFINES += BOSS_NDEBUG=1
}

###########################################################
# ADDON
###########################################################
HEADERS += $$TOPPATH/addon/boss_addon_aac.hpp
HEADERS += $$TOPPATH/addon/boss_addon_alpr.hpp
HEADERS += $$TOPPATH/addon/boss_addon_curl.hpp
HEADERS += $$TOPPATH/addon/boss_addon_dlib.hpp
HEADERS += $$TOPPATH/addon/boss_addon_freetype.hpp
HEADERS += $$TOPPATH/addon/boss_addon_git.hpp
HEADERS += $$TOPPATH/addon/boss_addon_h264.hpp
HEADERS += $$TOPPATH/addon/boss_addon_jpg.hpp
HEADERS += $$TOPPATH/addon/boss_addon_ogg.hpp
HEADERS += $$TOPPATH/addon/boss_addon_opencv.hpp
HEADERS += $$TOPPATH/addon/boss_addon_psd.hpp
HEADERS += $$TOPPATH/addon/boss_addon_ssl.hpp
HEADERS += $$TOPPATH/addon/boss_addon_tesseract.hpp
HEADERS += $$TOPPATH/addon/boss_addon_tif.hpp
HEADERS += $$TOPPATH/addon/boss_addon_webrtc.hpp
HEADERS += $$TOPPATH/addon/boss_addon_websocket.hpp
HEADERS += $$TOPPATH/addon/boss_addon_zip.hpp
HEADERS += $$TOPPATH/addon/boss_addon_zipa.hpp
HEADERS += $$TOPPATH/addon/boss_fakewin.h
HEADERS += $$TOPPATH/addon/boss_integration_abseil-cpp-20180600.h
HEADERS += $$TOPPATH/addon/boss_integration_curl-7.51.0.h
HEADERS += $$TOPPATH/addon/boss_integration_dlib-19.17.h
HEADERS += $$TOPPATH/addon/boss_integration_fdk-aac-0.1.4.h
HEADERS += $$TOPPATH/addon/boss_integration_jsoncpp-1.8.4.h
HEADERS += $$TOPPATH/addon/boss_integration_libgit2-0.22.2.h
HEADERS += $$TOPPATH/addon/boss_integration_libogg-1.3.3.h
HEADERS += $$TOPPATH/addon/boss_integration_libsrtp-2.2.0.h
HEADERS += $$TOPPATH/addon/boss_integration_libssh2-1.6.0.h
HEADERS += $$TOPPATH/addon/boss_integration_libvorbis-1.3.6.h
HEADERS += $$TOPPATH/addon/boss_integration_openalpr-2.3.0.h
HEADERS += $$TOPPATH/addon/boss_integration_openalpr-2.3.0_3rdparty_liblept.h
HEADERS += $$TOPPATH/addon/boss_integration_openalpr-2.3.0_3rdparty_libtiff.h
HEADERS += $$TOPPATH/addon/boss_integration_opencv-3.1.0.h
HEADERS += $$TOPPATH/addon/boss_integration_opencv-3.1.0_3rdparty_libjpeg.h
HEADERS += $$TOPPATH/addon/boss_integration_openh264-1.6.0.h
HEADERS += $$TOPPATH/addon/boss_integration_openssl-1.1.1a.h
HEADERS += $$TOPPATH/addon/boss_integration_psd_sdk-commit23.h
HEADERS += $$TOPPATH/addon/boss_integration_rnnoise-chromium.h
HEADERS += $$TOPPATH/addon/boss_integration_rtmpdump-2.4.h
HEADERS += $$TOPPATH/addon/boss_integration_tesseract-3.05.02.h
HEADERS += $$TOPPATH/addon/boss_integration_usrsctp-0.9.3.0.h
HEADERS += $$TOPPATH/addon/boss_integration_webrtc-jumpingyang001.h
HEADERS += $$TOPPATH/addon/boss_integration_websocketpp-0.8.1.hpp
HEADERS += $$TOPPATH/addon/boss_integration_ziparchive-4.6.7.h
SOURCES += $$TOPPATH/addon/boss_addon_aac.cpp
SOURCES += $$TOPPATH/addon/boss_addon_alpr.cpp
SOURCES += $$TOPPATH/addon/boss_addon_curl.cpp
SOURCES += $$TOPPATH/addon/boss_addon_dlib.cpp
SOURCES += $$TOPPATH/addon/boss_addon_freetype.cpp
SOURCES += $$TOPPATH/addon/boss_addon_git.cpp
SOURCES += $$TOPPATH/addon/boss_addon_h264.cpp
SOURCES += $$TOPPATH/addon/boss_addon_jpg.cpp
SOURCES += $$TOPPATH/addon/boss_addon_ogg.cpp
SOURCES += $$TOPPATH/addon/boss_addon_opencv.cpp
SOURCES += $$TOPPATH/addon/boss_addon_psd.cpp
SOURCES += $$TOPPATH/addon/boss_addon_ssl.cpp
SOURCES += $$TOPPATH/addon/boss_addon_tesseract.cpp
SOURCES += $$TOPPATH/addon/boss_addon_tif.cpp
SOURCES += $$TOPPATH/addon/boss_addon_webrtc.cpp
SOURCES += $$TOPPATH/addon/boss_addon_websocket.cpp
SOURCES += $$TOPPATH/addon/boss_addon_zip.cpp
SOURCES += $$TOPPATH/addon/boss_addon_zipa.cpp
SOURCES += $$TOPPATH/addon/boss_fakewin.cpp
SOURCES += $$TOPPATH/addon/boss_integration_abseil-cpp-20180600.cpp
SOURCES += $$TOPPATH/addon/boss_integration_curl-7.51.0.c
SOURCES += $$TOPPATH/addon/boss_integration_dlib-19.17.cpp
SOURCES += $$TOPPATH/addon/boss_integration_fdk-aac-0.1.4.cpp
SOURCES += $$TOPPATH/addon/boss_integration_jsoncpp-1.8.4.cpp
SOURCES += $$TOPPATH/addon/boss_integration_libgit2-0.22.2.c
SOURCES += $$TOPPATH/addon/boss_integration_libogg-1.3.3.c
SOURCES += $$TOPPATH/addon/boss_integration_libsrtp-2.2.0.c
SOURCES += $$TOPPATH/addon/boss_integration_libssh2-1.6.0.c
SOURCES += $$TOPPATH/addon/boss_integration_libvorbis-1.3.6.c
SOURCES += $$TOPPATH/addon/boss_integration_openalpr-2.3.0.cpp
SOURCES += $$TOPPATH/addon/boss_integration_openalpr-2.3.0_3rdparty_liblept.c
SOURCES += $$TOPPATH/addon/boss_integration_openalpr-2.3.0_3rdparty_libtiff.c
SOURCES += $$TOPPATH/addon/boss_integration_opencv-3.1.0.cpp
SOURCES += $$TOPPATH/addon/boss_integration_opencv-3.1.0_3rdparty_libjpeg.c
SOURCES += $$TOPPATH/addon/boss_integration_openh264-1.6.0.cpp
SOURCES += $$TOPPATH/addon/boss_integration_openh264-1.6.0_for_decoder.cpp
SOURCES += $$TOPPATH/addon/boss_integration_openh264-1.6.0_for_encoder.cpp
SOURCES += $$TOPPATH/addon/boss_integration_openssl-1.1.1a.c
SOURCES += $$TOPPATH/addon/boss_integration_psd_sdk-commit23.cpp
SOURCES += $$TOPPATH/addon/boss_integration_rnnoise-chromium.cpp
SOURCES += $$TOPPATH/addon/boss_integration_rtmpdump-2.4.c
SOURCES += $$TOPPATH/addon/boss_integration_tesseract-3.05.02.cpp
SOURCES += $$TOPPATH/addon/boss_integration_usrsctp-0.9.3.0.c
SOURCES += $$TOPPATH/addon/boss_integration_webrtc-jumpingyang001.cpp
SOURCES += $$TOPPATH/addon/boss_integration_websocketpp-0.8.1.cpp
SOURCES += $$TOPPATH/addon/boss_integration_ziparchive-4.6.7.cpp
SOURCES += $$TOPPATH/addon/boss_integration_ziparchive-4.6.7_for_bzip2.c
SOURCES += $$TOPPATH/addon/boss_integration_ziparchive-4.6.7_for_zlib.c

###########################################################
# CORE
###########################################################
HEADERS += $$TOPPATH/core/boss.h
HEADERS += $$TOPPATH/core/boss.hpp
HEADERS += $$TOPPATH/core/boss_addon.hpp
HEADERS += $$TOPPATH/core/boss_array.hpp
HEADERS += $$TOPPATH/core/boss_asset.hpp
HEADERS += $$TOPPATH/core/boss_buffer.hpp
HEADERS += $$TOPPATH/core/boss_context.hpp
HEADERS += $$TOPPATH/core/boss_counter.hpp
HEADERS += $$TOPPATH/core/boss_library.hpp
HEADERS += $$TOPPATH/core/boss_map.hpp
HEADERS += $$TOPPATH/core/boss_math.hpp
HEADERS += $$TOPPATH/core/boss_memory.hpp
HEADERS += $$TOPPATH/core/boss_mutex.hpp
HEADERS += $$TOPPATH/core/boss_object.hpp
HEADERS += $$TOPPATH/core/boss_parser.hpp
HEADERS += $$TOPPATH/core/boss_profile.hpp
HEADERS += $$TOPPATH/core/boss_queue.hpp
HEADERS += $$TOPPATH/core/boss_remote.hpp
HEADERS += $$TOPPATH/core/boss_share.hpp
HEADERS += $$TOPPATH/core/boss_storage.hpp
HEADERS += $$TOPPATH/core/boss_string.hpp
HEADERS += $$TOPPATH/core/boss_tasking.hpp
HEADERS += $$TOPPATH/core/boss_tree.hpp
HEADERS += $$TOPPATH/core/boss_updater.hpp
HEADERS += $$TOPPATH/core/boss_view.hpp
HEADERS += $$TOPPATH/core/boss_wstring.hpp
SOURCES += $$TOPPATH/core/boss.cpp
SOURCES += $$TOPPATH/core/boss_addon.cpp
SOURCES += $$TOPPATH/core/boss_asset.cpp
SOURCES += $$TOPPATH/core/boss_buffer.cpp
SOURCES += $$TOPPATH/core/boss_context.cpp
SOURCES += $$TOPPATH/core/boss_library.cpp
SOURCES += $$TOPPATH/core/boss_math.cpp
SOURCES += $$TOPPATH/core/boss_memory.cpp
SOURCES += $$TOPPATH/core/boss_mutex.cpp
SOURCES += $$TOPPATH/core/boss_parser.cpp
SOURCES += $$TOPPATH/core/boss_profile.cpp
SOURCES += $$TOPPATH/core/boss_remote.cpp
SOURCES += $$TOPPATH/core/boss_share.cpp
SOURCES += $$TOPPATH/core/boss_storage.cpp
SOURCES += $$TOPPATH/core/boss_string.cpp
SOURCES += $$TOPPATH/core/boss_tasking.cpp
SOURCES += $$TOPPATH/core/boss_updater.cpp
SOURCES += $$TOPPATH/core/boss_view.cpp
SOURCES += $$TOPPATH/core/boss_wstring.cpp

###########################################################
# ELEMENT
###########################################################
HEADERS += $$TOPPATH/element/boss_clip.hpp
HEADERS += $$TOPPATH/element/boss_color.hpp
HEADERS += $$TOPPATH/element/boss_font.hpp
HEADERS += $$TOPPATH/element/boss_image.hpp
HEADERS += $$TOPPATH/element/boss_matrix.hpp
HEADERS += $$TOPPATH/element/boss_point.hpp
HEADERS += $$TOPPATH/element/boss_rect.hpp
HEADERS += $$TOPPATH/element/boss_size.hpp
HEADERS += $$TOPPATH/element/boss_solver.hpp
HEADERS += $$TOPPATH/element/boss_tween.hpp
HEADERS += $$TOPPATH/element/boss_vector.hpp
SOURCES += $$TOPPATH/element/boss_clip.cpp
SOURCES += $$TOPPATH/element/boss_color.cpp
SOURCES += $$TOPPATH/element/boss_font.cpp
SOURCES += $$TOPPATH/element/boss_image.cpp
SOURCES += $$TOPPATH/element/boss_matrix.cpp
SOURCES += $$TOPPATH/element/boss_point.cpp
SOURCES += $$TOPPATH/element/boss_rect.cpp
SOURCES += $$TOPPATH/element/boss_size.cpp
SOURCES += $$TOPPATH/element/boss_solver.cpp
SOURCES += $$TOPPATH/element/boss_tween.cpp
SOURCES += $$TOPPATH/element/boss_vector.cpp

###########################################################
# FORMAT
###########################################################
HEADERS += $$TOPPATH/format/boss_bmp.hpp
HEADERS += $$TOPPATH/format/boss_flv.hpp
HEADERS += $$TOPPATH/format/boss_png.hpp
HEADERS += $$TOPPATH/format/boss_zen.hpp
SOURCES += $$TOPPATH/format/boss_bmp.cpp
SOURCES += $$TOPPATH/format/boss_flv.cpp
SOURCES += $$TOPPATH/format/boss_png.cpp
SOURCES += $$TOPPATH/format/boss_zen.cpp

###########################################################
# PLATFORM
###########################################################
HEADERS += $$TOPPATH/platform/boss_platform.hpp
HEADERS += $$TOPPATH/platform/boss_platform_blank.hpp
HEADERS += $$TOPPATH/platform/boss_platform_cocos2dx.hpp
HEADERS += $$TOPPATH/platform/boss_platform_impl.hpp
HEADERS += $$TOPPATH/platform/boss_platform_native.hpp
HEADERS += $$TOPPATH/platform/boss_platform_qt5.hpp
SOURCES += $$TOPPATH/platform/boss_platform_blank.cpp
SOURCES += $$TOPPATH/platform/boss_platform_cocos2dx.cpp
SOURCES += $$TOPPATH/platform/boss_platform_impl.cpp
SOURCES += $$TOPPATH/platform/boss_platform_native.cpp
SOURCES += $$TOPPATH/platform/boss_platform_qt5.cpp
win32-msvc*{
    HEADERS += $$TOPPATH/platform/win32/glew.h
}
ios{
    OBJECTIVE_HEADERS += $$TOPPATH/platform/ios/src/BossWebView.h
    OBJECTIVE_SOURCES += $$TOPPATH/platform/ios/src/BossWebView.mm
}
android{
    HEADERS += $$TOPPATH/platform/android/OpenGLES_Functions.h
    SOURCES += $$TOPPATH/platform/android/OpenGLES_Functions.cpp
}

###########################################################
# SERVICE
###########################################################
HEADERS += $$TOPPATH/service/boss_asyncfilemanager.hpp
HEADERS += $$TOPPATH/service/boss_backey.hpp
HEADERS += $$TOPPATH/service/boss_batchbuilder.hpp
HEADERS += $$TOPPATH/service/boss_boxr.hpp
HEADERS += $$TOPPATH/service/boss_brii.hpp
HEADERS += $$TOPPATH/service/boss_camera360service.hpp
HEADERS += $$TOPPATH/service/boss_firebaseservice.hpp
HEADERS += $$TOPPATH/service/boss_livechatservice.hpp
HEADERS += $$TOPPATH/service/boss_oauth2service.hpp
HEADERS += $$TOPPATH/service/boss_parasource.hpp
HEADERS += $$TOPPATH/service/boss_streamingservice.hpp
HEADERS += $$TOPPATH/service/boss_tryworld.hpp
HEADERS += $$TOPPATH/service/boss_typolar.hpp
HEADERS += $$TOPPATH/service/boss_zay.hpp
HEADERS += $$TOPPATH/service/boss_zayson.hpp
HEADERS += $$TOPPATH/service/boss_zaywidget.hpp
SOURCES += $$TOPPATH/service/boss_asyncfilemanager.cpp
SOURCES += $$TOPPATH/service/boss_backey.cpp
SOURCES += $$TOPPATH/service/boss_batchbuilder.cpp
SOURCES += $$TOPPATH/service/boss_boxr.cpp
SOURCES += $$TOPPATH/service/boss_brii.cpp
SOURCES += $$TOPPATH/service/boss_camera360service.cpp
SOURCES += $$TOPPATH/service/boss_firebaseservice.cpp
SOURCES += $$TOPPATH/service/boss_livechatservice.cpp
SOURCES += $$TOPPATH/service/boss_oauth2service.cpp
SOURCES += $$TOPPATH/service/boss_parasource.cpp
SOURCES += $$TOPPATH/service/boss_streamingservice.cpp
SOURCES += $$TOPPATH/service/boss_tryworld.cpp
SOURCES += $$TOPPATH/service/boss_typolar.cpp
SOURCES += $$TOPPATH/service/boss_zay.cpp
SOURCES += $$TOPPATH/service/boss_zayson.cpp
SOURCES += $$TOPPATH/service/boss_zaywidget.cpp
