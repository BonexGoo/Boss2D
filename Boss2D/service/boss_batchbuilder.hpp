#pragma once
#include <boss.hpp>

namespace BOSS
{
    /// @brief 빌더-배치
    class BatchBuilder
    {
    public:
        BatchBuilder(chars dirname);
        ~BatchBuilder();
        BatchBuilder& operator=(const BatchBuilder&)
        {
            BOSS_ASSERT("해당 객체는 복사할 수 없습니다", false);
            return *this;
        }

    public:
        void Processing(chars filename);
        static String MakeMacro(chars filename);

    protected:
        static void Process_SearchCB(chars itemname, payload data);
        void Process_Replace(const String& command, sint32 pos, sint32 posend, chars prm);
        void Process_IncludeAlias(const String& command, sint32 pos, sint32 posend, chars prm);
        String Build_Replace(String source, const Strings& dsts, const Strings& srcs, chars comment);
        String Build_IncludeAlias(String source, const Strings& dsts, const Strings& srcs, chars comment);

    private:
        String ReadFile(chars filename);

    private:
        const String mDirName;
        Strings ReplaceDsts;
        Strings ReplaceSrcs;
        Strings AliasDsts;
        Strings AliasSrcs;
        String ReplaceComment;
        String AliasComment;
    };
}
