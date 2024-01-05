#pragma once
#include <service/boss_zay.hpp>

class ZaySceneObject
{
public:
    ZaySceneObject() {}
    ~ZaySceneObject() {AddOn::Abc::Release(mAbcID);}

public:
    id_abc mAbcID {nullptr};
    String mFileName;
    Strings mChildren;
};

class ZayScene
{
public:
    ZayScene();
    ~ZayScene();

public:
    void Load(chars filename);
    void Free();
    void Render(ZayPanel& panel);
    void SetObjectStatus(chars status, chars objectid, chars child);
    void SetObjectMatrix(chars matrix, chars objectid, chars child);

public:
    const String GetObjectJson() const;
    const String& GetScriptJson() const {return mScript;}
    void SetMatrix(id_abc abcid, chars matrix);

public:
    Map<ZaySceneObject> mObjects; // [obj001]
    String mScript;
};
