#include <boss.hpp>
#include "boss_library.hpp"

#include <stdio.h>

#if BOSS_WINDOWS
    #include <windows.h>
#elif BOSS_MAC_OSX || BOSS_IPHONE
    #include <CoreFoundation/CoreFoundation.h>
#elif BOSS_ANDROID
    #include <jni.h>
#endif

extern "C"
{
    extern int printf(const char*, ...);
}

namespace BOSS
{
    class ApiFunctions
    {
        public: chars Type;
        public: chars TypeForJava;
        public: chars FuncName;
        public: chars FuncFullName;
        public: chars Param;
        public: String ParamForJava;
        public: String Argument;
        public: ApiFunctions* Next;
        public: ApiFunctions(chars type, chars funcname, chars funcfullname, chars param)
            : Type(type), FuncName(funcname), FuncFullName(funcfullname), Param(param)
        {
            auto ApiToJava = [](chars typecode)->chars
            {
                if(boss_strcmp("void", typecode) == 0) return "void";
                else if(boss_strcmp("api_int", typecode) == 0) return "int";
                else if(boss_strcmp("api_uint", typecode) == 0) return "unsigned int";
                else if(boss_strcmp("api_flag", typecode) == 0) return "int";
                else if(boss_strcmp("api_enum", typecode) == 0) return "int";
                else if(boss_strcmp("api_long", typecode) == 0) return "long";
                else if(boss_strcmp("api_float", typecode) == 0) return "float";
                else if(boss_strcmp("api_object", typecode) == 0) return "Object";
                else if(boss_strcmp("api_string", typecode) == 0) return "String";
                else if(boss_strcmp("api_bytes", typecode) == 0) return "byte[]";
                else if(boss_strcmp("api_floats", typecode) == 0) return "float[]";
                else if(boss_strcmp("api_results", typecode) == 0) return "String";
                else BOSS_ASSERT("알 수 없는 typecode값으로 ApiFunctions구성에 실패하였습니다", false);
                return "unknown_type";
            };
            if(type) TypeForJava = ApiToJava(type);
            if(param && boss_strcmp("void", param) != 0)
            {
                for(sint32 i = 0; Param[i]; ++i)
                {
                    if(Param[i] == 'a' && Param[i + 1] == 'p' && Param[i + 2] == 'i' && Param[i + 3] == '_')
                    {
                        const sint32 iOld = i; i += 3;
                        while(Param[++i] != ' ');
                        ParamForJava += ApiToJava(String(&Param[iOld], i - iOld));
                        ParamForJava += ' ';
                        while(Param[++i] == ' ');
                    }
                    ParamForJava += Param[i];
                    Argument += Param[i];
                }
            }
            Next = nullptr;
        }
        public: ~ApiFunctions() {delete Next;}
        public: static ApiFunctions* Top()
        {static ApiFunctions _(nullptr, nullptr, nullptr, nullptr); return &_;}
    };

    autorun CollectForPlatform(chars type, chars funcname, chars funcfullname, chars param)
    {
        ApiFunctions* CurFunction = ApiFunctions::Top();
        while(CurFunction->Next) CurFunction = CurFunction->Next;
        CurFunction->Next = new ApiFunctions(type, funcname, funcfullname, param);
        return true;
    }

    void ReadyForPlatform(void* env)
    {
        #if BOSS_ANDROID
            static JavaVM* jvm = nullptr;
            ((JNIEnv*) env)->GetJavaVM(&jvm);
            jvm->AttachCurrentThread((JNIEnv**) &env, 0);
        #endif
    }

    api_string ReturnForPlatform(void* env, chars data)
    {
        #if BOSS_ANDROID
            return ((JNIEnv*) env)->NewStringUTF(data);
        #else
            return data;
        #endif
    }

    api_bytes ReturnForPlatform(void* env, bytes data, const sint32 len)
    {
        #if BOSS_ANDROID
            jbyteArray result = ((JNIEnv*) env)->NewByteArray(len);
            Memory::Copy(result, data, sizeof(unsigned char) * len);
            return result;
        #else
            return data;
        #endif
    }

    api_floats ReturnForPlatform(void* env, const float* data, const sint32 len)
    {
        #if BOSS_ANDROID
            jfloatArray result = ((JNIEnv*) env)->NewFloatArray(len);
            Memory::Copy(result, data, sizeof(float) * len);
            return result;
        #else
            return data;
        #endif
    }

