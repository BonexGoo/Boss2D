#include <boss.hpp>
#include "source.hpp"

SourceLine::SourceLine()
{
    mCommentType = CommentType::Null;
    mAreaLevel = 0;
    mRefID = -1;
}

SourceLine::~SourceLine()
{
}

SourceFile::SourceFile()
{
    mType = Type::Source;
    mScrollPos = 0;
    mScrollTarget = 0;
}

SourceFile::~SourceFile()
{
}

ChecklistElement::ChecklistElement()
{
    mCodeLine = 0;
    mChecked = false;
}

ChecklistElement::~ChecklistElement()
{
}

ApilistElement::ApilistElement()
{
    mCodeLine = 0;
    mBriefType = BriefType::Null;
    mLevel = 0;
}

ApilistElement::~ApilistElement()
{
}
