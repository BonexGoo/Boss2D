﻿#pragma once
#include <platform/boss_platform.hpp>

namespace BOSS
{
    namespace PlatformImpl
    {
        // 일반함수와 전역인스턴스
        namespace Core
        {
            void FlushProcedure();
            bool CallProcedures(sint32 msec);
            String NormalPath(chars itemname, bool QCodeTest = true);
            WString NormalPathW(wchars itemname, bool QCodeTest = true);
            void SetRoot(sint32 i, chars dirname);
            String GetCopiedRoot(sint32 i);

            extern View::CreatorCB g_Creator;
        }

        // 래핑함수
        namespace Wrap
        {
            sint32 AddProcedure(ProcedureCB cb, payload data, sint32 msec);
            void SubProcedure(sint32 id);
            void SetUserEventListener(chars event, UserEventCB cb, payload data);
            void ClearUserEventListener(chars event);
            void SendUserEvent(chars event, chars args);

            chars Utility_GetOSName();
            sint32 Utility_EnumPrograms(Context& programs, bool visible_only);
            chars Utility_GetDeviceID();
            sint64 Utility_CurrentAvailableMemory(sint64* totalbytes);
            double Utility_CurrentTrafficCPU();

            void Option_SetOptionFlag(chars name, bool flag);
            bool Option_GetOptionFlag(chars name);
            Strings Option_GetOptionFlagNames();
            void Option_SetOptionText(chars name, chars text);
            chars Option_GetOptionText(chars name);
            Strings Option_GetOptionTextNames();
            void Option_SetOptionPayload(chars name, payload data);
            payload Option_GetOptionPayload(chars name);
            Strings Option_GetOptionPayloadNames();

            bool Popup_FileDialog(DialogShellType type, String& path, String* shortpath, chars title, wchars filters, sint32* filterresult);
            sint32 Popup_MessageDialog(chars title, chars text, DialogButtonType type);
            void Popup_WebBrowserDialog(String url);
            void Popup_ProgramDialog(chars exepath, chars args, bool admin, bool hide, chars dirpath, ublock* getpid = nullptr);
            void Popup_KillProgramDialog(ublock pid);
            sint64 Popup_FindWindowHandle(chars titlename);
            bool Popup_MoveWindow(sint64 hwnd, sint32 left, sint32 top, sint32 right, sint32 bottom, bool repaint);
            bool Popup_MoveWindowGroup(sint64s windowparams);
            bool Popup_MoveWindowGroupCaptured(sint64s windowparams, bool release);

            void File_SetAttributeHidden(wchars itemname);
            WString File_GetDirName(wchars itemname, wchar_t badslash, wchar_t goodslash);
            WString File_GetShortName(wchars itemname);
            WString File_ExtensionName(wchars itemname);
            sint32 File_GetDriveCode();
            bool File_Tempname(char* format, sint32 length);

            h_web_native Web_CreateNative(chars url, bool clearcookies, Platform::Web::EventCB cb, payload data);
            void Web_ReleaseNative(h_web_native web_native);
            void Web_ReloadNative(h_web_native web_native, chars url);
        }
    }
}