    api_results ResultsForPlatform(void* env, char* temp, sint32 tempsize, uint32 a, uint32 b)
    {
        boss_snprintf(temp, tempsize, "%u|%u", a, b);
        #if BOSS_ANDROID
            return ((JNIEnv*) env)->NewStringUTF(temp);
        #else
            return temp;
        #endif
    }

    api_results ResultsForPlatform(void* env, char* temp, sint32 tempsize, float a, float b, float c)
    {
        boss_snprintf(temp, tempsize, "%f|%f|%f", a, b, c);
        #if BOSS_ANDROID
            return ((JNIEnv*) env)->NewStringUTF(temp);
        #else
            return temp;
        #endif
    }

    api_results ResultsForPlatform(void* env, char* temp, sint32 tempsize, float a, float b, float c, float d)
    {
        boss_snprintf(temp, tempsize, "%f|%f|%f|%f", a, b, c, d);
        #if BOSS_ANDROID
            return ((JNIEnv*) env)->NewStringUTF(temp);
        #else
            return temp;
        #endif
    }

    #if BOSS_ANDROID
        ObjectForPlatform::ObjectForPlatform(void* env, api_object obj)
            : GlobalRef((void*) ((JNIEnv*) env)->NewGlobalRef((jobject) obj)),
                Env(env), Obj(obj) {}
        ObjectForPlatform::~ObjectForPlatform()
        {((JNIEnv*) Env)->DeleteGlobalRef((jobject) GlobalRef);}
        ObjectForPlatform::operator void*() const
        {return GlobalRef;}
    #else
        ObjectForPlatform::ObjectForPlatform(void* env, api_object obj)
            : GlobalRef(nullptr), Env(env), Obj(obj) {}
        ObjectForPlatform::~ObjectForPlatform() {}
        ObjectForPlatform::operator void*() const
        {return GlobalRef;}
    #endif

    #if BOSS_ANDROID
        StringForPlatform::StringForPlatform(void* env, api_string str)
            : Utf8((const char*) ((JNIEnv*) env)->GetStringUTFChars((jstring) str, nullptr)),
                Env(env), Str(str) {}
        StringForPlatform::~StringForPlatform()
        {((JNIEnv*) Env)->ReleaseStringUTFChars((jstring) Str, Utf8);}
        StringForPlatform::operator const char*() const
        {return Utf8;}
    #else
        StringForPlatform::StringForPlatform(void* env, api_string str)
            : Utf8(nullptr), Env(env), Str(str) {}
        StringForPlatform::~StringForPlatform() {}
        StringForPlatform::operator const char*() const
        {return Str;}
    #endif

    #if BOSS_ANDROID
        ByteArrayForPlatform::ByteArrayForPlatform(void* env, api_bytes brr)
            : Bytes((const unsigned char*) ((JNIEnv*) env)->GetByteArrayElements((jbyteArray) brr, nullptr)),
                Env(env), Brr(brr) {}
        ByteArrayForPlatform::~ByteArrayForPlatform()
        {((JNIEnv*) Env)->ReleaseByteArrayElements((jbyteArray) Brr, (jbyte*) Bytes, JNI_ABORT);}
        ByteArrayForPlatform::operator const unsigned char*() const
        {return Bytes;}
    #else
        ByteArrayForPlatform::ByteArrayForPlatform(void* env, api_bytes brr)
            : Bytes(nullptr), Env(env), Brr(brr) {}
        ByteArrayForPlatform::~ByteArrayForPlatform() {}
        ByteArrayForPlatform::operator const unsigned char*() const
        {return Brr;}
    #endif

    #if BOSS_ANDROID
        FloatArrayForPlatform::FloatArrayForPlatform(void* env, api_floats frr)
            : Floats((const float*) ((JNIEnv*) env)->GetFloatArrayElements((jfloatArray) frr, nullptr)),
                Env(env), Frr(frr) {}
        FloatArrayForPlatform::~FloatArrayForPlatform()
        {((JNIEnv*) Env)->ReleaseFloatArrayElements((jfloatArray) Frr, (jfloat*) Floats, JNI_ABORT);}
        FloatArrayForPlatform::operator const float*() const
        {return Floats;}
    #else
        FloatArrayForPlatform::FloatArrayForPlatform(void* env, api_floats frr)
            : Floats(nullptr), Env(env), Frr(frr) {}
        FloatArrayForPlatform::~FloatArrayForPlatform() {}
        FloatArrayForPlatform::operator const float*() const
        {return Frr;}
    #endif

