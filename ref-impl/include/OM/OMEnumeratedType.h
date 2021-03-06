//=---------------------------------------------------------------------=
//
// $Id: OMEnumeratedType.h,v 1.7 2009/06/01 11:46:58 stuart_hc Exp $ $Name:  $
//
// The contents of this file are subject to the AAF SDK Public Source
// License Agreement Version 2.0 (the "License"); You may not use this
// file except in compliance with the License.  The License is available
// in AAFSDKPSL.TXT, or you may obtain a copy of the License from the
// Advanced Media Workflow Association, Inc., or its successor.
//
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
// the License for the specific language governing rights and limitations
// under the License.  Refer to Section 3.3 of the License for proper use
// of this Exhibit.
//
// WARNING:  Please contact the Advanced Media Workflow Association,
// Inc., for more information about any additional licenses to
// intellectual property covering the AAF Standard that may be required
// to create and distribute AAF compliant products.
// (http://www.amwa.tv/policies).
//
// Copyright Notices:
// The Original Code of this file is Copyright 1998-2009, licensor of the
// Advanced Media Workflow Association.  All rights reserved.
//
// The Initial Developer of the Original Code of this file and the
// licensor of the Advanced Media Workflow Association is
// Avid Technology.
// All rights reserved.
//
//=---------------------------------------------------------------------=

// @doc OMEXTERNAL
#ifndef OMENUMERATEDTYPE_H
#define OMENUMERATEDTYPE_H

#include "OMType.h"
#include "OMVector.h"

// @author Tim Bingham | tjb | Avid Technology, Inc. | OMEnumeratedType
class OMEnumeratedType : public OMType {
public:

  virtual OMType::Tag tag(void) const;

  virtual OMType* elementType(void) const = 0;

  virtual OMUInt32 elementCount(void) const = 0;

  virtual const wchar_t* elementName(OMUInt32 index) const = 0;

  virtual OMInt64 elementValue(OMUInt32 index) const = 0;

  virtual wchar_t* elementNameFromValue(OMInt64 value) const = 0;

  virtual OMInt64 elementValueFromName(const wchar_t* name) const = 0;

};

#endif
