#include <boss.hpp>
#include "boss_batchbuilder.hpp"

#include <platform/boss_platform.hpp>

namespace BOSS
{
    BatchBuilder::BatchBuilder(chars dirname) : mDirName(dirname)
    {
    }

    BatchBuilder::~BatchBuilder()
    {
    }

    void BatchBuilder::Processing(chars filename)
    {
        ReplaceDsts.Clear();
        ReplaceSrcs.Clear();
        AliasDsts.Clear();
        AliasSrcs.Clear();
        ReplaceComment = "";
        AliasComment = "";

        String Command = ReadFile(filename);
        const String FindKey = "_BOSS_BATCH_COMMAND_";
        sint32 FindedPos = Command.Find(0, FindKey);
        while(0 <= FindedPos)
        {
            sint32 NextFindedPos = Command.Find(FindedPos + 1, FindKey);
            Context JsonCommand(ST_Json, SO_OnlyReference, ((chars) Command) + FindedPos + FindKey.Length());
            if(!String::Compare(JsonCommand("type").GetText(), "replace"))
            {
                Process_Replace(Command, FindedPos, NextFindedPos, JsonCommand("prm").GetText());
                ReplaceComment = JsonCommand("restore-comment").GetText();
            }
            else if(!String::Compare(JsonCommand("type").GetText(), "include-alias"))
            {
                Process_IncludeAlias(Command, FindedPos, NextFindedPos, JsonCommand("prm").GetText());
                AliasComment = JsonCommand("restore-comment").GetText();
            }
            FindedPos = NextFindedPos;
        }

        // 폴더순회
        Platform::File::Search(mDirName, Process_SearchCB, this, true);
    }

    String BatchBuilder::MakeMacro(chars filename)
    {
        String Result = filename;
        if(!Result.Right(2).CompareNoCase(".h"))
            Result = Result.Left(Result.Length() - 2) + "_h";
        else if(!Result.Right(4).CompareNoCase(".hpp"))
            Result = Result.Left(Result.Length() - 4) + "_hpp";
        else if(!Result.Right(4).CompareNoCase(".inc"))
            Result = Result.Left(Result.Length() - 4) + "_inc";
        Result.Replace("/", "__");
        Result.Replace("\\", "__");
        Result.Replace("-", "_M_");
        Result.Replace(".", "_D_");
        return Result;
    }

    void BatchBuilder::Process_SearchCB(chars itemname, payload data)
    {
        if(Platform::File::ExistForDir(itemname))
            Platform::File::Search(itemname, Process_SearchCB, data, true);
        else
        {
            const String Filename = itemname;
            if(!String::CompareNoCase(".c", Filename.Right(2))
                || !String::CompareNoCase(".cc", Filename.Right(3))
                || !String::CompareNoCase(".cpp", Filename.Right(4))
                || !String::CompareNoCase(".h", Filename.Right(2))
                || !String::CompareNoCase(".hpp", Filename.Right(4)))
            {
                BatchBuilder* This = (BatchBuilder*) data;
                bool IsSourceCodeUpdated = false;
                String SourceCode = This->ReadFile(itemname);

                if(0 < This->ReplaceDsts.Count())
                {
                    const String ResultCode = This->Build_Replace(SourceCode, This->ReplaceDsts, This->ReplaceSrcs, This->ReplaceComment);
                    if(0 < ResultCode.Length())
                    {
                        IsSourceCodeUpdated = true;
                        SourceCode = ResultCode;
                    }
                }
                if(0 < This->AliasDsts.Count())
                {
                    const String ResultCode = This->Build_IncludeAlias(SourceCode, This->AliasDsts, This->AliasSrcs, This->AliasComment);
                    if(0 < ResultCode.Length())
                    {
                        IsSourceCodeUpdated = true;
                        SourceCode = ResultCode;
                    }
                }

                // 결과가 존재하면 파일로 구성
                if(IsSourceCodeUpdated)
                {
                    String NewSourcePath = This->mDirName + "_batched" + &itemname[This->mDirName.Length()];
                    id_file NewSourceFileID = Platform::File::OpenForWrite(NewSourcePath, true);
                    Platform::File::Write(NewSourceFileID, (bytes)(chars) SourceCode, SourceCode.Length());
                    Platform::File::Close(NewSourceFileID);
                }
            }
        }
    }

    void BatchBuilder::Process_Replace(const String& command, sint32 pos, sint32 posend, chars prm)
    {
        // 기호수집
        String FindPrm = String("#define ") + prm;
        while(-1 < (pos = command.Find(pos, FindPrm)))
        {
            if(posend != -1 && posend < pos)
                break;
            chars FindFocus = &((chars) command)[pos] + FindPrm.Length();
            String& NewDst = ReplaceDsts.AtAdding();
            String& NewSrc = ReplaceSrcs.AtAdding();
            NewSrc += prm;
            // 패스기호 수집
            while(*FindFocus != ' ' && *FindFocus != '\t')
            {
                NewDst += *FindFocus;
                NewSrc += *FindFocus;
                FindFocus++;
            }
            pos += FindFocus - &((chars) command)[pos];
        }
    }

