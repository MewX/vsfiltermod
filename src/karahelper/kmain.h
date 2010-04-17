#include "lua.h"
#include "lauxlib.h"
#include "png.h"
#include "kcsri.h"
#include "kAssParser.h"
#include "kRenderer.h"
#include "windowsx.h" // selectfont

#define EXTERNC extern "C" 

#ifdef KMAIN_EXPORTS
#define KMAIN_API EXTERNC __declspec(dllexport)
#else
#define KMAIN_API __declspec(dllimport)
#endif

// init
KMAIN_API int KHlibinit(lua_State* L);