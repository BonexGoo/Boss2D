#include <boss.hpp>

#define BOSS_EMBEDDED_ASSET_COUNT Signed64(0)
#define BOSS_EMBEDDED_ASSET_SIZE  Signed64(0)
#define BOSS_EMBEDDED_ASSET_TIME  Signed64(0)

struct EmbeddedFile {
    String mPath;
    buffer mBuffer;
    bytes  mData;
    uint64 mSize;
    uint64 mCTime;
    uint64 mATime;
    uint64 mMTime;
    ~EmbeddedFile() {Buffer::Free(mBuffer);}
};

static Map<EmbeddedFile> gEmbeddedCaches;
static EmbeddedFile gSortedEmbeddedFiles[] = {
    {"", nullptr, nullptr, 0, 0, 0, 0}
};
