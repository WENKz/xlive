#ifndef XLIVELESS_H

#define XLIVELESS_H

#ifdef NDEBUG
NO_TRACE
#endif

#ifdef XLIVELESS_EXPORTS
#define XLIVELESS_API extern "C" __declspec(dllexport)
#else
#define XLIVELESS_API extern "C" __declspec(dllimport)
#endif

#define XLIVELESS_VERSION   0x00020000
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_FREE(p)  { if(p) { free(p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define ARRAYOF(x) (sizeof(x)/sizeof(*x))
#define IN_RANGE(val, min, max) ((val) > (min) && (val) < (max))
#define IN_RANGE2(val, min, max) ((val) >= (min) && (val) <= (max))
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define BYTESOF(a, b) ((a) * sizeof(b))
#define GAMEPACKETHEADERSIZE 17

extern HMODULE hThis;
extern CRITICAL_SECTION d_lock;

#ifndef NO_TRACE

extern void trace(LPWSTR message, ...);
extern void trace2(LPWSTR message, ...);

#define TRACE(msg, ...) trace (L ## msg, __VA_ARGS__)
#define TRACE2(msg, ...) trace2 (L ## msg, __VA_ARGS__)

#else
#define TRACE()
#endif

#endif