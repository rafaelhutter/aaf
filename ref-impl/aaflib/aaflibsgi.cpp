/***********************************************************************
 *
 *              Copyright (c) 1996 Avid Technology, Inc.
 *
 * Permission to use, copy and modify this software and to distribute
 * and sublicense application software incorporating this software for
 * any purpose is hereby granted, provided that (i) the above
 * copyright notice and this permission notice appear in all copies of
 * the software and related documentation, and (ii) the name Avid
 * Technology, Inc. may not be used in any advertising or publicity
 * relating to the software without the specific, prior written
 * permission of Avid Technology, Inc.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL AVID TECHNOLOGY, INC. BE LIABLE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT, CONSEQUENTIAL OR OTHER DAMAGES OF
 * ANY KIND, OR ANY DAMAGES WHATSOEVER ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE, INCLUDING, 
 * WITHOUT  LIMITATION, DAMAGES RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, AND WHETHER OR NOT ADVISED OF THE POSSIBILITY OF
 * DAMAGE, REGARDLESS OF THE THEORY OF LIABILITY.
 *
 ************************************************************************/

// WARNING! WARNING! WARNING!
// This is untested code. It has not even been compiled!
// [trr:1999-04-20]
// At this time the irix port has not been maintained for several months.
// Even then it was using the MainWin environment.
//
// This code is provided as a template to the irix port engineer.
//


//
// Use include guard so that the file can be included in every 
// platform build without causing any errors in the build.
//
#if defined(sgi)

// Declare the public interface that must be implemented.
#include "aafrdli.h"


#include "AAFResult.h"


#include <dlfcn.h>

#include <stdio.h>
#include <stdlib.h>



AAFRDLIRESULT AAFLoadLibrary(const char* name, AAFLibraryHandle* pLibHandle)
{
  if (NULL == name || NULL == pLibHandle)
    return AAFRESULT_NULL_PARAM;

  // Attempt to load the library.
  *pLibHandle = ::dlopen(dllname, RTLD_LAZY);
  if (NULL == *pLibHandle)
  {
    fprintf(stderr, "dlopen(\"%s\", RTLD_LAZY) failed.\n", dllname);
    return -1; // Need an AAFRESULT
  }
	
	return AAFRESULT_SUCCESS;
}

AAFRDLIRESULT AAFUnloadLibrary(AAFLibraryHandle libHandle)
{
  if (NULL == pLibHandle)
    return AAFRESULT_NULL_PARAM;

	::dlclose(libHandle)
	
	return AAFRESULT_SUCCESS;
}

AAFRDLIRESULT AAFFindSymbol(AAFLibraryHandle libHandle, const char* symbolName, AAFSymbolAddr* pSymbol)
{
  if (NULL == libHandle || NULL == symbolName || NULL == pSymbol)
    return AAFRESULT_NULL_PARAM;


	*pSymbol = ::dlsym(libHandle, symbolName);
  if (NULL == *pSymbol)
  {
    fprintf(stderr, "dlsym(\"%s\") failed.\n", fcnName[4]);
    return -2; // Need an AAFRESULT
  }

	return AAFRESULT_SUCCESS;
}



/* Limited set of platform independent directory searching functions.*/
AAFRDLIRESULT AAFFindLibrary(const char* name, LPFNAAFTESTFILEPROC testProc, void *userData)
{
	// Default implementation will just continue to use a hard-coded list of shared libaries.

	const char *pluginFileNames[] = 
	{
		"aafpgapi.so",
		"aafintp.so",
		NULL
	};
	AAFRDLIRESULT rc = AAFRESULT_SUCCESS;

	if (NULL == name || NULL == testProc)
		return AAFRESULT_NULL_PARAM;

	for (int i = 0; AAFRESULT_SUCCESS == rc && pluginFileNames[i]; ++i)
		rc = testProc(pluginFileNames[i], false /* not a directory */, userData);

	return rc;
}


#endif /* #if defined(sgi) */
