/******************************************\
*                                          *
* Advanced Authoring Format                *
*                                          *
* Copyright (c) 1998 Avid Technology, Inc. *
*                                          *
\******************************************/

/******************************************\
*                                          *
* Advanced Authoring Format                *
/***********************************************************************
 *
 *              Copyright (c) 1998-1999 Avid Technology, Inc.
 *
 * Permission to use, copy and modify this software and accompanying 
 * documentation, and to distribute and sublicense application software
 * incorporating this software for any purpose is hereby granted, 
 * provided that (i) the above copyright notice and this permission
 * notice appear in all copies of the software and related documentation,
 * and (ii) the name Avid Technology, Inc. may not be used in any
 * advertising or publicity relating to the software without the specific,
 *  prior written permission of Avid Technology, Inc.
 *
 * THE SOFTWARE IS PROVIDED AS-IS AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL AVID TECHNOLOGY, INC. BE LIABLE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, PUNITIVE, INDIRECT, ECONOMIC, CONSEQUENTIAL OR
 * OTHER DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE AND
 * ACCOMPANYING DOCUMENTATION, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, AND WHETHER OR NOT
 * ADVISED OF THE POSSIBILITY OF DAMAGE, REGARDLESS OF THE THEORY OF
 * LIABILITY.
 *
 ************************************************************************/
#endif

#include "AAFStoredObjectIDs.h"
#include "AAFPropertyIDs.h"


#ifndef __ImplAAFFiller_h__
#include "ImplAAFFiller.h"
#endif

#include <assert.h>
#include "AAFResult.h"


ImplAAFFiller::ImplAAFFiller ()
{}


ImplAAFFiller::~ImplAAFFiller ()
{}


AAFRESULT STDMETHODCALLTYPE
    ImplAAFFiller::Initialize (aafUID_t*	pDataDef,
                           aafLength_t		length)
{
	if (pDataDef == NULL)
		return AAFRESULT_NULL_PARAM;
	else
		return( SetNewProps( length, pDataDef ) );
}

AAFRESULT ImplAAFFiller::TraverseToClip(aafLength_t length,
										 ImplAAFSegment **sclp,
										 ImplAAFPulldown **pulldownObj,
										 aafInt32 *pulldownPhase,
										 aafLength_t *sclpLen,
										 aafBool *isMask)
{
	return ( AAFRESULT_FILL_FOUND );
}

OMDEFINE_STORABLE(ImplAAFFiller, AUID_AAFFiller);
