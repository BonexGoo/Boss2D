#include "../bodymovin/bmconstants_p.h"

#include <boss.h>

// QTCREATOR가 아닌 경우 MOC코드 포함
#if !defined(BOSS_QTCREATOR) || BOSS_QTCREATOR != 1
    #if BOSS_NDEBUG
        #if BOSS_X64
            #include "../../../../../project/GeneratedFiles/Release64/moc_bmconstants_p.cpp"
        #else
            #include "../../../../../project/GeneratedFiles/Release32/moc_bmconstants_p.cpp"
        #endif
    #else
        #if BOSS_X64
            #include "../../../../../project/GeneratedFiles/Debug64/moc_bmconstants_p.cpp"
        #else
            #include "../../../../../project/GeneratedFiles/Debug32/moc_bmconstants_p.cpp"
        #endif
    #endif
#endif
