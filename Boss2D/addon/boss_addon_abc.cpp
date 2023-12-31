#include <boss.h>

#if !defined(BOSS_NEED_ADDON_ABC) || (BOSS_NEED_ADDON_ABC != 0 && BOSS_NEED_ADDON_ABC != 1)
    #error BOSS_NEED_ADDON_ABC macro is invalid use
#endif
bool __LINK_ADDON_ABC__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_ABC

#include "boss_addon_abc.hpp"

#include <boss.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Abc, Create, id_abc, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Abc, Release, void, id_abc)
    BOSS_DECLARE_ADDON_FUNCTION(Abc, SetColor, void, id_abc, chars, double, double, double)
    BOSS_DECLARE_ADDON_FUNCTION(Abc, SetButton, void, id_abc, chars, double, double, double, double)
    BOSS_DECLARE_ADDON_FUNCTION(Abc, SetAction, void, id_abc, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Abc, AddRotation, void, id_abc, double, double, double)
    BOSS_DECLARE_ADDON_FUNCTION(Abc, Render, void, id_abc, sint32, sint32, sint32, sint32, AddOn::Abc::ButtonCB, payload)

    static autorun Bind_AddOn_Abc()
    {
        Core_AddOn_Abc_Create() = Customized_AddOn_Abc_Create;
        Core_AddOn_Abc_Release() = Customized_AddOn_Abc_Release;
        Core_AddOn_Abc_SetColor() = Customized_AddOn_Abc_SetColor;
        Core_AddOn_Abc_SetButton() = Customized_AddOn_Abc_SetButton;
        Core_AddOn_Abc_SetAction() = Customized_AddOn_Abc_SetAction;
        Core_AddOn_Abc_AddRotation() = Customized_AddOn_Abc_AddRotation;
        Core_AddOn_Abc_Render() = Customized_AddOn_Abc_Render;
        return true;
    }
    static autorun _ = Bind_AddOn_Abc();
}

#include <platform/win32/glew-2.2.0/include/GL/glew.h>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/Abc/All.h>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCoreOgawa/All.h>
#include <addon/alembic-1.8.5_for_boss/lib/Alembic/AbcCollection/All.h>

// 구현부
namespace BOSS
{
    class AbcClass
    {
    public:
        AbcClass()
        {
            mObject = nullptr;
            mCamera = nullptr;
        }
        ~AbcClass()
        {
            delete mObject;
            delete mCamera;
        }

    public:
        AbcOpenGL::SceneWrapper* mObject;
        AbcOpenGL::GLCamera* mCamera;
    };

    id_abc Customized_AddOn_Abc_Create(chars abcpath)
    {
        boss_file FileCheck = boss_fopen(abcpath, "rb");
        if(!FileCheck) return nullptr;
        boss_fclose(FileCheck);

        auto NewAbc = (AbcClass*) Buffer::Alloc<AbcClass>(BOSS_DBG 1);
        NewAbc->mObject = new AbcOpenGL::SceneWrapper(std::string(abcpath));
        NewAbc->mCamera = new AbcOpenGL::GLCamera();
        NewAbc->mCamera->setClippingPlanes(0.1, 100000);
        NewAbc->mCamera->frame(NewAbc->mObject->bounds());
        return (id_abc) NewAbc;
    }

    void Customized_AddOn_Abc_Release(id_abc abc)
    {
        Buffer::Free((buffer) abc);
    }

    void Customized_AddOn_Abc_SetColor(id_abc abc, chars name, double r, double g, double b)
    {
        auto CurAbc = (AbcClass*) abc;
        if(!CurAbc) return;

        const String OverrideText = "/" + String(name).Replace(".", "/");
        CurAbc->mObject->addOverrideColorString((chars) OverrideText, Alembic::Abc::C3f(r, g, b));
    }

    void Customized_AddOn_Abc_SetButton(id_abc abc, chars name, double x, double y, double z, double r)
    {
        auto CurAbc = (AbcClass*) abc;
        if(!CurAbc) return;
    }

    void Customized_AddOn_Abc_SetAction(id_abc abc, chars name)
    {
        auto CurAbc = (AbcClass*) abc;
        if(!CurAbc) return;
    }

    void Customized_AddOn_Abc_AddRotation(id_abc abc, double x, double y, double z)
    {
        auto CurAbc = (AbcClass*) abc;
        if(!CurAbc) return;

        auto OldValue = CurAbc->mCamera->rotation();
        CurAbc->mCamera->setRotation(Alembic::Abc::V3d(OldValue.x + x, OldValue.y + y, OldValue.z + z));
    }

    void Customized_AddOn_Abc_Render(id_abc abc, sint32 x, sint32 y, sint32 w, sint32 h, AddOn::Abc::ButtonCB cb, payload data)
    {
        auto CurAbc = (AbcClass*) abc;
        if(!CurAbc) return;

        glPointSize(1.0);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_AUTO_NORMAL);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_NORMALIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);
        {
            ////////////////////////////////////////////////////////////////////////////////
            // 라이팅
            GLfloat ambient[] = {0.2, 0.2, 0.3, 1.0};
            GLfloat diffuse[] = {0.9, 0.9, 0.9, 1.0};
            GLfloat position[] = {90.0, 90.0, 150.0, 0.0};
            GLfloat front_mat_shininess[] = {10.0};
            GLfloat front_mat_specular[] = {0.3, 0.3, 0.3, 1.0};
            GLfloat front_mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
            GLfloat back_mat_shininess[] = {10.0};
            GLfloat back_mat_specular[] = {0.2, 0.2, 0.2, 1.0};
            GLfloat back_mat_diffuse[] = {0.5, 0.5, 0.5, 1.0};
            GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};

            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glShadeModel(GL_SMOOTH);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
    
            glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
            glLightfv(GL_LIGHT0, GL_POSITION, position);

            glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess);
            glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
            glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess);
            glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular);
            glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);

            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient); // 특정 광원에 소속되지 않은 전역 주변광
            //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // 물체의 뒷면에도 조명을 적용
            glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // 물체면을 구성하는 모든 삼각형마다 별도의 시점벡터가 계산

            ////////////////////////////////////////////////////////////////////////////////
            // 애니메이션 진행
            CurAbc->mObject->playForward(60);

            glDrawBuffer(GL_BACK);
            glLoadIdentity();

            ////////////////////////////////////////////////////////////////////////////////
            // 카메라적용
            CurAbc->mCamera->setSize(w, h);
            CurAbc->mCamera->apply();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            CurAbc->mObject->draw(true, false);
        }
        glDisable(GL_DEPTH_TEST);
    }
}

#endif