    void BatchBuilder::Process_IncludeAlias(const String& command, sint32 pos, sint32 posend, chars prm)
    {
        // 기호수집
        String FindPrm = String("#define ") + prm;
        while(-1 < (pos = command.Find(pos, FindPrm)))
        {
            if(posend != -1 && posend < pos)
                break;
            chars FindFocus = &((chars) command)[pos] + FindPrm.Length();
            String& NewDst = AliasDsts.AtAdding();
            String& NewSrc = AliasSrcs.AtAdding();
            NewSrc += prm;
            // <>/""기호 수집
            if(FindFocus[0] == 'V' && FindFocus[1] == '_')
            {
                NewDst += '<';
                NewSrc += "V_";
                FindFocus += 2;
            }
            else if(FindFocus[0] == 'U' && FindFocus[1] == '_')
            {
                NewDst += '\"';
                NewSrc += "U_";
                FindFocus += 2;
            }
            // 패스기호 수집
            while(*FindFocus != ' ' && *FindFocus != '\t')
            {
                // 치환기호
                if(FindFocus[0] == '_')
                {
                    if(FindFocus[1] == '_')
                    {
                        NewDst += '/';
                        NewSrc += "__";
                        FindFocus += 2;
                        continue;
                    }
                    else if(FindFocus[1] == 'h' && (FindFocus[2] == ' ' || FindFocus[2] == '\t'))
                    {
                        NewDst += ".h";
                        NewSrc += "_h";
                        FindFocus += 2;
                        continue;
                    }
                    else if(FindFocus[1] == 'h' && FindFocus[2] == 'p' && FindFocus[3] == 'p' && (FindFocus[4] == ' ' || FindFocus[4] == '\t'))
                    {
                        NewDst += ".hpp";
                        NewSrc += "_hpp";
                        FindFocus += 4;
                        continue;
                    }
                    else if(FindFocus[1] == 'i' && FindFocus[2] == 'n' && FindFocus[3] == 'c' && (FindFocus[4] == ' ' || FindFocus[4] == '\t'))
                    {
                        NewDst += ".inc";
                        NewSrc += "_inc";
                        FindFocus += 4;
                        continue;
                    }
                    else if(FindFocus[1] == 'M' && FindFocus[2] == '_')
                    {
                        NewDst += "-";
                        NewSrc += "_M_";
                        FindFocus += 3;
                        continue;
                    }
                    else if(FindFocus[1] == 'D' && FindFocus[2] == '_')
                    {
                        NewDst += ".";
                        NewSrc += "_D_";
                        FindFocus += 3;
                        continue;
                    }
                }
                // 소문자화
                if('A' <= *FindFocus && *FindFocus <= 'Z')
                    NewDst += *FindFocus - 'A' + 'a';
                else NewDst += *FindFocus;
                NewSrc += *FindFocus;
                FindFocus++;
            }
            pos += FindFocus - &((chars) command)[pos];
        }
    }

    String BatchBuilder::Build_Replace(String source, const Strings& dsts, const Strings& srcs, chars comment)
    {
        String ResultCode;
        // 비교대상을 변경목록에서 찾기(역순탐색)
        for(sint32 i = dsts.Count() - 1; 0 <= i; --i)
        {
            String Result;
            sint32 FindFocus = 0, CopyFocus = 0;
            chars SourcePtr = (chars) source;
            while(-1 < (FindFocus = source.Find(FindFocus, dsts[i])))
            {
                if(!String::Compare(srcs[i], &SourcePtr[FindFocus + dsts[i].Length() - srcs[i].Length()], srcs[i].Length()))
                {
                    sint32 NewLineFocus = source.Find(FindFocus, "\n");
                    if(-1 < NewLineFocus)
                        FindFocus = NewLineFocus + 1;
                    else FindFocus += dsts[i].Length();
                }
                else
                {
                    const char Front = (FindFocus == 0)? '\0' : SourcePtr[FindFocus - 1];
                    const char Rear = SourcePtr[FindFocus + dsts[i].Length()];
                    if(!('a' <= Front && Front <= 'z') && !('a' <= Rear && Rear <= 'z')
                    && !('A' <= Front && Front <= 'Z') && !('A' <= Rear && Rear <= 'Z')
                    && !('0' <= Front && Front <= '9') && !('0' <= Rear && Rear <= '9')
                    && Front != '_' && Rear != '_')
                    {
                        // 지연시킨 복사수행
                        Result.AddTail(&SourcePtr[CopyFocus], FindFocus - CopyFocus);
                        CopyFocus = (FindFocus += dsts[i].Length());
                        // 변경목록 수정안을 복사
                        Result += srcs[i];
                        // 원본(비교대상)을 주석화
                        if(comment && *comment)
                        {
                            sint32 NewLineFocus = source.Find(FindFocus, "\n");
                            if(-1 < NewLineFocus)
                            {
                                if(SourcePtr[NewLineFocus - 1] == '\r') NewLineFocus--;
                                Result.AddTail(&SourcePtr[CopyFocus], NewLineFocus - CopyFocus);
                                Result += comment;
                                Result.AddTail(&SourcePtr[CopyFocus - dsts[i].Length()], NewLineFocus - (CopyFocus - dsts[i].Length()));
                                CopyFocus = FindFocus = NewLineFocus;
                            }
                        }
                    }
                    else FindFocus += dsts[i].Length();
                }
            }
            if(0 < Result.Length())
            {
                Result.AddTail(&SourcePtr[CopyFocus], source.Length() - CopyFocus);
                ResultCode = Result;
                source = Result;
            }
        }
        return ResultCode;
    }

