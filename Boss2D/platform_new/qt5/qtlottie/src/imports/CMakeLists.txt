qt_internal_add_qml_module(lottieqtplugin
    URI "Qt.labs.lottieqt"
    VERSION "${PROJECT_VERSION}"
    CLASSNAME BodymovinPlugin
    SKIP_TYPE_REGISTRATION
    SOURCES
        lottie_plugin.cpp lottie_plugin.h
        lottieanimation.cpp lottieanimation.h
        rasterrenderer/batchrenderer.cpp rasterrenderer/batchrenderer.h
        rasterrenderer/lottierasterrenderer.cpp rasterrenderer/lottierasterrenderer.h
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::GuiPrivate
        Qt::Qml
        Qt::Quick
        Qt::BodymovinPrivate
)

qt_internal_add_docs(lottieqtplugin
    doc/qtlottieanimation.qdocconf
)
