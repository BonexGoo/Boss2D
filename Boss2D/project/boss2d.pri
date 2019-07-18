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
android: LIBS += -landroid
macx|ios: LIBS += -framework CoreFoundation
linux-g++: CONFIG += link_pkgconfig
equals(QT_ENABLE_GRAPHICS, "ok"){
    linux-g++: PKGCONFIG += gtk+-3.0
    # sudo apt-get install build-essential libgtk-3-dev
    # sudo apt-get install build-essential libgl1-mesa-dev
}
INCLUDEPATH += ../../Boss2D/core
!android: INCLUDEPATH += ../../Boss2D/addon/trick_for_fakewin
INCLUDEPATH += ../../Boss2D
CONFIG(debug, debug|release){
    DEFINES += BOSS_NDEBUG=0
}
CONFIG(release, debug|release){
    DEFINES += BOSS_NDEBUG=1
}

###########################################################
# ADDON
###########################################################
HEADERS += ../../Boss2D/addon/boss_addon_aac.hpp
HEADERS += ../../Boss2D/addon/boss_addon_alpr.hpp
HEADERS += ../../Boss2D/addon/boss_addon_curl.hpp
HEADERS += ../../Boss2D/addon/boss_addon_freetype.hpp
HEADERS += ../../Boss2D/addon/boss_addon_git.hpp
HEADERS += ../../Boss2D/addon/boss_addon_h264.hpp
HEADERS += ../../Boss2D/addon/boss_addon_jpg.hpp
HEADERS += ../../Boss2D/addon/boss_addon_ogg.hpp
HEADERS += ../../Boss2D/addon/boss_addon_opencv.hpp
HEADERS += ../../Boss2D/addon/boss_addon_ssl.hpp
HEADERS += ../../Boss2D/addon/boss_addon_tesseract.hpp
HEADERS += ../../Boss2D/addon/boss_addon_tif.hpp
HEADERS += ../../Boss2D/addon/boss_addon_webrtc.hpp
HEADERS += ../../Boss2D/addon/boss_addon_websocket.hpp
HEADERS += ../../Boss2D/addon/boss_addon_zip.hpp
HEADERS += ../../Boss2D/addon/boss_fakewin.h
HEADERS += ../../Boss2D/addon/boss_integration_abseil-cpp-20180600.h
HEADERS += ../../Boss2D/addon/boss_integration_curl-7.51.0.h
HEADERS += ../../Boss2D/addon/boss_integration_fdk-aac-0.1.4.h
HEADERS += ../../Boss2D/addon/boss_integration_jsoncpp-1.8.4.h
HEADERS += ../../Boss2D/addon/boss_integration_libgit2-0.22.2.h
HEADERS += ../../Boss2D/addon/boss_integration_libogg-1.3.3.h
HEADERS += ../../Boss2D/addon/boss_integration_libsrtp-2.2.0.h
HEADERS += ../../Boss2D/addon/boss_integration_libssh2-1.6.0.h
HEADERS += ../../Boss2D/addon/boss_integration_libvorbis-1.3.6.h
HEADERS += ../../Boss2D/addon/boss_integration_openalpr-2.3.0.h
HEADERS += ../../Boss2D/addon/boss_integration_openalpr-2.3.0_3rdparty_liblept.h
HEADERS += ../../Boss2D/addon/boss_integration_openalpr-2.3.0_3rdparty_libtiff.h
HEADERS += ../../Boss2D/addon/boss_integration_opencv-3.1.0.h
HEADERS += ../../Boss2D/addon/boss_integration_opencv-3.1.0_3rdparty_libjpeg.h
HEADERS += ../../Boss2D/addon/boss_integration_openh264-1.6.0.h
HEADERS += ../../Boss2D/addon/boss_integration_openssl-1.1.1a.h
HEADERS += ../../Boss2D/addon/boss_integration_rnnoise-chromium.h
HEADERS += ../../Boss2D/addon/boss_integration_rtmpdump-2.4.h
HEADERS += ../../Boss2D/addon/boss_integration_tesseract-3.05.02.h
HEADERS += ../../Boss2D/addon/boss_integration_usrsctp-0.9.3.0.h
HEADERS += ../../Boss2D/addon/boss_integration_webrtc-jumpingyang001.h
HEADERS += ../../Boss2D/addon/boss_integration_websocketpp-0.8.1.hpp
SOURCES += ../../Boss2D/addon/boss_addon_aac.cpp
SOURCES += ../../Boss2D/addon/boss_addon_alpr.cpp
SOURCES += ../../Boss2D/addon/boss_addon_curl.cpp
SOURCES += ../../Boss2D/addon/boss_addon_freetype.cpp
SOURCES += ../../Boss2D/addon/boss_addon_git.cpp
SOURCES += ../../Boss2D/addon/boss_addon_h264.cpp
SOURCES += ../../Boss2D/addon/boss_addon_jpg.cpp
SOURCES += ../../Boss2D/addon/boss_addon_ogg.cpp
SOURCES += ../../Boss2D/addon/boss_addon_opencv.cpp
SOURCES += ../../Boss2D/addon/boss_addon_ssl.cpp
SOURCES += ../../Boss2D/addon/boss_addon_tesseract.cpp
SOURCES += ../../Boss2D/addon/boss_addon_tif.cpp
SOURCES += ../../Boss2D/addon/boss_addon_webrtc.cpp
SOURCES += ../../Boss2D/addon/boss_addon_websocket.cpp
SOURCES += ../../Boss2D/addon/boss_addon_zip.cpp
SOURCES += ../../Boss2D/addon/boss_fakewin.cpp
SOURCES += ../../Boss2D/addon/boss_integration_abseil-cpp-20180600.cpp
SOURCES += ../../Boss2D/addon/boss_integration_curl-7.51.0.c
SOURCES += ../../Boss2D/addon/boss_integration_fdk-aac-0.1.4.cpp
SOURCES += ../../Boss2D/addon/boss_integration_jsoncpp-1.8.4.cpp
SOURCES += ../../Boss2D/addon/boss_integration_libgit2-0.22.2.c
SOURCES += ../../Boss2D/addon/boss_integration_libogg-1.3.3.c
SOURCES += ../../Boss2D/addon/boss_integration_libsrtp-2.2.0.c
SOURCES += ../../Boss2D/addon/boss_integration_libssh2-1.6.0.c
SOURCES += ../../Boss2D/addon/boss_integration_libvorbis-1.3.6.c
SOURCES += ../../Boss2D/addon/boss_integration_openalpr-2.3.0.cpp
SOURCES += ../../Boss2D/addon/boss_integration_openalpr-2.3.0_3rdparty_liblept.c
SOURCES += ../../Boss2D/addon/boss_integration_openalpr-2.3.0_3rdparty_libtiff.c
SOURCES += ../../Boss2D/addon/boss_integration_opencv-3.1.0.cpp
SOURCES += ../../Boss2D/addon/boss_integration_opencv-3.1.0_3rdparty_libjpeg.c
SOURCES += ../../Boss2D/addon/boss_integration_openh264-1.6.0.cpp
SOURCES += ../../Boss2D/addon/boss_integration_openh264-1.6.0_for_decoder.cpp
SOURCES += ../../Boss2D/addon/boss_integration_openh264-1.6.0_for_encoder.cpp
SOURCES += ../../Boss2D/addon/boss_integration_openssl-1.1.1a.c
SOURCES += ../../Boss2D/addon/boss_integration_rnnoise-chromium.cpp
SOURCES += ../../Boss2D/addon/boss_integration_rtmpdump-2.4.c
SOURCES += ../../Boss2D/addon/boss_integration_tesseract-3.05.02.cpp
SOURCES += ../../Boss2D/addon/boss_integration_usrsctp-0.9.3.0.c
SOURCES += ../../Boss2D/addon/boss_integration_webrtc-jumpingyang001.cpp
SOURCES += ../../Boss2D/addon/boss_integration_websocketpp-0.8.1.cpp

