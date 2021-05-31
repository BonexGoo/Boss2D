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
    BOSS_DECLARE_ADDON_FUNCTION(Abc, Render, void, id_abc, float, float, uint32)

    static autorun Bind_AddOn_Abc()
    {
        Core_AddOn_Abc_Create() = Customized_AddOn_Abc_Create;
        Core_AddOn_Abc_Release() = Customized_AddOn_Abc_Release;
        Core_AddOn_Abc_Render() = Customized_AddOn_Abc_Render;
        return true;
    }
    static autorun _ = Bind_AddOn_Abc();
}

#include <platform/win32/glew-2.2.0/include/GL/glew.h>

// 구현부
namespace BOSS
{
    //AbcOpenGL::GLCamera* gCamera;
    AbcOpenGL::SceneWrapper* gObject;

    id_abc Customized_AddOn_Abc_Create(chars abcpath)
    {
        //gCamera = new AbcOpenGL::GLCamera();
        //gCamera->setFovy(45.0);
        //gCamera->setClippingPlanes(0.1, 100000);
        //gCamera->setCenterOfInterest(0.1);
        //gCamera->setSize(2000, 2000);

        gObject = new AbcOpenGL::SceneWrapper(std::string(abcpath));
        //gObject->selection(0, 0, *gCamera);
        //gCamera->frame(gObject->bounds());
        return nullptr;
    }

    void Customized_AddOn_Abc_Release(id_abc abc)
    {
        //delete gCamera;
        delete gObject;

        Buffer::Free((buffer) abc);
    }

    static void SetLight()
    {
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

        //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient); // 특정 광원에 소속되지 않은 전역 주변광
        //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // 물체의 뒷면에도 조명을 적용
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // 물체면을 구성하는 모든 삼각형마다 별도의 시점벡터가 계산
    }

    void Customized_AddOn_Abc_Render(id_abc abc, float x, float y, uint32 fbo)
    {
        // 카메라조절
        //gCamera->dolly(Imath::V2d(1, 1));
        //gCamera->track(Imath::V2d(1, 1));
        //gCamera->rotate(Imath::V2d(1, 1));

        ////////////////////////////////////////////////////////////////////////////////
        glPointSize(1.0);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_AUTO_NORMAL);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_NORMALIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glShadeModel(GL_SMOOTH);

        SetLight();

        ////////////////////////////////////////////////////////////////////////////////
        // 애니메이션
        gObject->playForward(60);

        ////////////////////////////////////////////////////////////////////////////////
        glClearColor(1.0, 1.0, 0.8, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawBuffer(GL_BACK);
        glLoadIdentity();

        // 카메라워크
        //gCamera->apply();
        if(false) // Gen_Car.abc
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            ::glScaled(0.001, 0.001, 0.001);
            ::glRotated(-30, 1.0, 0.0, 0.0);
            ::glRotated(-0, 0.0, 1.0, 0.0);
            ::glRotated(-0, 0.0, 0.0, 1.0);
            ::glTranslated(-1500, -0, -0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            ////////////////////////////////////////////////////////////////////////////////
            gObject->draw(true, false);
        }
        else if(false) // particle_v01_test.abc
        {
            ::glRotated(-45, 1.0, 0.0, 0.0);
            ::glRotated(-0, 0.0, 1.0, 0.0);
            ::glRotated(-0, 0.0, 0.0, 1.0);
            ::glTranslated(500, 500, -0);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            ////////////////////////////////////////////////////////////////////////////////
            gObject->draw(false, true);
        }
        else if(false) // Aston_Martin_DBS_ABC.abc
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, 1.0, 0.1f, 1000);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            ::glScaled(0.001, 0.001, 0.001);
            ::glRotated(-20, 1.0, 0.0, 0.0);
            ::glRotated(-30, 0.0, 1.0, 0.0);
            ::glRotated(-0, 0.0, 0.0, 1.0);
            ::glTranslated(-200, -0, -500);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            ////////////////////////////////////////////////////////////////////////////////
            gObject->draw(true, false);
        }
        else if(true) // Pagani Huayra BC.abc
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, 1.0, 0.1f, 1000);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            ::glScaled(0.001, 0.001, 0.001);
            ::glRotated(-0, 1.0, 0.0, 0.0);
            ::glRotated(-0, 0.0, 1.0, 0.0);
            ::glRotated(-0, 0.0, 0.0, 1.0);
            ::glTranslated(-0, -0, -200);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            ////////////////////////////////////////////////////////////////////////////////
            gObject->draw(true, false);
        }
    }
}

#endif