    class ApiBuilder
    {
        private: void* FilePtr;
        private: String Package;
        public: ApiBuilder(void* fileptr, chars package) : FilePtr(fileptr), Package(package) {}
        public: ~ApiBuilder() {}
        public: void WriteTypes(chars value)
        {
            char Temp[1024];
            boss_snprintf(Temp, 1024,
                "#pragma once\r\n"
                "\r\n"
                "////////////////////////////////////////////////////////////////////////////////\r\n"
                "typedef int                    api_int;\r\n"
                "typedef unsigned int           api_uint;\r\n"
                "typedef int                    api_flag;\r\n"
                "typedef int                    api_enum;\r\n"
                "typedef long long              api_long;\r\n"
                "typedef float                  api_float;\r\n"
                "typedef void*                  api_object;\r\n"
                "typedef const char*            api_string;\r\n"
                "typedef const unsigned char*   api_bytes;\r\n"
                "typedef const float*           api_floats;\r\n"
                "typedef const char*            api_results;\r\n"
                "\r\n"
                "////////////////////////////////////////////////////////////////////////////////\r\n"
                "void Init%s(%s dll);\r\n", (chars) Package, value);
            String Text = Temp;
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
        }
        public: void WriteDeclares()
        {
            ApiFunctions* CurFunction = ApiFunctions::Top();
            String LastGroupName = "";
            while(CurFunction = CurFunction->Next)
            {
                sint32 GroupNameLength = 0;
                for(int i = 0; CurFunction->FuncFullName[i]; ++i)
                    if(CurFunction->FuncFullName[i] == '_')
                        GroupNameLength = i;
                String CurGroupName(CurFunction->FuncFullName, GroupNameLength);
                if(LastGroupName.Compare(CurGroupName, 0) != 0)
                {
                    LastGroupName = CurGroupName;
                    String Text =
                        "\r\n"
                        "////////////////////////////////////////////////////////////////////////////////\r\n";
                    boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
                }

                char Temp[1024];
                boss_snprintf(Temp, 1024, "%s %s(%s);\r\n",
                    CurFunction->Type, CurFunction->FuncFullName, CurFunction->Param);
                String Text = Temp;
                boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            }
        }
        public: void WriteIncludes(chars value)
        {
            char Temp[1024];
            boss_snprintf(Temp, 1024,
                "#include %s\r\n"
                "#include \"%s.h\"\r\n", (chars) Package, value);
            String Text = Temp;
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
        }
        public: void WriteTypedefs()
        {
            String Text =
                "\r\n"
                "////////////////////////////////////////////////////////////////////////////////\r\n";
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            ApiFunctions* CurFunction = ApiFunctions::Top();
            while(CurFunction = CurFunction->Next)
            {
                char Temp[1024];
                boss_snprintf(Temp, 1024,
                    "extern \"C\" typedef %s (*_%s)(%s);\r\n"
                    "static _%s DLL_%s = nullptr;\r\n",
                    CurFunction->Type, CurFunction->FuncFullName, CurFunction->Param,
                    CurFunction->FuncFullName, CurFunction->FuncFullName);
                Text = Temp;
                boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            }
        }
        public: void WriteInitializer(chars value1, chars value2)
        {
            char Temp[1024];
            boss_snprintf(Temp, 1024,
                "\r\n"
                "////////////////////////////////////////////////////////////////////////////////\r\n"
                "void Init%s(%s dll)\r\n"
                "{\r\n", (chars) Package, value1);
            String Text = Temp;
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            ApiFunctions* CurFunction = ApiFunctions::Top();
            while(CurFunction = CurFunction->Next)
            {
                boss_snprintf(Temp, 1024,
                    "\tDLL_%s = (_%s) %s(dll, \"%s\");\r\n",
                    CurFunction->FuncFullName, CurFunction->FuncFullName, value2, CurFunction->FuncFullName);
                Text = Temp;
                boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            }
            Text = "}\r\n";
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
        }
        public: void WriteImplement()
        {
            String Text = "\r\n"
                "////////////////////////////////////////////////////////////////////////////////";
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            ApiFunctions* CurFunction = ApiFunctions::Top();
            while(CurFunction = CurFunction->Next)
            {
                char Temp[1024];
                boss_snprintf(Temp, 1024,
                    "\r\n"
                    "%s %s(%s)\r\n"
                    "{\r\n"
                    "\t%sDLL_%s(%s);\r\n"
                    "}\r\n",
                    CurFunction->Type, CurFunction->FuncFullName, CurFunction->Param,
                    (boss_strcmp("void", CurFunction->Type) == 0)? "" : "return ",
                    CurFunction->FuncFullName, (chars) CurFunction->Argument);
                Text = Temp;
                boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            }
        }
        public: void WritePackageAndImport(chars company)
        {
            String Text = String::Format(
                "package com.%s;\r\n"
                "import android.app.Application;\r\n", company);
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
        }
        public: void WriteClass(chars application)
        {
            String Text = String::Format(
                "\r\npublic class %s\r\n"
                "{\r\n", application);
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            WriteLoadLibrary(application, "\t");
            WriteNativeFunctions("\t");
            Text = "}\r\n";
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
        }
        public: void WriteLoadLibrary(chars application, chars tab)
        {
            String Text = String::Format(
                "%sstatic\r\n"
                "%s{\r\n"
                "%s\ttry\r\n"
                "%s\t{\r\n"
                "%s\t\tSystem.loadLibrary(\"%s\");\r\n"
                "%s\t}\r\n"
                "%s\tcatch (UnsatisfiedLinkError e)\r\n"
                "%s\t{\r\n"
                "%s\t\tSystem.err.println(\"Native code library failed to load.\" + e);\r\n"
                "%s\t\tSystem.exit(1);\r\n"
                "%s\t}\r\n"
                "%s}\r\n", tab, tab, tab, tab, tab, application, tab, tab, tab, tab, tab, tab, tab);
            boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
        }
        public: void WriteNativeFunctions(chars tab)
        {
            ApiFunctions* CurFunction = ApiFunctions::Top();
            while(CurFunction = CurFunction->Next)
            {
                String Text = String::Format(
                    "%spublic native %s %s(%s);\r\n",
                    tab, CurFunction->TypeForJava, CurFunction->FuncName, (chars) CurFunction->ParamForJava);
                boss_fwrite((chars) Text, 1, Text.Length(), FilePtr);
            }
        }
    };
}

