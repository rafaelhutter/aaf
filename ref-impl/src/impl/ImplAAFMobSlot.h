//@doc
//@class    AAFMobSlot | Implementation class for AAFMobSlot
#ifndef __ImplAAFMobSlot_h__
#define __ImplAAFMobSlot_h__

#include "ImplAAFObject.h"
#ifndef __ImplAAFSegment_h__
#include "ImplAAFSegment.h"
#endif
#include "OMProperty.h"

/******************************************\
*                                          *
* Advanced Authoring Format                *
*                                          *
* Copyright (c) 1998 Avid Technology, Inc. *
* Copyright (c) 1998 Microsoft Corporation *
*                                          *
\******************************************/

////////////////////////////////////////////////////////////////////////////////
// Types required by this module:
//
// aafBool,
// aafRational_t,
// AAFSegment,
// aafPosition_t,
// aafSlotID_t,
// aafUInt32,
// AAFDataDef

const int PID_MOBSLOT_NAME				= 0;
const int PID_MOBSLOT_TRACKID			= 1;
const int PID_MOBSLOT_PHYSICAL_TRACK	= 2;
const int PID_MOBSLOT_SEGMENT			= 3;

class ImplAAFSegment;

class ImplAAFDataDef;







#ifndef __ImplAAFObject_h__
#include "ImplAAFObject.h"
#endif


class ImplAAFMobSlot : public ImplAAFObject
{
public:
  //
  // Constructor/destructor
  //
  //********
  ImplAAFMobSlot ();
  ~ImplAAFMobSlot ();

  OMDECLARE_STORABLE(ImplAAFMobSlot)
  //****************
  // GetSegment()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    GetSegment
        (ImplAAFSegment ** result);  //@parm [out,retval] Segment property value

  //****************
  // SetSegment()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    SetSegment
        (ImplAAFSegment *value);  //@parm [in] Segment property value


 //****************
  // GetName()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    GetName
        (aafWChar *  name,  //@parm [in] buffer provided by client to hold the Mob Slot Name
		aafInt32	bufsize);	//@parm [in] length of the buffer provided to hold the slot name

  //****************
  // GetNameLength()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    GetNameLength
		(aafInt32	*bufsize);	//@parm [in] length of the buffer provided to hold the slot name
							// including the terminator

  //****************
  // SetName()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    SetName
        (aafWChar *  name);  //@parm [in] Mob Slot Name


  //****************
  // GetPhysicalNum()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    GetPhysicalNum
        (aafUInt32 *  result);  //@parm [out,retval] The physical track number property value


  //****************
  // SetPhysicalNum()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    SetPhysicalNum
        (aafUInt32  number);  //@parm [in] The physical track number property value


  //****************
  // GetDataKind()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
    GetDataDef
        (aafUID_t *result);  //@parm [out,retval] Data Definition UUID

  //***********************************************************
  // METHOD NAME: GetSlotID()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
  GetSlotID (
    // @parm [out,retval] aafSlotID_t * | result | Slot id of the Mob Slot
    aafSlotID_t *  result
  );

  //***********************************************************
  // METHOD NAME: SetSlotID()
  //
  virtual AAFRESULT STDMETHODCALLTYPE
  SetSlotID (
    // @parm [in] aafSlotID_t | value | Slot id of the Mob Slot
    aafSlotID_t value
  );

public:
  // Declare the module test method. The implementation of the will be be
  // in /test/ImplAAFMobSlotTest.cpp.
  static AAFRESULT test();

protected:
	OMWideStringProperty				_name;
	OMFixedSizeProperty<aafUInt32>		_trackID;
	OMFixedSizeProperty<aafUInt32>		_physicalTrackNum;
	OMStrongReferenceProperty<ImplAAFSegment> _segment;
};

#endif // ! __ImplAAFMobSlot_h__

