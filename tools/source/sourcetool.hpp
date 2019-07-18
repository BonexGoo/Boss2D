#pragma once
#include <service/boss_zay.hpp>
#include <service/boss_batchbuilder.hpp>

class sourcetoolData : public ZayObject
{
    BOSS_DECLARE_NONCOPYABLE_CLASS(sourcetoolData)
public:
    sourcetoolData();
    ~sourcetoolData();

public:
    void Run();
    void CollectHeader();
    void CollectSource();

public:
    String OptionFile;
    bool OptionFileEnabled;
    String TargetFolder;
    bool TargetFolderEnabled;
    String CollectHeaderFolder;
    bool CollectHeaderFolderEnabled;
    String CollectHeaderResult;
    String CollectSourceFolder;
    bool CollectSourceFolderEnabled;
    String CollectSourceResult;
};