BOSS_API_FUNC(BOSS, void, PrintFunctions)()
{
    ApiFunctions* CurFunction = ApiFunctions::Top();
    for(sint32 i = 0; CurFunction = CurFunction->Next; ++i)
        printf("%03d> %s %s(%s)\r\n", i,
            CurFunction->Type, CurFunction->FuncFullName, CurFunction->Param);
}

BOSS_API_FUNC(BOSS, void, BuildForMFC)(chars package)
{
    void* HeaderFile = boss_fopen(String(package) + ".h", "wb");
    ApiBuilder(HeaderFile, package).WriteTypes("HMODULE");
    ApiBuilder(HeaderFile, package).WriteDeclares();
    boss_fclose(HeaderFile);

    void* SourceFile = boss_fopen(String(package) + ".cpp", "wb");
    ApiBuilder(SourceFile, package).WriteIncludes("\"stdafx.h\"");
    ApiBuilder(SourceFile, package).WriteTypedefs();
    ApiBuilder(SourceFile, package).WriteInitializer("HMODULE", "GetProcAddress");
    ApiBuilder(SourceFile, package).WriteImplement();
    boss_fclose(SourceFile);
}

BOSS_API_FUNC(BOSS, void, BuildForXCODE)(chars package)
{
    void* HeaderFile = boss_fopen(String(package) + ".h", "wb");
    ApiBuilder(HeaderFile, package).WriteTypes("void*");
    ApiBuilder(HeaderFile, package).WriteDeclares();
    boss_fclose(HeaderFile);

    void* SourceFile = boss_fopen(String(package) + ".mm", "wb");
    ApiBuilder(SourceFile, package).WriteIncludes("<dlfcn.h>");
    ApiBuilder(SourceFile, package).WriteTypedefs();
    ApiBuilder(SourceFile, package).WriteInitializer("void*", "dlsym");
    ApiBuilder(SourceFile, package).WriteImplement();
    boss_fclose(SourceFile);
}

BOSS_API_FUNC(BOSS, void, BuildForJAVA)(chars package)
{
    const String PackageName = String(package).Replace('_', '.');
    sint32 LastDotIndex = -1;
    for(sint32 i = 0; PackageName[i] != '\0'; ++i)
        if(PackageName[i] == '.') LastDotIndex = i;
    const String CompanyName = (LastDotIndex == -1)? String("unknown") : PackageName.Left(LastDotIndex);
    const String ApplicationName = PackageName.Right(PackageName.Length() - LastDotIndex - 1);

    void* SourceFile = boss_fopen(ApplicationName + ".java", "wb");
    ApiBuilder(SourceFile, package).WritePackageAndImport(CompanyName);
    ApiBuilder(SourceFile, package).WriteClass(ApplicationName);
    boss_fclose(SourceFile);
}