###########################################################
# CORE
###########################################################
HEADERS += ../../Boss2D/core/boss.h
HEADERS += ../../Boss2D/core/boss.hpp
HEADERS += ../../Boss2D/core/boss_addon.hpp
HEADERS += ../../Boss2D/core/boss_array.hpp
HEADERS += ../../Boss2D/core/boss_asset.hpp
HEADERS += ../../Boss2D/core/boss_buffer.hpp
HEADERS += ../../Boss2D/core/boss_context.hpp
HEADERS += ../../Boss2D/core/boss_counter.hpp
HEADERS += ../../Boss2D/core/boss_library.hpp
HEADERS += ../../Boss2D/core/boss_map.hpp
HEADERS += ../../Boss2D/core/boss_math.hpp
HEADERS += ../../Boss2D/core/boss_memory.hpp
HEADERS += ../../Boss2D/core/boss_mutex.hpp
HEADERS += ../../Boss2D/core/boss_object.hpp
HEADERS += ../../Boss2D/core/boss_parser.hpp
HEADERS += ../../Boss2D/core/boss_profile.hpp
HEADERS += ../../Boss2D/core/boss_queue.hpp
HEADERS += ../../Boss2D/core/boss_remote.hpp
HEADERS += ../../Boss2D/core/boss_share.hpp
HEADERS += ../../Boss2D/core/boss_storage.hpp
HEADERS += ../../Boss2D/core/boss_string.hpp
HEADERS += ../../Boss2D/core/boss_tasking.hpp
HEADERS += ../../Boss2D/core/boss_tree.hpp
HEADERS += ../../Boss2D/core/boss_updater.hpp
HEADERS += ../../Boss2D/core/boss_view.hpp
HEADERS += ../../Boss2D/core/boss_wstring.hpp
SOURCES += ../../Boss2D/core/boss.cpp
SOURCES += ../../Boss2D/core/boss_addon.cpp
SOURCES += ../../Boss2D/core/boss_asset.cpp
SOURCES += ../../Boss2D/core/boss_buffer.cpp
SOURCES += ../../Boss2D/core/boss_context.cpp
SOURCES += ../../Boss2D/core/boss_library.cpp
SOURCES += ../../Boss2D/core/boss_math.cpp
SOURCES += ../../Boss2D/core/boss_memory.cpp
SOURCES += ../../Boss2D/core/boss_mutex.cpp
SOURCES += ../../Boss2D/core/boss_parser.cpp
SOURCES += ../../Boss2D/core/boss_profile.cpp
SOURCES += ../../Boss2D/core/boss_remote.cpp
SOURCES += ../../Boss2D/core/boss_share.cpp
SOURCES += ../../Boss2D/core/boss_storage.cpp
SOURCES += ../../Boss2D/core/boss_string.cpp
SOURCES += ../../Boss2D/core/boss_tasking.cpp
SOURCES += ../../Boss2D/core/boss_updater.cpp
SOURCES += ../../Boss2D/core/boss_view.cpp
SOURCES += ../../Boss2D/core/boss_wstring.cpp

