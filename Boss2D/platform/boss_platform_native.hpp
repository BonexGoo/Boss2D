#pragma once
#include <platform/boss_platform.hpp>
#include <platform/boss_platform_impl.hpp>

#ifdef BOSS_PLATFORM_NATIVE

    #include <element/boss_rect.hpp>
    #include <element/boss_color.hpp>

    #if BOSS_NEED_NATIVE_OPENGL
    # if BOSS_WINDOWS
    #  include <platform/win32/glew-2.2.0/include/GL/glew.h>
    # if BOSS_X64
    #  pragma comment(lib, "../../Boss2D/platform/win32/glew-2.2.0/bin/Release/x64/glew32.lib")
    # else
    #  pragma comment(lib, "../../Boss2D/platform/win32/glew-2.2.0/bin/Release/Win32/glew32.lib")
    # endif
    #  pragma comment(lib, "opengl32.lib")
    #  pragma comment(lib, "glu32.lib")
    #  pragma comment(lib, "user32.lib")
    #  pragma comment(lib, "comdlg32.lib")
    #  pragma comment(lib, "ole32.lib")
    # elif BOSS_MAC_OSX
    #  include <OpenGL/gl3.h>
    #  include <OpenGL/gl3ext.h>
    # elif BOSS_IPHONE
    #  include <OpenGLES/ES3/gl.h>
    #  include <OpenGLES/ES3/glext.h>
    #  ifdef __OBJC__
    #   import <OpenGLES/EAGLDrawable.h>
    #  endif
    # elif BOSS_ANDROID
    #  include <EGL/egl.h>
    #  include <android/native_window_jni.h>
    #  include <platform/android/OpenGLES_Functions.h>
    # endif
    #else
    # define GLint   sint32
    # define GLuint  uint32
    # define GLubyte uint08
    # define GLfloat float
    #endif

    class NativePainter
    {
    public:
        NativePainter();
        ~NativePainter();

    public:
        void reset(const sint32 width, const sint32 height);
        void setZoom(const float zoom);
        void fillRect(const BOSS::Rect& rect, const BOSS::Color& color);

    private:
        void initShaderES(chars vsource, chars fsource);
        void initShaderES20();
        void initShaderES30();
        void termShaderES();

    private:
        uint08 __version();
        void __testGL(BOSS_DBG_PRM sint32 nouse);
        void __testShader(BOSS_DBG_PRM GLuint shader);
        void __testProgram(BOSS_DBG_PRM GLuint program);
        void __loadIdentity();
        void __multiply(
            const float m00, const float m01, const float m02, const float m03,
            const float m10, const float m11, const float m12, const float m13,
            const float m20, const float m21, const float m22, const float m23,
            const float m30, const float m31, const float m32, const float m33);

    private:
        enum {VerticeID = 0, ColorID = 1};
        struct Attrib
        {
            GLfloat vertices[2];
            union
            {
                GLubyte colors[4];
                GLuint color32;
            };
        };
        GLuint m_vshader;
        GLuint m_fshader;
        GLuint m_program;
        Attrib m_attrib[4];
        GLint m_matrix;
        GLfloat m_m[4][4];
        sint32 m_width;
        sint32 m_height;
    };

    class ViewAPI;
    extern ViewAPI* g_view;

    class ViewAPI
    {
    public:
        ViewAPI(View* manager = nullptr);
        ViewAPI(const ViewAPI& rhs);
        ViewAPI& operator=(const ViewAPI& rhs);
        ~ViewAPI();

    public:
        static NativePainter*& CurPainter();
        static BOSS::Color& CurColor();

    public:
        h_view changeViewManager(View* manager);
        void setNextViewManager(View* manager);
        void render(sint32 width, sint32 height);
        void touch(TouchType type, sint32 id, sint32 x, sint32 y);
        void sendCreate();

    private:
        View* m_view_manager;
        View* m_next_manager;
    };

#endif
