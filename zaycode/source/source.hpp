#pragma once
#include <boss.hpp>

class SourceLine
{
public:
    SourceLine();
    ~SourceLine();

public:
    enum class CommentType {Null, Api, Check};
public:
    String mText;
    CommentType mCommentType;
    String mComment;
    sint32 mAreaLevel;
    sint32 mRefID;
};
typedef Array<SourceLine> SourceLines;

class SourceFile
{
public:
    enum class Type {Source, Project};
public:
    SourceFile();
    ~SourceFile();

public:
    Type mType;
    String mShortName;
    SourceLines mLines;
    float mScrollPos;
    float mScrollTarget;
};
typedef Map<SourceFile> SourcePool;

class ChecklistElement
{
public:
    ChecklistElement();
    ~ChecklistElement();

public:
    String mFilePath;
    String mShortName;
    sint32 mCodeLine;
    String mAuthor;
    String mDate;
    String mAction;
    String mKorText;
    String mEngText;
    bool mChecked;
};
typedef Array<ChecklistElement> ChecklistElements;

class ApilistElement
{
public:
    ApilistElement();
    ~ApilistElement();

public:
    enum class BriefType {Null, Class, Method};
public:
    String mFilePath;
    String mShortName;
    sint32 mCodeLine;
    String mTitle;
    BriefType mBriefType;
    String mBrief;
    sint32 mLevel;
};
typedef Array<ApilistElement> ApilistElements;
