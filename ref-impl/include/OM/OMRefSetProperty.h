/***********************************************************************
*
*              Copyright (c) 1998-2000 Avid Technology, Inc.
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
* THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
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

// @doc OMEXTERNAL
#ifndef OMREFSETPROPERTY_H
#define OMREFSETPROPERTY_H

#include "OMContainerProperty.h"

  // @class Abstract base class for persistent object reference set
  //        properties supported by the Object Manager.
  //   @base public | <c OMProperty>
  //   @cauthor Tim Bingham | tjb | Avid Technology, Inc.
class OMReferenceSetProperty : public OMContainerProperty {
public:
  // @access Public members.

    // @cmember Constructor.
  OMReferenceSetProperty(const OMPropertyId propertyId,
                         const OMStoredForm storedForm,
                         const wchar_t* name);

    // @cmember Destructor.
  virtual ~OMReferenceSetProperty(void);

    // @cmember Remove the <c OMObject> identified by <p identification>
    //          from this <c OMReferenceSetProperty>.
  virtual OMObject* remove(void* identification) = 0;

    // @cmember Does this <c OMReferenceSetProperty> contain an
    //          <c OMObject> identified by <p identification> ?
  virtual bool contains(void* identification) const = 0;

    // @cmember Find the <c OMObject> in this <c OMReferenceSetProperty>
    //          identified by <p identification>.  If the object is found
    //          it is returned in <p object> and the result is < e bool.true>.
    //          If the object is not found the result is <e bool.false>.
  virtual bool findObject(void* identification, OMObject*& object) const = 0;

};

#endif

