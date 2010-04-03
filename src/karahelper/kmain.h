// Приведенный ниже блок ifdef - это стандартный метод создания макросов, упрощающий процедуру 
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа KMAIN_EXPORTS,
// заданного в командной строке. Данный символ не должен быть определен ни в одном проекте,
// использующем данную DLL. Благодаря этому любой другой проект, чьи исходные файлы включают данный файл, видит 
// функции KMAIN_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.
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