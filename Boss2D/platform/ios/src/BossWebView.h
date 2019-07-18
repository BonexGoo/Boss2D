class BossWebView
{
public: typedef void (*EventCB)(void*, const char*, const char*);
public: static int Create(void* uiview, const char* url, bool clearcache, EventCB cb, void* data);
public: static void Release(int id);
};
