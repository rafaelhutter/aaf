//@doc
//@class    AAFContainerDef | Implementation class for AAFContainerDef
#ifndef __ImplAAFContainerDef_h__
#define __ImplAAFContainerDef_h__


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
 * prior written permission of Avid Technology, Inc.
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


#ifndef __ImplAAFDefObject_h__
#include "ImplAAFDefObject.h"
#endif


class ImplAAFContainerDef : public ImplAAFDefObject
{
public:
  //
  // Constructor/destructor
  //
  //********
  ImplAAFContainerDef ();


  //****************
  // Initialize()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    Initialize
        // @parm [in] Pointer to an AUID reference
        (const aafUID_t & id,
		 const aafCharacter *name,
		 const aafCharacter *description);

protected:
  virtual ~ImplAAFContainerDef ();

public:


  //****************
  // EssenceIsIdentified()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    EssenceIsIdentified
        // @parm [out] The EssenceIsIdentified
        (aafBool *  pEssenceIsIdentified);


  //****************
  // SetEssenceIsIdentified()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    SetEssenceIsIdentified
        // @parm [in] The EssenceIsIdentified flag
        (aafBool  EssenceIsIdentified);

private:
	OMFixedSizeProperty<aafBool>					_isIdentified;
};

#endif // ! __ImplAAFContainerDef_h__