###########################################################
# ELEMENT
###########################################################
HEADERS += ../../Boss2D/element/boss_clip.hpp
HEADERS += ../../Boss2D/element/boss_color.hpp
HEADERS += ../../Boss2D/element/boss_font.hpp
HEADERS += ../../Boss2D/element/boss_image.hpp
HEADERS += ../../Boss2D/element/boss_matrix.hpp
HEADERS += ../../Boss2D/element/boss_point.hpp
HEADERS += ../../Boss2D/element/boss_rect.hpp
HEADERS += ../../Boss2D/element/boss_size.hpp
HEADERS += ../../Boss2D/element/boss_solver.hpp
HEADERS += ../../Boss2D/element/boss_tween.hpp
HEADERS += ../../Boss2D/element/boss_vector.hpp
SOURCES += ../../Boss2D/element/boss_clip.cpp
SOURCES += ../../Boss2D/element/boss_color.cpp
SOURCES += ../../Boss2D/element/boss_font.cpp
SOURCES += ../../Boss2D/element/boss_image.cpp
SOURCES += ../../Boss2D/element/boss_matrix.cpp
SOURCES += ../../Boss2D/element/boss_point.cpp
SOURCES += ../../Boss2D/element/boss_rect.cpp
SOURCES += ../../Boss2D/element/boss_size.cpp
SOURCES += ../../Boss2D/element/boss_solver.cpp
SOURCES += ../../Boss2D/element/boss_tween.cpp
SOURCES += ../../Boss2D/element/boss_vector.cpp