    String BatchBuilder::Build_IncludeAlias(String source, const Strings& dsts, const Strings& srcs, chars comment)
    {
        String ResultCode;
        sint32 FindFocus = 0, CopyFocus = 0;
        String FindKey = "include";
        chars SourcePtr = (chars) source;
        while(-1 < (FindFocus = source.Find(FindFocus, FindKey)))
        {
            // #여부 확인
            bool HasSharp = false;
            for(sint32 i = FindFocus - 1; 0 <= i; --i)
            {
                if(SourcePtr[i] == '#')
                {
                    HasSharp = true;
                    break;
                }
                else if(SourcePtr[i] != ' ' && SourcePtr[i] != '\t')
                    break;
            }

            // 검색키/공백 스킵
            FindFocus += FindKey.Length();
            while(SourcePtr[FindFocus] == ' ' || SourcePtr[FindFocus] == '\t')
                FindFocus++;

            if(HasSharp)
            {
                // 매칭분석으로 비교대상 도출
                sint32 FindEnd = FindFocus + 1;
                if(SourcePtr[FindFocus] == '\"')
                    while(SourcePtr[FindEnd] && SourcePtr[FindEnd] != '\n' && SourcePtr[FindEnd] != '\"') FindEnd++;
                else if(SourcePtr[FindFocus] == '<')
                    while(SourcePtr[FindEnd] && SourcePtr[FindEnd] != '\n' && SourcePtr[FindEnd] != '>') FindEnd++;
                else continue;
                // 매칭실패
                if(!SourcePtr[FindEnd] || SourcePtr[FindEnd] == '\n')
                    continue;

                // 비교대상의 정규화
                String IncludePath = String(&SourcePtr[FindFocus], FindEnd - FindFocus);
                char* IncludePathFocus = (char*)(chars) IncludePath;
                while(*IncludePathFocus)
                {
                    // 특수문자(UTF-8/CP949)가 아닐 경우
                    if(~*IncludePathFocus & 0x80)
                    {
                        // 소문자화
                        if('A' <= *IncludePathFocus && *IncludePathFocus <= 'Z')
                            *IncludePathFocus += 'a' - 'A';
                        // 슬래시전환
                        else if(*IncludePathFocus == '\\')
                            *IncludePathFocus = '/';
                    }
                    IncludePathFocus++;
                }

                // 비교대상을 변경목록에서 찾기(역순탐색)
                for(sint32 i = dsts.Count() - 1; 0 <= i; --i)
                {
                    if(!dsts[i].Compare(IncludePath))
                    {
                        // 지연시킨 복사수행
                        ResultCode.AddTail(&SourcePtr[CopyFocus], FindFocus - CopyFocus);
                        CopyFocus = FindEnd + 1;
                        // 변경목록 수정안을 복사
                        ResultCode += srcs[i];
                        // 원본(비교대상)을 주석화
                        if(comment && *comment)
                        {
                            ResultCode += comment;
                            ResultCode.AddTail(&SourcePtr[FindFocus], FindEnd - FindFocus + 1);
                        }
                        break;
                    }
                }
            }
        }
        if(0 < ResultCode.Length())
            ResultCode.AddTail(&SourcePtr[CopyFocus], source.Length() - CopyFocus);
        return ResultCode;
    }

    String BatchBuilder::ReadFile(chars filename)
    {
        id_file_read OptionFileID = Platform::File::OpenForRead(filename);
        const sint32 OptionFileSize = Platform::File::Size(OptionFileID);
        chararray CommandWords;
        CommandWords.AtWherever(OptionFileSize) = '\0';
        Platform::File::Read(OptionFileID, (uint08*) CommandWords.AtDumping(0, OptionFileSize + 1), OptionFileSize);
        Platform::File::Close(OptionFileID);
        return String(CommandWords);
    }
}
