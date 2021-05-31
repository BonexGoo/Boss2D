qt_internal_add_test(tst_shapelayer
    SOURCES
        tst_bmshapelayer.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Test
        Qt::BodymovinPrivate
)
