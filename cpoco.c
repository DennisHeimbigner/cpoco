/*********************************************************************
 Copyright 2016, UCAR/Unidata
 See netcdf/COPYRIGHT file for copying and redistribution conditions.
 Derived from poco library from Boost Software: see cpoco/LICENSE file.
*********************************************************************/

#include "config.h"
#include "stdlib.h"
#include "string.h"
#ifdef HAVE_UNISTD_H
#include "unistd.h"
#endif

#include "cpoco.h"

/**************************************************/

/* Creates a SharedLib object. */
cperr
cpsharedlibnew(SharedLib** libp)
{
    cperr ret = CP_OK;
    SharedLib* lib;
    lib = (SharedLib*)calloc(1,sizeof(SharedLib));
    if(lib == 0)
	{ret = CP_ENOMEM; goto done;}
    /* fill in the api */
    lib->api = cp_host_api;
    ret = lib->api.init(lib);
    if(ret == CP_OK && libp)
	*libp = lib;
	
done:
    return ret;
}

/* free this shared library */
cperr
cpsharedlibfree(SharedLib* lib)
{
    cperr ret = CP_OK;
    if(lib == NULL) return CP_OK;
    ret = lib->api.reclaim(lib);
    free(lib);
    return ret;
}

/*
Loads a shared library from the given path using specified flags.
Returns error if a library has already been loaded or cannot be loaded.
*/
cperr
cpload(SharedLib* lib, const char* path, int flags)
{
    if(lib == NULL || path == NULL) return CP_EINVAL;
    return lib->api.load(lib,path,flags);
}

cperr
cpunload(SharedLib* lib) /* Unloads a shared library. */
{
    if(lib == NULL) return CP_EINVAL;
    return lib->api.unload(lib);
}

int
cpisloaded(SharedLib* lib) /* Returns 1 iff a library has been loaded. */
{
    if(lib == NULL) return 0;
    return lib->api.isloaded(lib);
}

/* Returns the address of the symbol with the given name.
   For functions, this is the entry point of the function.
   Return error if the symbol does not exist
*/
void*
cpgetsymbol(SharedLib* lib,const char* name)
{
    if(lib == NULL) return NULL;
    return lib->api.getsymbol(lib,name);
}

/* Returns the path of the library, as specified in
   a call to load()
*/
const char*
cpgetpath(SharedLib* lib)
{
    if(lib == NULL) return NULL;
    return lib->api.getpath(lib);
}

const char*
cperrstr(int err1)
{
   /* System error? */
   if(err1 > 0) {
      const char *cp = (const char *) strerror(err1);
      if(cp == NULL)
	 return "Unknown Error";
      return cp;
   }

   /* If we're here, this is a cpoco error code. */
   switch(err1)
   {
      case CP_OK:
	 return "No error";
      case CP_ENOMEM:
	 return "cpoco: out of memory";
      case CP_EINVAL:
	 return "cpoco: illegal argument";
      case CP_ELOAD:
	 return "cpoco: load failed";
      case CP_ELOADED:
	 return "cpoco: library already loaded";
      case CP_ENOLIB:
	 return "cpoco: non-existent library";
      default:
	 return "Unknown Error";
   }
}
