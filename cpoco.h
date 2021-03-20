/*********************************************************************
 Copyright 2016, UCAR/Unidata
 See netcdf/COPYRIGHT file for copying and redistribution conditions.
 Derived from poco library from Boost Software: see cpoco/LICENSE file.
*********************************************************************/

#ifndef CPOCO_H
#define CPOCO_H

/* Use CP or cp as our "namespace */

/* Forward */
typedef struct SharedLib SharedLib;
typedef enum CP_Flags CP_Flags;

enum CP_Flags {
    CP_GLOBAL = 1, /* (default) use RTLD_GLOBAL; ignored on platforms that do not use dlopen(). */
    CP_LOCAL  = 2  /* use RTLD_LOCAL; RTTI will not work with this flag. */
};

/* Error codes */

typedef enum cperr {
CP_OK=0,
CP_ENOMEM=(-1),  /* malloc failed */
CP_EINVAL=(-2),  /* invalid argument e.g. null path */
CP_ELOAD=(-3),   /* library load failed */
CP_ELOADED=(-4), /* library already loaded */
CP_ENOLIB=(-5),  /* no such library */
} cperr;

struct SharedLib {
    char* path;
    int flags;
    struct CPstate {
        void* handle;
        int flags;
    } state;
    struct {
	char* msg;
    } err;
    struct API {
	cperr (*init)(SharedLib*);
	cperr (*reclaim)(SharedLib*); /* reclaim resources; do not free lib */
	cperr (*load)(SharedLib*,const char*,int);
	cperr (*unload)(SharedLib*);
	int (*isloaded)(SharedLib*);
	void* (*getsymbol)(SharedLib*,const char*);
	const char* (*getpath)(SharedLib*);
    } api;
};


/**************************************************/
#ifdef _MSC_VER
#  ifdef DLL_EXPORT /* define when building the library */
#   define EXTERNL __declspec(dllexport) extern
#  else
#   define EXTERNL __declspec(dllimport) extern
#  endif
#else
#  ifdef DLL_EXPORT
#    define EXTERNL
#  else
#    define EXTERNL extern
#  endif
#endif

/**************************************************/

EXTERNL struct API cp_host_api;

/**************************************************/
/* SharedLib API */

EXTERNL cperr cpsharedlibnew(SharedLib**); /* Creates a SharedLib object. */

EXTERNL cperr cpsharedlibfree(SharedLib*); /* free this shared library */

/*
Loads a shared library from the given path using specified flags.
Returns error if a library has already been loaded or cannot be loaded.
*/
EXTERNL cperr cpload(SharedLib*,const char* path, int flags);

EXTERNL cperr cpunload(SharedLib*); /* Unloads a shared library. */

EXTERNL int cpisloaded(SharedLib*); /* Returns 1 iff a library has been loaded.*/

/* Returns the address of the symbol with the given name.
   For functions, this is the entry point of the function.
   Return NULL if the symbol does not exist
*/
EXTERNL void* cpgetsymbol(SharedLib*,const char* name);

/* Returns the path of the library, as specified in
   a call to load(SharedLib*)
*/
EXTERNL const char* cpgetpath(SharedLib*);

EXTERNL const char* cperrstr(int err1);

#define nulldup(s) (s == NULL ? NULL : strdup(s))

#endif /*CPOCO_H*/
