/*********************************************************************
 Copyright 2016, UCAR/Unidata
 See netcdf/COPYRIGHT file for copying and redistribution conditions.
 Derived from poco library from Boost Software: see cpoco/LICENSE file.
*********************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif
#ifdef USE_MUTEX
#include <pthread.h>
#endif

#include "cpoco.h"

// Note: cygwin is missing RTLD_LOCAL, set it to 0
#if defined(__CYGWIN__) && !defined(RTLD_LOCAL)
#define RTLD_LOCAL 0
#endif

#ifdef USE_MUTEX
static pthread_mutex_t mutex;
#endif

int
cp_unix_initialize()
{
    int ret = 1;
#ifdef USE_MUTEX
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    if(pthread_mutex_init(&mutex, &attr)) {
	fprintf(stderr,"cp_unix: Cannot create mutext\n");
	ret = 1;
    }
    pthread_mutexattr_destroy(&attr);
#endif
    return ret;
}

int
cp_unix_finalize()
{
#ifdef USE_MUTEX
    pthread_mutex_destroy(&mutex);
#endif
    return 1;
}

/**************************************************/

#ifdef USE_MUTEX
static void lock(void) {pthread_mutex_lock(&mutex);}
#else
#define lock()
#endif

#ifdef USE_MUTEX
static void unlock(void) {pthread_mutex_unlock(&mutex);}
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
load(SharedLib* lib , const char* path, int flags)
{
    cperr ret = CP_OK;
    int realflags = RTLD_LAZY;

    lock();
    if(lib->state.handle != NULL)
	return CP_ELOADED;
    lib->path = nulldup(path);
    lib->flags = flags;
    if(flags & CP_LOCAL)
	realflags |= RTLD_LOCAL;
    else
	realflags |= RTLD_GLOBAL;
    lib->state.flags = realflags;
    lib->state.handle = dlopen(lib->path, lib->state.flags);
    if(lib->state.handle == NULL) {
	lib->err.msg = nulldup(dlerror());
	ret = CP_ELOAD;
	goto done;
    }
done:
    unlock();
    return ret;
}

static cperr
unload(SharedLib* lib)
{
    cperr ret = CP_OK;
    lock();
    if(lib->state.handle != NULL) {
	dlclose(lib->state.handle);
	lib->state.handle = NULL;
    }
    unlock();
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
    lock();
    if(lib->state.handle != NULL)
	result = dlsym(lib->state.handle, name);
    unlock();
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


