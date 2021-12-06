qt_internal_add_test(tst_imports
    GUI
    QMLTEST
    SOURCES
        tst_imports.cpp
    PUBLIC_LIBRARIES
        Qt::Quick
    TESTDATA
        data/tst_main.qml
)

qt_internal_add_resource(tst_imports "imports"
    PREFIX
        "/"
    FILES
        data/rec_pos_col_opa.json
)
