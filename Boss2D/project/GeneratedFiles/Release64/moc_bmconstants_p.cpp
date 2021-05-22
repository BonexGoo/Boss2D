/****************************************************************************
** Meta object code from reading C++ file 'bmconstants_p.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../platform/plugin_qt5/qtlottie/src/bodymovin/bmconstants_p.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bmconstants_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BMLiteral_t {
    QByteArrayData data[5];
    char stringdata0[59];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BMLiteral_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BMLiteral_t qt_meta_stringdata_BMLiteral = {
    {
QT_MOC_LITERAL(0, 0, 9), // "BMLiteral"
QT_MOC_LITERAL(1, 10, 12), // "PropertyType"
QT_MOC_LITERAL(2, 23, 12), // "RectPosition"
QT_MOC_LITERAL(3, 36, 8), // "RectSize"
QT_MOC_LITERAL(4, 45, 13) // "RectRoundness"

    },
    "BMLiteral\0PropertyType\0RectPosition\0"
    "RectSize\0RectRoundness"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BMLiteral[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x0,    3,   19,

 // enum data: key, value
       2, uint(BMLiteral::RectPosition),
       3, uint(BMLiteral::RectSize),
       4, uint(BMLiteral::RectRoundness),

       0        // eod
};

void BMLiteral::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject BMLiteral::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_BMLiteral.data,
    qt_meta_data_BMLiteral,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BMLiteral::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BMLiteral::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BMLiteral.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BMLiteral::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
