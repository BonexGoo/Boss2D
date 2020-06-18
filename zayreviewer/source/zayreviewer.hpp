#pragma once
#include <service/boss_zay.hpp>
#include <source.hpp>

class zayreviewerData : public ZayObject
{
public:
    zayreviewerData();
    ~zayreviewerData();

public:
    void ProjectLoad(chars dirpath);
    void RenderSourceView(ZayPanel& panel);
    void RenderAPIView(ZayPanel& panel);
    void RenderChecklistView(ZayPanel& panel);

public:
    void CollectSourceFile(chars dirpath);
    Image& GetUserPic(chars author);
    String& GetUserName(chars author);

public:
    static const sint32 LineSize = 20;
    static const sint32 ApiBoxSize = 33;
    static const sint32 ApiBoxTitleSize = 20;
    static const sint32 CheckBoxSize = 80;
    static const sint32 CheckBoxTitleSize = 25;
    static const sint32 CheckBoxDirInfoSize = 13;
    static const sint32 CheckBoxPicSize = 25;

    id_freetype mFonts[3];
    Map<Image> mUserPics;
    Map<String> mUserNames;

    // 소스코드
    SourceFile* mFileFocus;
    SourcePool mPool;
    // API리스트
    ApilistElements mApilist;
    float mApilistScrollPos;
    float mApilistScrollTarget;
    // 체크리스트
    ChecklistElements mChecklist;
    float mChecklistScrollPos;
    float mChecklistScrollTarget;

    sint32 mLoadCount;
    sint32 mLoadPos;
    id_tasking mLoadTask;

    Strings mRecentProjectPathes;
    String mCurProjectPath;
};
