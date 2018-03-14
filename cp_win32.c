/*********************************************************************
 Copyright 2016, UCAR/Unidata
 See netcdf/COPYRIGHT file for copying and redistribution conditions.
 Derived from poco library from Boost Software: see cpoco/LICENSE file.
*********************************************************************/

#include "config.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpoco.h"

#ifdef USE_MUTEX
static CRITICAL_SECTION mutex;
#endif

int
cp_win32_initialize()
{
    int ret = 1;
#ifdef USE_MUTEX
    InitializeCriticalSectionAndSpinCount(&mutex, 4000);
#endif
    return ret;
}

int
cp_win32_finalize()
{
#ifdef USE_MUTEX
    DeleteCriticalSection(&mutex);
#endif
    return 1;
}

/**************************************************/


#ifdef USE_MUTEX
static void lock(void) {EnterCriticalSection(&mutex);}
#else
#define lock()
#endif

#ifdef USE_MUTEX
static void unlock(void) {LeaveCriticalSection(&mutex);}
#else
#define unlock()
#endif

/**************************************************/

static cperr
init(SharedLib* lib)
{
    cperr ret = CP_OK;
    return ret;
}

static cperr
reclaim(SharedLib* lib)
{
    cperr ret = CP_OK;
    return ret;
}

static cperr
load(SharedLib* lib , const char* path0, int flags)
{
    cperr ret = CP_OK;
    DWORD realflags = 0;
    Path path = path0;

    lock(mutex);
    if(lib->handle != NULL)
	return CP_ELOADED;
    lib->path = nulldup(path);
    lib->flags = flags;
    if(path.isAbsolute()) realflags |= LOAD_WITH_ALTERED_SEARCH_PATH;
    lib->state.flags = realflags;    
    lib->state.handle = LoadLibraryExA(path, 0, realflags);
    if(_handle == NULL) {
	ret = CP_ELOAD;
	goto done;
    }

done:
    unlock(mutex);
    return ret;
}

static cperr
unload(SharedLib* lib)
{
    cperr ret = CP_OK;
    lock(mutex);
    if(lib->state.handle != NULL) {
	FreeLibrary((HMODULE)lib->state.handle);
	lib->state.handle = NULL;
    }
done:
    unlock(mutex);
    return ret;
}

static int
isLoaded(SharedLib* lib)
{
    return lib->state.handle != NULL;
}

static void*
getsymbol(SharedLib* lib, const char* name)
{
    void* result = NULL;
    lock(mutex);
    if(lib->state.handle != NULL) {
	result = (void*)getProcAddress((HMODULE)lib->state.handle,name);
    }
    unlock(mutex);
    return result;
}

static const char*
getpath(SharedLib* lib)
{
    return lib->path;
}

struct API cp_host_api = {
	init,
	reclaim,
	load,
	unload,
	isLoaded,
	getsymbol,
	getpath,
};

