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

#include "ImplAAFFile.h"

#include "OMFile.h"
#include "OMUtilities.h"
#include "OMClassFactory.h"

#include "ImplAAFDataDef.h"
#include "ImplAAFDictionary.h"
#include "ImplAAFMetaDictionary.h"
#include "ImplAAFHeader.h"
#include "ImplAAFRandomRawStorage.h"
#include "ImplAAFIdentification.h"

#include "AAFStoredObjectIDs.h"
#include "ImplAAFObjectCreation.h"
#include "ImplAAFBuiltinDefs.h"
#include "ImplAAFOMRawStorage.h"

#include "AAFFileSignatures.h"
#include "AAFFileMode.h"

#include "ImplAAFSmartPointer.h"
typedef ImplAAFSmartPointer<ImplAAFIdentification>
  ImplAAFIdentificationSP;
typedef ImplAAFSmartPointer<ImplAAFRandomRawStorage>
  ImplAAFRandomRawStorageSP;

#include <assert.h>

// For IAAFRawStorage
#include "AAF.h"

// AAF file signature.
//static const OMFileSignature aafFileSignature  =
//{0x42464141,
//0xff0d, 0x4d4f,
//{0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0xff}};


static const aafUID_t kNullAuid = { 0 };
static const aafProductIdentification_t kNullIdent = { 0 };

//
// File Format Version
//
// Update this when incompatible changes are made to AAF file format
// version. 
//
//    0 : Tue Jan 11 17:08:26 EST 2000
//        Initial Release version.
//
static const aafUInt32 sCurrentAAFObjectModelVersion = 0;


static const aafUID_t kNullFileKind = { 0 };


// local function for simplifying error handling.
inline void checkResult(AAFRESULT r)
{
  if (AAFRESULT_SUCCESS != r)
    throw r;
}

inline void checkExpression(bool test, AAFRESULT r)
{
  if (!test)
    throw r;
}


//
// Returns true if all flags set are defined flags; returns false if
// any flags set are *not* defined.
//
static bool areAllModeFlagsDefined (aafUInt32 modeFlags)
{
  static const aafUInt32 kAllFlags =
	AAF_FILE_MODE_EAGER_LOADING |
	AAF_FILE_MODE_REVERTABLE |
	AAF_FILE_MODE_UNBUFFERED |
	AAF_FILE_MODE_RECLAIMABLE |
	AAF_FILE_MODE_USE_LARGE_SS_SECTORS |
	AAF_FILE_MODE_CLOSE_FAIL_DIRTY |
	AAF_FILE_MODE_DEBUG0_ON |
	AAF_FILE_MODE_DEBUG1_ON;

  if (modeFlags & (~kAllFlags))
	{
	  // at least one flag is not defined
	  return false;
	}
  else
	{
	  return true;
	}
}


extern "C" const aafClassID_t CLSID_AAFDictionary;

AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::Initialize ()
{
	if (_initialized)
	{
		return AAFRESULT_ALREADY_INITIALIZED;
	}

	// Create the class factory for base classes.
	_factory = ImplAAFDictionary::CreateDictionary();
	if (NULL == _factory)
		return AAFRESULT_NOMEMORY;

	// Create the class factory for meta classes.
	_metafactory = ImplAAFMetaDictionary::CreateMetaDictionary();
	if (NULL == _metafactory)
		return AAFRESULT_NOMEMORY;

	// The following is temporary code. (transdel:2000-APR-11)
	_factory->setMetaDictionary(_metafactory);
	_metafactory->setDataDictionary(_factory);
	
	// Experimental: Create and initialize all of the axiomatic definitions.
	// This must be done before another other definitions or data objects
	// can be created.
	AAFRESULT result = _metafactory->InstantiateAxiomaticDefinitions();
	assert(AAFRESULT_SUCCEEDED(result));

	_initialized = kAAFTrue;

	return result;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::OpenExistingRead (const aafCharacter * pFileName,
							   aafUInt32 modeFlags)
{
	AAFRESULT stat = AAFRESULT_SUCCESS;

	// Validate parameters and preconditions.
	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (IsOpen())
		return AAFRESULT_ALREADY_OPEN;

	if (_file)
		return AAFRESULT_ALREADY_OPEN;

	if (! pFileName)
		return AAFRESULT_NULL_PARAM;

	if (! areAllModeFlagsDefined (modeFlags))
	  return AAFRESULT_BAD_FLAGS;

	if (modeFlags)
	  return AAFRESULT_NOT_IN_CURRENT_VERSION;

	// Save the mode flags for now. They are not currently (2/4/1999) used by the
	// OM to open the doc file. Why do we return an error if modeFlags != 0?
	//
	// Answer: because none of them are implemented yet.
	_modeFlags = modeFlags;

	try
	{
		// Ask the OM to open the file.
		_file = OMFile::openExistingRead(pFileName,
										 _factory,
										 0,
										 OMFile::lazyLoad,
										 _metafactory);
		checkExpression(NULL != _file, AAFRESULT_INTERNAL_ERROR);

        // Check the file's signature.
        OMFileSignature sig = _file->signature();
		const OMFileSignature aafFileSignature =
		  *reinterpret_cast<const OMFileSignature *>
		  (&aafFileSignatureAafSSBinaryGUID);
        checkExpression(sig == aafFileSignature, AAFRESULT_NOT_AAF_FILE);

		// Get the byte order
		OMByteOrder byteOrder = _file->byteOrder();
		if (byteOrder == littleEndian) {
			_byteOrder = 0x4949; // 'II'
		} else { // bigEndian
			_byteOrder = 0x4d4d; // 'MM'
		}

		// Restore the header.
		bool regWasEnabled = _factory->SetEnableDefRegistration (false);
		OMStorable* head = _file->restore();
		_head = dynamic_cast<ImplAAFHeader *>(head);
		_head->SetFile(this);
		
		// Check for file format version.
		if (_head->IsObjectModelVersionPresent())
		  {
			// If property isn't present, the default version is 0,
			// which is always (supposed to be) legible.  If it is
			// present, find out the version number to determine if
			// the file is legible.
			if (_head->GetObjectModelVersion() >
				sCurrentAAFObjectModelVersion)
			  {
				// File version is higher than the version understood
				// by this toolkit.  Therefore this file cannot be read.
				return AAFRESULT_FILEREV_DIFF;
			  }
		  }

		// Now that the file is open and the header has been
		// restored, complete the initialization of the
		// dictionary. We obtain the dictionary via the header
		// to give the persistence mechanism a chance to work.
    _factory->InitOMProperties(_factory->GetBuiltinDefs()->cdDictionary());
		ImplAAFDictionary* dictionary = 0;
		HRESULT hr = _head->GetDictionary(&dictionary);
		if (hr != AAFRESULT_SUCCESS)
		  return hr;
		_factory->SetEnableDefRegistration (regWasEnabled);
		dictionary->InitBuiltins();
		dictionary->ReleaseReference();
		dictionary = 0;

		_isOpen = kAAFTrue;
		_openType = kOmRead;
	}
	catch (AAFRESULT &r)
	{
		stat = r;

		// Cleanup after failure.
		if (_file)
		{
			_file->close();
			_file = 0;
		}

		if (NULL == _head)
		{
			_head->ReleaseReference();
			_head = 0;
		}
	}
	
	return stat;
}



AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::OpenExistingModify (const aafCharacter * pFileName,
								 aafUInt32 modeFlags,
								 aafProductIdentification_t * pIdent)
{
	AAFRESULT stat = AAFRESULT_SUCCESS;


	// Validate parameters and preconditions.
	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (IsOpen())
		return AAFRESULT_ALREADY_OPEN;

	if (_file)
		return AAFRESULT_ALREADY_OPEN;

	if (! pFileName)
		return AAFRESULT_NULL_PARAM;

	if (! pIdent)
		return AAFRESULT_NULL_PARAM;

	if (! areAllModeFlagsDefined (modeFlags))
	  return AAFRESULT_BAD_FLAGS;

	if (modeFlags)
	  return AAFRESULT_NOT_IN_CURRENT_VERSION;
	
	// Save the mode flags for now. They are not currently (2/4/1999) used by the
	// OM to open the doc file. Why do we return an error if modeFlags != 0?
	//
	// Answer: because none of them are implemented yet.
	_modeFlags = modeFlags;

	try 
	{
		// Ask the OM to open the file.
		_file = OMFile::openExistingModify(pFileName,
										   _factory,
										   0,
										   OMFile::lazyLoad,
										   _metafactory);
		checkExpression(NULL != _file, AAFRESULT_INTERNAL_ERROR);

		// Get the byte order
		OMByteOrder byteOrder = _file->byteOrder();
		if (byteOrder == littleEndian) {
			_byteOrder = 0x4949; // 'II'
		} else { // bigEndian
			_byteOrder = 0x4d4d; // 'MM'
		}

		// Restore the header.
		bool regWasEnabled = _factory->SetEnableDefRegistration (false);
		OMStorable* head = _file->restore();
		_head = dynamic_cast<ImplAAFHeader *>(head);
		checkExpression(NULL != _head, AAFRESULT_BADHEAD);
		// Check for file format version.
		if (_head->IsObjectModelVersionPresent())
		  {
			// If property isn't present, the default version is 0,
			// which is always (supposed to be) legible.  If it is
			// present, find out the version number to determine if
			// the file is legible.
			if (_head->GetObjectModelVersion() >
				sCurrentAAFObjectModelVersion)
			  {
				// File version is higher than the version understood
				// by this toolkit.  Therefore this file cannot be read.
				return AAFRESULT_FILEREV_DIFF;
			  }
		  }

		// Now that the file is open and the header has been
		// restored, complete the initialization of the
		// dictionary. We obtain the dictionary via the header
		// to give the persistence mechanism a chance to work.
		ImplAAFDictionary* dictionary = 0;
		HRESULT hr = _head->GetDictionary(&dictionary);
		if (hr != AAFRESULT_SUCCESS)
		  return hr;
		_factory->SetEnableDefRegistration (regWasEnabled);
		dictionary->InitBuiltins();
		dictionary->ReleaseReference();
		dictionary = 0;

		checkResult(_head->SetToolkitRevisionCurrent());
	  
		// NOTE: If modifying an existing file WITHOUT an IDNT object, add a
		// dummy IDNT object to indicate that this program was not the creator.
		//
		aafUInt32	numIdent = 0;
		checkResult(_head->CountIdentifications(&numIdent));
		if(numIdent == 0)
		{
			_head->AddIdentificationObject((aafProductIdentification_t *)NULL);
		}
		// Now, always add the information from THIS application */
		_head->AddIdentificationObject(pIdent);
		
		_isOpen = kAAFTrue;
		_openType = kOmModify;
	}
	catch (AAFRESULT &rc)
	{
		stat = rc;

		// Cleanup after failure.
		if (_file)
		{
			_file->close();
			_file = 0;
		}

		if (NULL == _head)
		{
			_head->ReleaseReference();
			_head = 0;
		}
	}

	return stat;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::OpenNewModify (const aafCharacter * pFileName,
							aafUInt32 modeFlags,
							aafProductIdentification_t * pIdent)
{
	ImplAAFContentStorage	*pCStore = NULL;
	AAFRESULT stat = AAFRESULT_SUCCESS;
	aafVersionType_t		theVersion = { 1, 0 };

	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (IsOpen())
		return AAFRESULT_ALREADY_OPEN;

	if (_file)
		return AAFRESULT_ALREADY_OPEN;

	if (! pFileName)
		return AAFRESULT_NULL_PARAM;

	if (! pIdent)
		return AAFRESULT_NULL_PARAM;

	if (! areAllModeFlagsDefined (modeFlags))
	  return AAFRESULT_BAD_FLAGS;

	if (modeFlags)
	  return AAFRESULT_NOT_IN_CURRENT_VERSION;

	try
	{
		// Create the header for the OM manager to use as the root
		// for the file. Use the OMClassFactory interface for this 
    // object because the dictionary has not been associated with
    // a header yet (Chicken 'n Egg problem).
    const OMClassId& soid = *reinterpret_cast<const OMClassId *>(&AUID_AAFHeader);
    _head = static_cast<ImplAAFHeader *>(_factory->create(soid));
		checkExpression(NULL != _head, AAFRESULT_BADHEAD);
		
		// Make sure the header is initialized with our previously created
		// dictionary.
		_head->SetDictionary(_factory);

		// Set the file format version.
		//
		// BobT Fri Jan 21 14:37:43 EST 2000: the default behavior is
		// that if the version isn't present, it's assumed to Version
		// 0.  Therefore if the current version is 0, don't write out
		// the property.  We do this so that hackers examining written
		// files won't know that a mechanism exists to mark future
		// incompatible versions, and so will work harder to make any
		// future changes compatible.
		if (sCurrentAAFObjectModelVersion)
		  {
			_head->SetObjectModelVersion(sCurrentAAFObjectModelVersion);
		  }

		// Add the ident to the header.
		checkResult(_head->AddIdentificationObject(pIdent));
		  
		// Set the byte order
		OMByteOrder byteOrder = hostByteOrder();
		if (byteOrder == littleEndian) {
			_byteOrder = 0x4949; // 'II'
		} else { // bigEndian
			_byteOrder = 0x4d4d; // 'MM'
		}
		_head->SetByteOrder(_byteOrder);
		_head->SetFileRevision (theVersion);

		//JeffB!!! We must decide whether def-only files have a content storage
		checkResult(_head->GetContentStorage(&pCStore));
		pCStore->ReleaseReference(); // need to release this pointer!
		pCStore = 0;

		// Attempt to create the file.
		const OMFileSignature aafFileSignature =
		  *reinterpret_cast<const OMFileSignature *>
		  (&aafFileSignatureAafSSBinaryGUID);
		_file = OMFile::openNewModify(pFileName,
									  _factory,
									  0,
									  byteOrder,
									  _head,
									  aafFileSignature,
									  _metafactory);
		checkExpression(NULL != _file, AAFRESULT_INTERNAL_ERROR);

		// Now that the file is open and the header has been
		// restored, complete the initialization of the
		// dictionary. We obtain the dictionary via the header
		// to give the persistence mechanism a chance to work.
		ImplAAFDictionary* dictionary = 0;
		HRESULT hr = _head->GetDictionary(&dictionary);
		if (hr != AAFRESULT_SUCCESS)
		  return hr;
		dictionary->InitOMProperties
		  (dictionary->GetBuiltinDefs()->cdDictionary());
		dictionary->InitBuiltins();

		dictionary->ReleaseReference();
		dictionary = 0;

		_isOpen = kAAFTrue;
		_openType = kOmCreate;
		GetRevision(&_setrev);
	}
	catch (AAFRESULT &rc)
	{
		stat = rc;

		// Cleanup after failure.
		if (_file)
		{
			_file->close();
			_file = 0;
		}

		if (NULL == _head)
		{
			_head->ReleaseReference();
			_head = 0;
		}
	}

	return stat;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::OpenTransient (aafProductIdentification_t * pIdent)
{

	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (IsOpen())
		return AAFRESULT_ALREADY_OPEN;

	if (! pIdent)
		return AAFRESULT_NULL_PARAM;

	return AAFRESULT_NOT_IN_CURRENT_VERSION;
}



AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::CreateAAFFileOnRawStorage
  (IAAFRawStorage * pRawStorage,
   aafUID_constptr pFileKind,
   aafUInt32 modeFlags,
   aafProductIdentification_constptr pIdent)
{
  if (! _initialized)
	return AAFRESULT_NOT_INITIALIZED;

  if (! pRawStorage)
	return AAFRESULT_NULL_PARAM;

  if (IsOpen())
	return AAFRESULT_ALREADY_OPEN;

  if (! areAllModeFlagsDefined (modeFlags))
	return AAFRESULT_BAD_FLAGS;

  if (modeFlags)
	return AAFRESULT_NOT_IN_CURRENT_VERSION;
	
  // Create the OM storage...
  assert (! _pOMStg);
  _pOMStg = new ImplAAFOMRawStorage (pRawStorage);
  assert (_pOMStg);

  _isReadable = _pOMStg->isReadable();
  _isWriteable = _pOMStg->isWritable();

  if (pFileKind)
	{
	  _preOpenFileKindSet = kAAFTrue;
	  _preOpenFileKind = *pFileKind;
	}
  else
	{
	  _preOpenFileKindSet = kAAFFalse;
	  _preOpenFileKind = kNullFileKind;
	}
  if (pIdent)
	{
	  _preOpenIdentSet = kAAFTrue;
	  _preOpenIdent = *pIdent;
	}
  else
	{
	  _preOpenIdentSet = kAAFFalse;
	  _preOpenIdent = kNullIdent;
	}

  // Save the mode flags for now. They are not currently (2/4/1999)
  // used by the OM to open the doc file. Why do we return an error
  // if modeFlags != 0?
  //
  // Answer: because none of them are implemented yet.
  _modeFlags = modeFlags;

  return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::Open ()
{
  if (IsOpen() || IsClosed())
	return AAFRESULT_ALREADY_OPEN;

  _isOpen = kAAFTrue;

  // Here we decide which of the pvtCreateXXX() methods to call.  The
  // different possibilities:
  //
  // new
  // existing
  //
  // read-only (R)
  // modify (RW)
  // write-only (W) (for streams only?)
  //
  // Let's chart them:
  //
  //               new                 existing
  //               ---                 --------
  // R           <none>              pvtCreateExistingRead
  // W or RW     pvtCreateNewModify  pvtCreateExistingModify
  //
  // We can determine the {new,existing} criterion by asking the
  // storage for its size.  Note that this only works for random raw
  // storages.  If it's *not* a random raw storage, assume it's a
  // stream and assume it's a new file (for write) and an existing one
  // (for read).

  // BobT: Note that for now we're saving versions of the _preOpenXXX
  // data; this may no longer be necessary once OM support is
  // implemented allowing the client to get the raw storage of a file
  // which has not yet been opened.

  if (IsWriteable())
	{
	  // Figure out if file exists already or not.  If it's not
	  // positionable, assume it's on a stream and therefore a
	  // writeable file on a stream is not pre-existing.
	  aafBoolean_t existing = kAAFFalse;
	  if (_pOMStg->isPositionable())
		{
		  if (0 != _pOMStg->size())
			existing = kAAFTrue;
		}
	  if (existing)
		{
		  return pvtCreateExistingModify
			(_preOpenIdentSet ?	&_preOpenIdent : 0);
		}
	  else
		{
		  return pvtCreateNewModify
			(_preOpenFileKindSet ? &_preOpenFileKind : 0,
			 _preOpenIdentSet ?	&_preOpenIdent : 0);
		}
	}
  else if (IsReadable())
	{
	  // If we're here, it's not writeable, so it must be read-only.
	  return pvtCreateExistingRead ();
	}
  else
	return AAFRESULT_INVALID_PARAM;
}


AAFRESULT ImplAAFFile::pvtCreateExistingRead ()
{
	AAFRESULT stat = AAFRESULT_SUCCESS;

	try
	  {
		assert (_pOMStg);

		// Ask the OM to open the file.
		_file = OMFile::openExistingRead(_pOMStg,
										 _factory,
										 0,
										 OMFile::lazyLoad,
										 _metafactory);
		checkExpression(NULL != _file, AAFRESULT_INTERNAL_ERROR);
		// Relinquish ownership of the storage to the OMFile object.
		_pOMStg = 0;

		_file->open ();

        // Check the file's signature.
        OMFileSignature sig = _file->signature();
		const OMFileSignature aafFileSignature  =
		  *reinterpret_cast<const OMFileSignature *>
		  (&aafFileSignatureAafSSBinaryGUID);
        checkExpression(sig == aafFileSignature, AAFRESULT_NOT_AAF_FILE);

		// Get the byte order
		OMByteOrder byteOrder = _file->byteOrder();
		if (byteOrder == littleEndian)
		  {
			_byteOrder = 0x4949; // 'II'
		  }
		else
		  { // bigEndian
			_byteOrder = 0x4d4d; // 'MM'
		  }

		// Restore the header.
		bool regWasEnabled = _factory->SetEnableDefRegistration (false);
		OMStorable* head = _file->restore();
		_head = dynamic_cast<ImplAAFHeader *>(head);
		_head->SetFile(this);
		
		// Check for file format version.
		if (_head->IsObjectModelVersionPresent())
		  {
			// If property isn't present, the default version is 0,
			// which is always (supposed to be) legible.  If it is
			// present, find out the version number to determine if
			// the file is legible.
			if (_head->GetObjectModelVersion() >
				sCurrentAAFObjectModelVersion)
			  {
				// File version is higher than the version understood
				// by this toolkit.  Therefore this file cannot be read.
				return AAFRESULT_FILEREV_DIFF;
			  }
		  }

		// Now that the file is open and the header has been
		// restored, complete the initialization of the
		// dictionary. We obtain the dictionary via the header
		// to give the persistence mechanism a chance to work.
		_factory->InitOMProperties
		  (_factory->GetBuiltinDefs()->cdDictionary());
		ImplAAFDictionary* dictionary = 0;
		HRESULT hr = _head->GetDictionary(&dictionary);
		if (hr != AAFRESULT_SUCCESS)
		  return hr;
		_factory->SetEnableDefRegistration (regWasEnabled);
		dictionary->InitBuiltins();
		dictionary->ReleaseReference();
		dictionary = 0;

		_openType = kOmRead;
	}
	catch (AAFRESULT &r)
	{
		stat = r;

		// Cleanup after failure.
		if (NULL == _head)
		{
			_head->ReleaseReference();
			_head = 0;
		}
	}
	
	return stat;
}


AAFRESULT ImplAAFFile::pvtCreateExistingModify
    (aafProductIdentification_constptr pIdent)
{
  AAFRESULT stat = AAFRESULT_SUCCESS;

  try 
	{
	  AAFRESULT hr;
	  // Ask the OM to open the file.
	  assert (_pOMStg);
	  _file = OMFile::openExistingModify(_pOMStg,
										 _factory,
										 0,
										 OMFile::lazyLoad,
										 _metafactory); 
	  checkExpression(NULL != _file, AAFRESULT_INTERNAL_ERROR);
	  // Relinquish ownership of the storage to the OMFile object.
	  _pOMStg = 0;
	  _file->open ();

	  // Get the byte order
	  OMByteOrder byteOrder = _file->byteOrder();
	  if (byteOrder == littleEndian)
		{
		  _byteOrder = 0x4949; // 'II'
		}
	  else
		{
		  // bigEndian
		  _byteOrder = 0x4d4d; // 'MM'
		}

	  // Restore the header.
	  bool regWasEnabled = _factory->SetEnableDefRegistration (false);
	  OMStorable* head = _file->restore();
	  _head = dynamic_cast<ImplAAFHeader *>(head);
	  checkExpression(NULL != _head, AAFRESULT_BADHEAD);
	  // Check for file format version.
	  if (_head->IsObjectModelVersionPresent())
		{
		  // If property isn't present, the default version is 0,
		  // which is always (supposed to be) legible.  If it is
		  // present, find out the version number to determine if
		  // the file is legible.
		  if (_head->GetObjectModelVersion() >
			  sCurrentAAFObjectModelVersion)
			{
			  // File version is higher than the version understood
			  // by this toolkit.  Therefore this file cannot be read.
			  return AAFRESULT_FILEREV_DIFF;
			}
		}

	  // Now that the file is open and the header has been
	  // restored, complete the initialization of the
	  // dictionary. We obtain the dictionary via the header
	  // to give the persistence mechanism a chance to work.
	  ImplAAFDictionary* dictionary = 0;
	  hr = _head->GetDictionary(&dictionary);
	  if (hr != AAFRESULT_SUCCESS)
		return hr;
	  _factory->SetEnableDefRegistration (regWasEnabled);
	  dictionary->InitBuiltins();
	  dictionary->ReleaseReference();
	  dictionary = 0;

	  checkResult(_head->SetToolkitRevisionCurrent());
	  
	  // NOTE: If modifying an existing file WITHOUT an IDNT object, add a
	  // dummy IDNT object to indicate that this program was not the creator.
	  //
	  aafUInt32	numIdent = 0;
	  checkResult(_head->CountIdentifications(&numIdent));
	  if(numIdent == 0)
		{
		  _head->AddIdentificationObject((aafProductIdentification_t *)NULL);
		}
	  // Now, always add the information from THIS application */
	  _head->AddIdentificationObject(pIdent);
	}
  catch (AAFRESULT &rc)
	{
	  stat = rc;

	  if (NULL == _head)
		{
		  _head->ReleaseReference();
		  _head = 0;
		}
	}

  return stat;
}


AAFRESULT ImplAAFFile::pvtCreateNewModify
    (aafUID_constptr pFileKind,
	 aafProductIdentification_constptr pIdent)
{
	ImplAAFContentStorage	*pCStore = NULL;
	AAFRESULT stat = AAFRESULT_SUCCESS;
	aafVersionType_t		theVersion = { 1, 0 };

	try
	  {
		// Create the header for the OM manager to use as the root
		// for the file. Use the OMClassFactory interface for this 
		// object because the dictionary has not been associated with
		// a header yet (Chicken 'n Egg problem).
		const OMClassId& soid =
		  *reinterpret_cast<const OMClassId *>(&AUID_AAFHeader);
		_head = static_cast<ImplAAFHeader *>(_factory->create(soid));
		checkExpression(NULL != _head, AAFRESULT_BADHEAD);
		
	  // Make sure the header is initialized with our previously created
	  // dictionary.
		_head->SetDictionary(_factory);

		// Set the file format version.
		//
		// BobT Fri Jan 21 14:37:43 EST 2000: the default behavior is
		// that if the version isn't present, it's assumed to Version
		// 0.  Therefore if the current version is 0, don't write out
		// the property.  We do this so that hackers examining written
		// files won't know that a mechanism exists to mark future
		// incompatible versions, and so will work harder to make any
		// future changes compatible.
		if (sCurrentAAFObjectModelVersion)
		  {
			_head->SetObjectModelVersion(sCurrentAAFObjectModelVersion);
		  }

		// Add the ident to the header.
		checkResult(_head->AddIdentificationObject(pIdent));
		  
		// Set the byte order
		OMByteOrder byteOrder = hostByteOrder();
		if (byteOrder == littleEndian)
		  {
			_byteOrder = 0x4949; // 'II'
		  }
		else
		  { // bigEndian
			_byteOrder = 0x4d4d; // 'MM'
		  }
		_head->SetByteOrder(_byteOrder);
		_head->SetFileRevision (theVersion);

		//JeffB!!! We must decide whether def-only files have a content storage
		checkResult(_head->GetContentStorage(&pCStore));
		pCStore->ReleaseReference(); // need to release this pointer!
		pCStore = 0;

		// Attempt to create the file.
		const OMFileSignature aafFileSignature =
		  *reinterpret_cast<const OMFileSignature *> (pFileKind);
		assert (_pOMStg);
		_file = OMFile::openNewModify(_pOMStg,
									  _factory,
									  0,
									  byteOrder,
									  _head,
									  aafFileSignature,
									  _metafactory);
		checkExpression(NULL != _file, AAFRESULT_INTERNAL_ERROR);
		// Relinquish ownership of the storage to the OMFile object.
		_pOMStg = 0;
		_file->open ();

		// Now that the file is open and the header has been
		// restored, complete the initialization of the
		// dictionary. We obtain the dictionary via the header
		// to give the persistence mechanism a chance to work.
		ImplAAFDictionary* dictionary = 0;
		HRESULT hr = _head->GetDictionary(&dictionary);
		if (hr != AAFRESULT_SUCCESS)
		  return hr;
		dictionary->InitOMProperties
		  (dictionary->GetBuiltinDefs()->cdDictionary());
		dictionary->InitBuiltins();

		dictionary->ReleaseReference();
		dictionary = 0;

		_openType = kOmCreate;
		GetRevision(&_setrev);
	  }
	catch (AAFRESULT &rc)
	  {
		stat = rc;

		// Cleanup after failure.
		if (NULL == _head)
		  {
			_head->ReleaseReference();
			_head = 0;
		  }
	  }

	return stat;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::Save ()
{
	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (!IsOpen())
		return AAFRESULT_NOT_OPEN;

	if (! _file)
		return AAFRESULT_NOT_OPEN;
	if (! _file->isOpen())
		return AAFRESULT_NOT_OPEN;

	// If any new modes are added then the following line will
	// have to be updated.
	if (kOmCreate == _openType || kOmModify == _openType) {
	  // Assure no registration of def objects in dictionary during
	  // save operation
	  ImplAAFDictionarySP dictSP;
	  AAFRESULT hr = _head->GetDictionary(&dictSP);
	  if (AAFRESULT_FAILED (hr))
		return hr;
	  dictSP->AssureClassPropertyTypes ();
	  bool regWasEnabled = dictSP->SetEnableDefRegistration (false);

	  // OMFile::save() allows us to pass a client context to be
	  // regurgitated to the OMStorable::onSave() callback.  We'll use
	  // it to pass the AUID of the latest generation.
	  ImplAAFIdentificationSP pLatestIdent;
	  hr = _head->GetLastIdentification (&pLatestIdent);
	  if (AAFRESULT_FAILED (hr)) return hr;
	  aafUID_t latestGen;
	  hr = pLatestIdent->GetGenerationID (&latestGen);
	  if (AAFRESULT_FAILED (hr)) return hr;
	  _file->saveFile(&latestGen);

	  dictSP->SetEnableDefRegistration (regWasEnabled);

	} else {
	  return AAFRESULT_WRONG_OPENMODE;
	}


	// Record the fact that this file was modified
	_head->SetModified();

	return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::SaveCopyAs (ImplAAFFile * pDestFile)
{
  if (! pDestFile)
	return AAFRESULT_NULL_PARAM;
  return AAFRESULT_NOT_IN_CURRENT_VERSION;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::SaveAs (const aafCharacter * pFileName,
					 aafUInt32 modeFlags)
{
	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (!IsOpen())
		return AAFRESULT_NOT_OPEN;

	if (! areAllModeFlagsDefined (modeFlags))
	  return AAFRESULT_BAD_FLAGS;

	if (modeFlags)
	  return AAFRESULT_NOT_IN_CURRENT_VERSION;

	// Assure no registration of def objects in dictionary during
	// save operation
	ImplAAFDictionarySP dictSP;
	AAFRESULT hr = _head->GetDictionary(&dictSP);
	dictSP->AssureClassPropertyTypes ();
	bool regWasEnabled = dictSP->SetEnableDefRegistration (false);

	assert(_file);
	_file->saveAs(pFileName);

	dictSP->SetEnableDefRegistration (regWasEnabled);

	return AAFRESULT_SUCCESS;
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::Revert ()
{
	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (!IsOpen())
		return AAFRESULT_NOT_OPEN;

        assert(_file);
        _file->revert();

	return AAFRESULT_SUCCESS;
}


ImplAAFFile::ImplAAFFile () :
		_cookie(0),
		_file(0),
		_factory(NULL),
		_metafactory(NULL),
		_byteOrder(0),
		_openType(kOmUndefined),
		_head(NULL),
		_semanticCheckEnable(kAAFFalse),
		_initialized(kAAFFalse),
		_isOpen(kAAFFalse)

  ,
  _preOpenFileKindSet (kAAFFalse),
  _preOpenFileKind (kNullFileKind),
  _preOpenIdentSet (kAAFFalse),
  _preOpenIdent (kNullIdent),
  _pOMStg (0),
  _isClosed (false),
  _isReadable (false),
  _isWriteable (false)

{
}


ImplAAFFile::~ImplAAFFile ()
{
	InternalReleaseObjects();

	// cleanup the container.
	if (_factory)
	{
		_factory->ReleaseReference();
		_factory = NULL;
	}

	if (_metafactory)
	{
		_metafactory->ReleaseReference();
		_metafactory = NULL;
	}

	// cleanup the OM File.
	if (_file)
	{
		delete _file;
		_file = NULL;
	}

	if (_pOMStg)
	  {
		delete _pOMStg;
		_pOMStg = 0;
	  }
}

void ImplAAFFile::InternalReleaseObjects()
{
}


//***********************************************************
// METHOD NAME: Close()
//
// DESCRIPTION:
// @mfunc AAFRESULT | AAFFile | Close |
// Closes an AAF file, saving the result.
// @end
//
//
AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::Close ()
{
	if (! _initialized)
		return AAFRESULT_NOT_INITIALIZED;

	if (!IsOpen())
		return AAFRESULT_NOT_OPEN;


	// Release all of the pointers that we created or copied
	// during the create or open methods.
	InternalReleaseObjects();

	// Close the OM file.
	_file->close();
	_isClosed = kAAFTrue;

	// Whenever a file is created or opened a new container is
	// created.  If we don't want to leak the container object
	// and any objects in the associated OMFile object we had
	// better delete the container object here.
	//
	// BobT: Keep this around longer!  We'll need GetBits later...
	// delete _file;
	// _file = 0;
  
	// Release the last reference to the header of the file. 
	// We need to release the header after the file is closed so
	// that the OMFile object within the container can safely
	// use its reference to its root (a.k.a. header).
	if (0 != _head)
	{
		_head->ReleaseReference();
		_head = 0;
	}

	_cookie = 0;
	_isOpen = kAAFFalse;
	// BobT: don't set this, so we'll remember what open type this
	// file had
	// _openType = kOmUndefined;

	return(AAFRESULT_SUCCESS);
}


//***********************************************************
// METHOD NAME: GetRevision()
//
// DESCRIPTION:
// @mfunc AAFRESULT | AAFFile | GetRevision |
// Get the revision of the ciurrent AAF file.
// @end
// 
AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::GetRevision
 (
  // @parm aafFileRev_t * | rev | [out] Revision of the current file
  aafFileRev_t *  rev
 )
{
  *rev = kAAFRev1;
  return(AAFRESULT_SUCCESS);
}


AAFRESULT STDMETHODCALLTYPE
ImplAAFFile::GetHeader (ImplAAFHeader ** header)
{
  if (! header)
	return AAFRESULT_NULL_PARAM;

  if (!IsOpen())
	return AAFRESULT_NOT_OPEN;

  *header = _head;

  // We are returning a copy of the reference counted object.
  if (_head)
	_head->AcquireReference();

  return(AAFRESULT_SUCCESS);
}

AAFRESULT STDMETHODCALLTYPE
    ImplAAFFile::GetDictionary (ImplAAFDictionary ** ppDictionary) const
{
	if (! ppDictionary)
	{
		return AAFRESULT_NULL_PARAM;
	}

	return(_head->GetDictionary(ppDictionary));
}

OMFile * ImplAAFFile::omFile (void)
{
  assert (IsOpen());
  assert (_file);
  return _file;
}


bool ImplAAFFile::IsReadable () const
{ return _isReadable; }

bool ImplAAFFile::IsWriteable () const
{ return _isWriteable; }

bool ImplAAFFile::IsOpen () const
{ return (0 != _isOpen); }

bool ImplAAFFile::IsClosed () const
{ return _isClosed; }

OMRawStorage * ImplAAFFile::RawStorage ()
{
  OMRawStorage * r = 0;
  if (_pOMStg)
	r = _pOMStg;
  else if (_file)
	r = _file->rawStorage ();
  assert (r);
  return r;
}
