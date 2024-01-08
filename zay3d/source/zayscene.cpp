#include <boss.hpp>
#include "zayscene.hpp"

ZayScene::ZayScene()
{
}

ZayScene::~ZayScene()
{
    Free();
}

void ZayScene::Load(chars filename)
{
    Free();
    const String DirName = String::FromWChars(Platform::File::GetDirName(WString::FromChars(filename)));
    const Context Json(ST_Json, SO_NeedCopy, String::FromFile(filename));
    // 오브젝트
    for(sint32 i = 0, iend = Json("object").LengthOfNamable(); i < iend; ++i)
    {
        chararray ObjectID;
        hook(Json("object")(i, &ObjectID))
        {
            const String FileName = fish("file").GetText();
            const String FilePath = DirName + '/' + FileName;
            if(id_abc NewAbcID = AddOn::Abc::Create(FilePath))
            {
                Strings Children;
                for(sint32 j = 0, jend = fish("child").LengthOfNamable(); j < jend; ++j)
                {
                    bool HasHidden = false;
                    chararray ChildID;
                    hook(fish("child")(j, &ChildID))
                    for(sint32 k = 0, kend = fish.LengthOfIndexable(); k < kend; ++k)
                    {
                        const String Command = fish[k].GetText();
                        if(!Command.Compare("color") && k + 1 < kend)
                        {
                            const Color OneColor(fish[k + 1].GetText());
                            AddOn::Abc::SetColor(NewAbcID, &ChildID[0],
                                OneColor.r / 255.0, OneColor.g / 255.0, OneColor.b / 255.0);
                        }
                        else if(!Command.Compare("hidden"))
                        {
                            AddOn::Abc::SetHidden(NewAbcID, &ChildID[0], true);
                            HasHidden = true;
                        }
                        else if(!Command.Compare("matrix") && k + 1 < kend)
                        {
                            const String OneMatrix(fish[k + 1].GetText());
                            SetMatrix(NewAbcID, &ChildID[0], OneMatrix);
                        }
                    }
                    if(!HasHidden)
                        Children.AtAdding() = &ChildID[0];
                }
                hook(mObjects(&ObjectID[0]))
                {
                    fish.mAbcID = NewAbcID;
                    fish.mFileName = FileName;
                    fish.mChildren = Children;
                }
            }
        }
    }
    // 스크립트
    mScript = Json("script").SaveJson();
}

void ZayScene::Free()
{
    mObjects.Reset();
    mScript.Empty();
}

void ZayScene::Render(ZayPanel& panel)
{
    for(sint32 i = 0, iend = mObjects.Count(); i < iend; ++i)
        if(auto CurObject = mObjects.AccessByOrder(i))
            AddOn::Abc::Render(CurObject->mAbcID, 0, 0, panel.w(), panel.h());
}

void ZayScene::SetObjectStatus(chars status, chars objectid, chars child)
{
    if(auto CurObject = mObjects.Access(objectid))
    {
        if(!String::Compare(status, "show"))
            AddOn::Abc::SetHidden(CurObject->mAbcID, child, false);
        else if(!String::Compare(status, "hide"))
            AddOn::Abc::SetHidden(CurObject->mAbcID, child, true);
    }
}

void ZayScene::SetObjectMatrix(chars matrix, chars objectid, chars child)
{
    if(auto CurObject = mObjects.Access(objectid))
        SetMatrix(CurObject->mAbcID, child, matrix);
}

const String ZayScene::GetObjectJson() const
{
    Context Objects;
    for(sint32 i = 0, iend = mObjects.Count(); i < iend; ++i)
    {
        chararray ObjectID;
        if(auto CurObject = mObjects.AccessByOrder(i, &ObjectID))
        hook(Objects.AtAdding())
        {
            fish.At("id").Set(&ObjectID[0]);
            fish.At("file").Set(CurObject->mFileName);
            for(sint32 j = 0, jend = CurObject->mChildren.Count(); j < jend; ++j)
                fish.At("children").AtAdding().Set(CurObject->mChildren[j]);
        }
    }
    return Objects.SaveJson();
}

void ZayScene::SetMatrix(id_abc abcid, chars child, chars matrix)
{
    if(!String::Compare(matrix, "M_identity"))
    {
        double Matrix[16] {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
        AddOn::Abc::SetMatrix(abcid, child, Matrix);
    }
    else if(matrix[0] == 'M' && matrix[1] == '_')
    {
        const Strings Values = String::Split(&matrix[2], '_');
        double Matrix[16] {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
        if(Values.Count() == 16)
        for(sint32 i = 0; i < 16; ++i)
            Matrix[i] = Parser::GetFloat<double>(Values[i]);
        AddOn::Abc::SetMatrix(abcid, child, Matrix);
    }
}
