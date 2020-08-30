#include <boss.hpp>

#define BOSS_EMBEDDED_ASSET_COUNT Signed64(0)
#define BOSS_EMBEDDED_ASSET_SIZE  Signed64(0)
#define BOSS_EMBEDDED_ASSET_TIME  Signed64(0)

struct EmbeddedFile {
    chars  mPath;
    bytes  mData;
    uint64 mSize;
    uint64 mCTime;
    uint64 mATime;
    uint64 mMTime;
};

static const sint32 gEmbeddedFileCount = 0;
static const EmbeddedFile gSortedEmbeddedFiles[] = {
    {"", nullptr, 0, 0, 0, 0}
};