###########################################################
# FORMAT
###########################################################
HEADERS += ../../Boss2D/format/boss_bmp.hpp
HEADERS += ../../Boss2D/format/boss_flv.hpp
HEADERS += ../../Boss2D/format/boss_png.hpp
HEADERS += ../../Boss2D/format/boss_zen.hpp
SOURCES += ../../Boss2D/format/boss_bmp.cpp
SOURCES += ../../Boss2D/format/boss_flv.cpp
SOURCES += ../../Boss2D/format/boss_png.cpp
SOURCES += ../../Boss2D/format/boss_zen.cpp

###########################################################
# PLATFORM
###########################################################
HEADERS += ../../Boss2D/platform/boss_platform.hpp
HEADERS += ../../Boss2D/platform/boss_platform_blank.hpp
HEADERS += ../../Boss2D/platform/boss_platform_cocos2dx.hpp
HEADERS += ../../Boss2D/platform/boss_platform_impl.hpp
HEADERS += ../../Boss2D/platform/boss_platform_native.hpp
HEADERS += ../../Boss2D/platform/boss_platform_qt5.hpp
SOURCES += ../../Boss2D/platform/boss_platform_blank.cpp
SOURCES += ../../Boss2D/platform/boss_platform_cocos2dx.cpp
SOURCES += ../../Boss2D/platform/boss_platform_impl.cpp
SOURCES += ../../Boss2D/platform/boss_platform_native.cpp
SOURCES += ../../Boss2D/platform/boss_platform_qt5.cpp
win32-msvc*{
    HEADERS += ../../Boss2D/platform/win32/glew.h
}
ios{
    OBJECTIVE_HEADERS += $$PWD/../../Boss2D/platform/ios/src/BossWebView.h
    OBJECTIVE_SOURCES += $$PWD/../../Boss2D/platform/ios/src/BossWebView.mm
}
android{
    HEADERS += ../../Boss2D/platform/android/OpenGLES_Functions.h
    SOURCES += ../../Boss2D/platform/android/OpenGLES_Functions.cpp
}

###########################################################
# SERVICE
###########################################################
HEADERS += ../../Boss2D/service/boss_asyncfilemanager.hpp
HEADERS += ../../Boss2D/service/boss_backey.hpp
HEADERS += ../../Boss2D/service/boss_batchbuilder.hpp
HEADERS += ../../Boss2D/service/boss_boxr.hpp
HEADERS += ../../Boss2D/service/boss_brii.hpp
HEADERS += ../../Boss2D/service/boss_camera360service.hpp
HEADERS += ../../Boss2D/service/boss_firebaseservice.hpp
HEADERS += ../../Boss2D/service/boss_livechatservice.hpp
HEADERS += ../../Boss2D/service/boss_oauth2service.hpp
HEADERS += ../../Boss2D/service/boss_parasource.hpp
HEADERS += ../../Boss2D/service/boss_streamingservice.hpp
HEADERS += ../../Boss2D/service/boss_tryworld.hpp
HEADERS += ../../Boss2D/service/boss_typolar.hpp
HEADERS += ../../Boss2D/service/boss_zay.hpp
HEADERS += ../../Boss2D/service/boss_zayson.hpp
SOURCES += ../../Boss2D/service/boss_asyncfilemanager.cpp
SOURCES += ../../Boss2D/service/boss_backey.cpp
SOURCES += ../../Boss2D/service/boss_batchbuilder.cpp
SOURCES += ../../Boss2D/service/boss_boxr.cpp
SOURCES += ../../Boss2D/service/boss_brii.cpp
SOURCES += ../../Boss2D/service/boss_camera360service.cpp
SOURCES += ../../Boss2D/service/boss_firebaseservice.cpp
SOURCES += ../../Boss2D/service/boss_livechatservice.cpp
SOURCES += ../../Boss2D/service/boss_oauth2service.cpp
SOURCES += ../../Boss2D/service/boss_parasource.cpp
SOURCES += ../../Boss2D/service/boss_streamingservice.cpp
SOURCES += ../../Boss2D/service/boss_tryworld.cpp
SOURCES += ../../Boss2D/service/boss_typolar.cpp
SOURCES += ../../Boss2D/service/boss_zay.cpp
SOURCES += ../../Boss2D/service/boss_zayson.cpp
