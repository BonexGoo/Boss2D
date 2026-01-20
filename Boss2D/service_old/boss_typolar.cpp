#include <boss.hpp>
#include "boss_typolar.hpp"

namespace BOSS
{
    typedef String (*BuildCB)(const String& classname, const Context& table);
    static void BuildCore(chars matchname, chars assetpath, chars outpath, BuildCB cb)
    {
        const String MatchName = matchname;
        const String AssetPath = assetpath;
        const String OutPath = outpath;
        void* Param[4] = {(void*) &MatchName, (void*) &AssetPath, (void*) &OutPath, (void*) cb};
        Platform::File::Search(String::Format("assets:/%s*", assetpath),
            [](chars name, payload data)->void
            {
                const String& MatchName = *(((const String**) data)[0]);
                const String& AssetPath = *(((const String**) data)[1]);
                const String& OutPath = *(((const String**) data)[2]);
                BuildCB Builder = ((BuildCB*) data)[3];
                if(!String::CompareNoCase(name, MatchName, MatchName.Length()))
                {
                    // 시간조사
                    const String AssetName = AssetPath + name;
                    uint64 AssetTime = 0;
                    Asset::Exist(AssetName, nullptr, nullptr, nullptr, nullptr, &AssetTime);

                    // 빌드될 파일명
                    String OutFileName = name + MatchName.Length();
                    sint32 DotPos = OutFileName.Find(0, ".");
                    if(DotPos != -1) OutFileName = OutFileName.Left(DotPos);
                    const String OutClassName = OutFileName;
                    OutFileName += ".hpp";

                    // 필요할 경우 빌드
                    const String OutName = OutPath + OutFileName;
                    uint64 OutTime = 0;
                    if(Platform::File::GetAttributes(WString::FromChars(OutName), nullptr, nullptr, nullptr, &OutTime) == -1 || OutTime < AssetTime)
                    {
                        String AssetJson = String::FromAsset(AssetName);
                        const Context Table(ST_Json, SO_OnlyReference, AssetJson);
                        const String Output = Builder(OutClassName, Table);
                        auto OutFile = Platform::File::OpenForWrite(OutName, true);
                        Platform::File::Write(OutFile, (bytes) "\xef\xbb\xbf", 3); // UTF-8 Bom: EF BB BF
                        Platform::File::Write(OutFile, (bytes)(chars) Output, Output.Length());
                        Platform::File::Close(OutFile);
                    }
                }
            }, Param, false);
    }

    void Typolar::BuildTypes(chars matchname, chars assetpath, chars outpath)
    {
        BuildCore(matchname, assetpath, outpath,
            [](const String& classname, const Context& table)->String
            {
                String Output;
                // 열거자
                Output += "class " + classname + "\r\n";
                Output += "{\r\n";
                Output += "public:\r\n";
                Output += "    enum Type {";
                for(sint32 i = 0, iend = table.LengthOfIndexable(); i < iend; ++i)
                {
                    Output += table[i]("Name").GetText("ERROR");
                    Output += ", ";
                }
                Output += "Max, Null = -1};\r\n";
                Output += "\r\n";
                // 생성자
                Output += "public:\r\n";
                Output += "    " + classname + "() {mName = Null;}\r\n";
                Output += "    " + classname + "(const " + classname + "& rhs) {operator=(rhs);}\r\n";
                Output += "    " + classname + "(chars rhs) {operator=(rhs);}\r\n";
                // 연산자
                Output += "    " + classname + "& operator=(const " + classname + "& rhs) {mName = rhs.mName; return *this;}\r\n";
                Output += "    " + classname + "& operator=(Type rhs) {mName = rhs; return *this;}\r\n";
                Output += "    " + classname + "& operator=(chars rhs)\r\n";
                Output += "    {\r\n";
                Output += "        if(false) {}\r\n";
                for(sint32 i = 0, iend = table.LengthOfIndexable(); i < iend; ++i)
                {
                    Output += "        else if(!String::Compare(rhs, \"";
                    Output += table[i]("Name").GetText("ERROR");
                    Output += "\")) // ";
                    Output += table[i]("Description").GetText("No description available");
                    Output += "\r\n";
                    Output += "            mName = ";
                    Output += table[i]("Name").GetText("ERROR");
                    Output += ";\r\n";
                }
                Output += "        else\r\n";
                Output += "        {\r\n";
                Output += "            mName = Null;\r\n";
                Output += "            if(!String::Compare(rhs, \"Null\"))\r\n";
                Output += "                BOSS_ASSERT(\"No valid keywords\", false);\r\n";
                Output += "            else BOSS_ASSERT(\"Unknown keyword\", false);\r\n";
                Output += "        }\r\n";
                Output += "        return *this;\r\n";
                Output += "    }\r\n";
                // 비교연산자
                Output += "    bool operator==(Type rhs) const\r\n";
                Output += "    {return (mName == rhs);}\r\n";
                Output += "    bool operator!=(Type rhs) const\r\n";
                Output += "    {return (mName != rhs);}\r\n";
                Output += "\r\n";
                // 멤버
                Output += "protected:\r\n";
                Output += "    Type mName;\r\n";
                Output += "};\r\n";
                return Output;
            });
    }
}
