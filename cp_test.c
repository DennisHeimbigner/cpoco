/*
Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
and Contributors.
*/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "cpoco.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* Possible names of the test shared library */
static const char* SHAREDLIBS[] = {
	"./libcpt.so",
	"./cygcpt.dll",
	"./.libs/libcpt.so",
	"./.libs/cygcpt.dll",
	"./.libs/cygcpt-0.dll",
	NULL};

typedef int (*GimmeFiveFunc)();

#define CHECK(err) check(CE_ERR,0,__LINE__,__FILE__,(err))
#define CHECKNO(err) check(CE_ERR,1,__LINE__,__FILE__,(err))
#define CHECKBOOL(b) check(CE_BOOL,0,__LINE__,__FILE__,(b))
#define CHECKNOBOOL(b) check(CE_BOOL,1,__LINE__,__FILE__,(b))
#define CHECKSTR(s1,s2) check(CE_STR,0,__LINE__,__FILE__,(s1),(s2))
#define CHECKSYM(sym) check(CE_SYM,0,__LINE__,__FILE__,(sym))
#define CHECKNOSYM(sym) check(CE_SYM,1,__LINE__,__FILE__,(sym))

enum CE {
    CE_ERR,
    CE_BOOL,
    CE_STR,
    CE_SYM,
};

static void
check(enum CE ce, int invert, int lineno, const char* file, ...)
{
    va_list args;
    va_start (args,file);
    int err;
    char* s1;
    char* s2;
    void* sym;

    switch (ce) {
    case CE_ERR:
        err = va_arg(args,int);
	if((!invert && err != CP_OK) || (invert && err == CP_OK)) {
            fprintf(stderr,"(%s:%d): %s\n",file,lineno,cperrstr(err));
	    goto fail;
	}
	break;
    case CE_BOOL:
        err = va_arg(args,int);
	if((err && invert) || (!err && !invert)) {
            fprintf(stderr,"(%s:%d): boolean fail\n",file,lineno);
	    goto fail;
	}
	break;
    case CE_STR:
        s1 = va_arg(args,char*);
        s2 = va_arg (args,char*);
	if(s1 == NULL) s1 = "null";
	if(s2 == NULL) s2 = "null";
	if((!invert && strcmp(s1,s2) != 0) || (invert && strcmp(s1,s2) == 0)) {
	   fprintf(stderr,"(%s:%d): mismatch: %s::%s \n",file,lineno,s1,s2);
	   goto fail;
	}
	break;
    case CE_SYM:
        sym = va_arg(args,void*);	
	if((!invert && sym == NULL) || (invert && sym != NULL)) {
	   fprintf(stderr,"(%s:%d): get symbol failure\n",file,lineno);
           goto fail;
	}
	break;
    default: abort();
    }
    va_end (args);  // clean up the system stack
    return;
fail:
    exit(1);
}

static void
test1(const char* path)
{
    SharedLib* sl = NULL;
    void* p1;

    CHECK(cpsharedlibnew(&sl));
    CHECK(cpload(sl,path,0));
    CHECKBOOL(cpisloaded(sl));
    CHECKNO(cpload(sl,path,0));
    CHECKSTR(cpgetpath(sl),path);
    CHECKBOOL(cpisloaded(sl));
    CHECKSYM(cpgetsymbol(sl,"gimmeFive"));
    CHECKNOSYM(cpgetsymbol(sl,"fooBar123"));

    p1 = cpgetsymbol(sl,"pocoBuildManifest");
    CHECK((p1 != NULL));

    p1 = cpgetsymbol(sl,"fooBar123");
    cpunload(sl);
    CHECKNOBOOL(cpisloaded(sl));
}

static void
test2(const char* path)
{
    SharedLib* sl = NULL;
    GimmeFiveFunc gimmeFive;

    CHECK(cpsharedlibnew(&sl));
    CHECK(cpload(sl,path,0));
    CHECKSTR(cpgetpath(sl),path);
    CHECKBOOL(cpisloaded(sl));

    gimmeFive = (GimmeFiveFunc) cpgetsymbol(sl,"gimmeFive");
    CHECKSYM(gimmeFive);
    CHECKBOOL((gimmeFive() == 5));
    CHECK(cpunload(sl));
    CHECKNOBOOL(cpisloaded(sl));
}

static void
test3(const char* path)
{
    SharedLib* sl = NULL;

    CHECK(cpsharedlibnew(&sl));
    CHECK(cpload(sl,path,0));
    CHECKSTR(cpgetpath(sl),path);
    CHECKBOOL(cpisloaded(sl));
    CHECKNO(cpload(sl,"NoSuchLibrary",0));
    CHECKBOOL(cpisloaded(sl));
    CHECKNO(cpload(sl,path,0));
    CHECKBOOL(cpisloaded(sl));
    CHECKNO(cpload(sl,path,0));
    CHECKBOOL(cpisloaded(sl));
    CHECK(cpunload(sl));
    CHECKNOBOOL(cpisloaded(sl));
}

static char*
abspath(const char* relpath)
{
#ifdef _MSC_VER
    return _fullpath(NULL,relpath,8192);
#else
    return realpath(relpath, NULL);
#endif
}

static const char*
findlib(void)
{
    char* path;
    const char** p;
    for(p=SHAREDLIBS;*p;p++) {
	FILE* f;
	path = abspath(*p);
	if(path == NULL) continue;
	f = fopen(path,"r");
	if(f != NULL) {
	    fclose(f);
	    return *p;
	}
    }
    return NULL;
}


int
main(int argc, char** argv)
{
    const  char* relpath = NULL;
    const char* path = NULL;

    if(argc > 1) {
	relpath = argv[1];
    } else {
        relpath = findlib();
    }
    if(relpath == NULL) {
	fprintf(stderr,"No dynamic test library specified\n");
	return 1;
    }

    path = abspath(relpath);

    if(path == NULL) {
	fprintf(stderr,"Cannot locate dynamic test library: %s\n",relpath);
	exit(1);
    }

    printf("Using shared test library: %s\n",path);

    test1(path);
    test2(path);
    test3(path);
    return 0;
}
