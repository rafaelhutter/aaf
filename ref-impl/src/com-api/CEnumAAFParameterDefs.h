//@doc
//@class    EnumAAFParameterDefs | Implementation class for EnumAAFParameterDefs
#ifndef __CEnumAAFParameterDefs_h__
#define __CEnumAAFParameterDefs_h__

//=---------------------------------------------------------------------=
//
// This file was GENERATED for the AAF SDK
//
// $Id: CEnumAAFParameterDefs.h,v 1.6 2009/06/01 11:47:04 stuart_hc Exp $ $Name:  $
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



#ifndef __AAF_h__
#include "AAF.h"
#endif








#ifndef __CAAFRoot_h__
#include "CAAFRoot.h"
#endif


class CEnumAAFParameterDefs
  : public IEnumAAFParameterDefs,
    public CAAFRoot
{
protected:

  //********
  //
  // Constructor/destructor
  //
  CEnumAAFParameterDefs (IUnknown * pControllingUnknown, aafBool doInit = kAAFTrue);
  virtual ~CEnumAAFParameterDefs ();

public:



  //***********************************************************
  //
  // NextOne()
  //
  // Enumerates to the next element in the enumerators list. The
  /// caller is responsible for properly releasing the returned pointer
  /// when it is no longer needed.
  /// 
  /// Succeeds if all of the following are true:
  /// - the ppParameterDef pointer is valid.
  /// - there are ParameterDef objects remaining to be returned.
  /// 
  /// If this method fails nothing is written to *ppParameterDef.
  /// 
  /// This method will return the following codes.  If more than one of
  /// the listed errors is in effect, it will return the first one
  /// encountered in the order given below:
  /// 
  /// AAFRESULT_SUCCESS
  ///   - succeeded.  (This is the only code indicating success.)
  ///
  /// AAFRESULT_NULL_PARAM
  ///   - ppParameterDef is null.
  ///
  /// AAFRESULT_NO_MORE_OBJECTS
  ///   - no ParameterDefs remaining to be returned.
  //
  STDMETHOD (NextOne) (
    // The Next ParameterDefinition 
    /*[out,retval]*/ IAAFParameterDef ** ppParameterDef);

  //***********************************************************
  //
  // Next()
  //
  // Enumerates the next count elements (AAFParameterDef pointers) in the
  /// enumerator's list, returning them in the given array along with
  /// the actual number of enumerated elements in pNumFetched. The caller
  /// is responsible for properly releasing the returned pointers.
  /// 
  /// Succeeds if all of the following are true:
  /// - The ppParameterDefs pointer is valid.
  /// - The pNumFetched pointer is valid. If count is 1, pNumFetched can be NULL.
  /// - There are AAFParameterDef objects remaining to be returned.
  /// 
  /// If this method fails nothing is written to *ppComponents or
  /// pNumFetched.
  /// 
  /// This method will return the following codes.  If more than one of
  /// the listed errors is in effect, it will return the first one
  /// encountered in the order given below:
  /// 
  /// AAFRESULT_SUCCESS
  ///   - succeeded.  (This is the only code indicating success.)
  ///
  /// AAFRESULT_NULL_PARAM
  ///   - either ppParameterDefs or pNumFetched is null.
  //
  STDMETHOD (Next) (
    // number of control code definitions requested
    /*[in]*/ aafUInt32  count,

    // array to receive control code definitions
    /*[out, size_is(count), length_is(*pFetched)]*/ IAAFParameterDef ** ppParameterDefs,

    // number of actual ParameterDefs fetched into ppParameterDefs array
    /*[out,ref]*/ aafUInt32 *  pFetched);

  //***********************************************************
  //
  // Skip()
  //
  // Instructs the enumerator to skip the next count elements in the
  /// enumeration so that the next call to Next will not
  /// return those elements.
  /// 
  /// Succeeds if all of the following are true:
  /// - count is less than or equal to the number of remaining objects.
  /// 
  /// This method will return the following codes.  If more than one of
  /// the listed errors is in effect, it will return the first one
  /// encountered in the order given below:
  ///
  /// AAFRESULT_SUCCESS
  ///   - succeeded.
  ///
  /// AAFRESULT_NO_MORE_OBJECTS
  ///   - count exceeded number of remaining objects.
  //
  STDMETHOD (Skip) (
    // Number of elements to skip 
    /*[in]*/ aafUInt32  count);

  //***********************************************************
  //
  // Reset()
  //
  // Instructs the enumerator to position itself at the beginning of
  /// the list of elements.
  /// 
  /// Always succeeds.
  /// 
  /// This method will return the following code:
  /// 
  /// AAFRESULT_SUCCESS
  ///   - succeeded.  (This is the only code indicating success.)
  //
  STDMETHOD (Reset)
     ();

  /// @comm There is no guarantee that the same set of elements will be enumerated on 
  /// each pass through the list\, nor will the elements necessarily be enumerated in 
  /// the same order. The exact behavior depends on the collection being enumerated.)

  //***********************************************************
  //
  // Clone()
  //
  // Creates another EnumAAFParameterDefs enumerator with the same state as the current
  /// enumerator to iterate over the same list. This method makes it
  /// possible to record a point in the enumeration sequence in order
  /// to return to that point at a later time.
  ///
  /// Note: The caller must release this new enumerator separately from
  /// the first enumerator.
  /// 
  /// Succeeds if all of the following are true:
  /// - the ppEnum pointer is valid.
  /// 
  /// This method will return the following codes.  If more than one of
  /// the listed errors is in effect, it will return the first one
  /// encountered in the order given below:
  /// 
  /// AAFRESULT_SUCCESS
  ///   - succeeded.  (This is the only code indicating success.)
  ///
  /// AAFRESULT_NULL_PARAM
  ///   - ppEnum is null.
  //
  STDMETHOD (Clone) (
    // new enumeration 
    /*[out,retval]*/ IEnumAAFParameterDefs ** ppEnum);


protected:
  // 
  // Declare the QI that implements for the interfaces
  // for this module. This will be called by CAAFUnknown::QueryInterface().
  // 
  STDMETHOD(InternalQueryInterface)(REFIID riid, void **ppvObjOut);


public:
  //
  // This class as concrete. All AAF objects can be constructed from
  // a CLSID. This will allow subclassing all "base-classes" by
  // aggreggation.
  // 
  AAF_DECLARE_CONCRETE();
  //
  //********
};

#endif // ! __CEnumAAFParameterDefs_h__


