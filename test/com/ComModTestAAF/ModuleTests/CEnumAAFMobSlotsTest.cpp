// @doc INTERNAL
// @com This file implements the module test for CEnumAAFMobSlots
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


#include "AAF.h"

#include <iostream.h>
#include <stdio.h>

#include "AAFStoredObjectIDs.h"
#include "AAFResult.h"
#include "AAFDefUIDs.h"

#include "CAAFBuiltinDefs.h"

static aafWChar *slotNames[5] = { L"SLOT1", L"SLOT2", L"SLOT3", L"SLOT4", L"SLOT5" };


// Cross-platform utility to delete a file.
static void RemoveTestFile(const wchar_t* pFileName)
{
  const size_t kMaxFileName = 512;
  char cFileName[kMaxFileName];

  size_t status = wcstombs(cFileName, pFileName, kMaxFileName);
  if (status != (size_t)-1)
  { // delete the file.
    remove(cFileName);
  }
}

// convenient error handlers.
inline void checkResult(HRESULT r)
{
  if (FAILED(r))
    throw r;
}
inline void checkExpression(bool expression, HRESULT r)
{
  if (!expression)
    throw r;
}

static HRESULT CreateAAFFile(aafWChar * pFileName)
{
	IAAFFile *					pFile = NULL;
  bool bFileOpen = false;
  IAAFHeader *        pHeader = NULL;
  IAAFDictionary*  pDictionary = NULL;
  IAAFMob						*pMob = NULL;
  IAAFTimelineMobSlot	*newSlot = NULL;
  IAAFSegment		*seg = NULL;
  IAAFSourceClip	*sclp = NULL;
  IAAFComponent*		pComponent = NULL;
  aafProductIdentification_t	ProductInfo;
  aafMobID_t					newMobID;
  HRESULT						hr = S_OK;

  aafProductVersion_t v;
  v.major = 1;
  v.minor = 0;
  v.tertiary = 0;
  v.patchLevel = 0;
  v.type = kAAFVersionUnknown;
  ProductInfo.companyName = L"AAF Developers Desk";
  ProductInfo.productName = L"AAFMob Test";
  ProductInfo.productVersion = &v;
  ProductInfo.productVersionString = NULL;
  ProductInfo.productID = UnitTestProductID;
  ProductInfo.platform = NULL;


  try
  {
    // Remove the previous test file if any.
    RemoveTestFile(pFileName);

    // Create the file.
		checkResult(AAFFileOpenNewModify(pFileName, 0, &ProductInfo, &pFile));
		bFileOpen = true;
 
    // We can't really do anthing in AAF without the header.
		checkResult(pFile->GetHeader(&pHeader));

    // Get the AAF Dictionary so that we can create valid AAF objects.
    checkResult(pHeader->GetDictionary(&pDictionary));
	CAAFBuiltinDefs defs (pDictionary);
 		
  //Make the first mob
	  long	test;
	  aafRational_t	audioRate = { 44100, 1 };

	  // Create a Mob
	  checkResult(defs.cdMob()->
				  CreateInstance(IID_IAAFMob, 
								 (IUnknown **)&pMob));

		checkResult(CoCreateGuid((GUID *)&newMobID));
	  checkResult(pMob->SetMobID(newMobID));
	  checkResult(pMob->SetName(L"MOBTest"));
	  
	  // Add some slots
	  for(test = 0; test < 5; test++)
	  {
 		  checkResult(defs.cdSourceClip()->
					  CreateInstance(IID_IAAFSourceClip, 
									 (IUnknown **)&sclp));		
		 checkResult(sclp->QueryInterface(IID_IAAFComponent, (void **)&pComponent));
		 checkResult(pComponent->SetDataDef(defs.ddPicture()));
		pComponent->Release();
		pComponent = NULL;

		  checkResult(sclp->QueryInterface (IID_IAAFSegment, (void **)&seg));

		  aafRational_t editRate = { 0, 1};
		  checkResult(pMob->AppendNewTimelineSlot (editRate,
												   seg,
												   test+1,
												   slotNames[test],
												   0,
												   &newSlot));

		  newSlot->Release();
		  newSlot = NULL;

		  seg->Release();
		  seg = NULL;
		  
		  sclp->Release();
		  sclp = NULL;
	  }

    // Add the mob to the file.
	  checkResult(pHeader->AddMob(pMob));
  }
  catch (HRESULT& rResult)
  {
    hr = rResult;
  }


  // Cleanup and return
  if (newSlot)
    newSlot->Release();

	if (pComponent)
		pComponent->Release();

  if (seg)
    seg->Release();

  if (sclp)
    sclp->Release();

  if (pMob)
    pMob->Release();

  if (pDictionary)
    pDictionary->Release();

  if (pHeader)
    pHeader->Release();
      
  if (pFile)
  {  // Close file
    if (bFileOpen)
	  {
		  pFile->Save();
		  pFile->Close();
	  }
    pFile->Release();
  }

  return hr;
}

static HRESULT ReadAAFFile(aafWChar * pFileName)
{
	IAAFFile			*pFile = NULL;
	bool				bFileOpen = false;
	IAAFHeader			*pHeader = NULL;
	IEnumAAFMobs		*mobIter = NULL;
	IAAFMob				*aMob = NULL;
	IEnumAAFMobSlots	*slotIter = NULL;
	IAAFMobSlot			*slot = NULL;
	aafProductIdentification_t	ProductInfo;
	aafNumSlots_t		numMobs, n, s;
	HRESULT				hr = S_OK;
	IAAFMobSlot*		pArray[2] = { NULL, NULL };
	IAAFMobSlot**		pArrayDef = pArray;
	aafUInt32			resultCount;
	IEnumAAFMobSlots	*slotClone = NULL;
	
	aafProductVersion_t v;
	v.major = 1;
	v.minor = 0;
	v.tertiary = 0;
	v.patchLevel = 0;
	v.type = kAAFVersionUnknown;
	ProductInfo.companyName = L"AAF Developers Desk";
	ProductInfo.productName = L"EnumAAFMobSlots Test";
	ProductInfo.productVersion = &v;
	ProductInfo.productVersionString = NULL;
	ProductInfo.platform = NULL;
	
	try
	{
		// Open the file
		checkResult(AAFFileOpenExistingRead(pFileName, 0, &pFile));
		bFileOpen = true;
		
		// We can't really do anthing in AAF without the header.
		checkResult(pFile->GetHeader(&pHeader));
		
		
		checkResult(pHeader->CountMobs(kAAFAllMob, &numMobs));
		checkExpression(1 == numMobs, AAFRESULT_TEST_FAILED);
		
		
		aafSearchCrit_t		criteria;
		criteria.searchTag = kAAFNoSearch;
		checkResult(pHeader->GetMobs (&criteria, &mobIter));
		
		for(n = 0; n < numMobs; n++)
		{
			aafWChar		name[500], slotName[500];
			aafNumSlots_t	numSlots;
			aafMobID_t		mobID;
			aafSlotID_t		trackID;
			
			checkResult(mobIter->NextOne (&aMob));
			checkResult(aMob->GetName (name, sizeof(name)));
			checkResult(aMob->GetMobID (&mobID));
			
			checkResult(aMob->CountSlots (&numSlots));
			checkExpression(5 == numSlots, AAFRESULT_TEST_FAILED);
			
			checkResult(aMob->GetSlots(&slotIter));
			
			/* Read and check all elements using NextOne */
			for(s = 0; s < numSlots; s++)
			{
				checkResult(slotIter->NextOne (&slot));
				checkResult(slot->GetName (slotName, sizeof(slotName)));
				checkResult(slot->GetSlotID(&trackID));
				checkExpression (wcscmp(slotName, slotNames[s]) == 0, AAFRESULT_TEST_FAILED);
				
				slot->Release();
				slot = NULL;
			}
			/* Read one past to make sure that it fails */
			checkExpression(slotIter->NextOne(&slot) != AAFRESULT_SUCCESS, AAFRESULT_TEST_FAILED);
			/* Reset, and check the first element again*/
			checkResult(slotIter->Reset());
			checkResult(slotIter->NextOne (&slot));
			checkResult(slot->GetName (slotName, sizeof(slotName)));
			checkResult(slot->GetSlotID(&trackID));
			checkExpression (wcscmp(slotName, slotNames[0]) == 0, AAFRESULT_TEST_FAILED);
			slot->Release();
			slot = NULL;
			/* Reset, Skip, and check the second element again*/
			checkResult(slotIter->Reset());
			checkResult(slotIter->Skip(1));
			checkResult(slotIter->NextOne (&slot));
			checkResult(slot->GetName (slotName, sizeof(slotName)));
			checkResult(slot->GetSlotID(&trackID));
			checkExpression (wcscmp(slotName, slotNames[1]) == 0, AAFRESULT_TEST_FAILED);
			slot->Release();
			slot = NULL;
			
			/* Reset, and read both elements */
			checkResult(slotIter->Reset());
			checkResult(slotIter->Next (2, (IAAFMobSlot **)&pArray, &resultCount));
			checkExpression (resultCount == 2, AAFRESULT_TEST_FAILED);
			checkResult(pArrayDef[0]->GetName (slotName, sizeof(slotName)));
			checkResult(pArrayDef[0]->GetSlotID(&trackID));
			checkExpression (wcscmp(slotName, slotNames[0]) == 0, AAFRESULT_TEST_FAILED);
			checkResult(pArrayDef[1]->GetName (slotName, sizeof(slotName)));
			checkResult(pArrayDef[1]->GetSlotID(&trackID));
			checkExpression (wcscmp(slotName, slotNames[1]) == 0, AAFRESULT_TEST_FAILED);
			pArrayDef[0]->Release();
			pArrayDef[0] = NULL;
			pArrayDef[1]->Release();
			pArrayDef[1] = NULL;

			/* Clone the enumerator, and read one element */
			checkResult(slotIter->Clone(&slotClone));
			checkResult(slotClone->Reset());
			checkResult(slotClone->NextOne (&slot));
			checkResult(slot->GetName (slotName, sizeof(slotName)));
			checkResult(slot->GetSlotID(&trackID));
			checkExpression (wcscmp(slotName, slotNames[0]) == 0, AAFRESULT_TEST_FAILED);
			slot->Release();
			slot = NULL;
			slotIter->Release();
			slotIter = NULL;
			slotClone->Release();
			slotClone = NULL;

			aMob->Release();
			aMob = NULL;
		}
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
	}
	
	// Cleanup object references
	if (slot)
		slot->Release();
	
	if (slotIter)
		slotIter->Release();
	
	if (aMob)
		aMob->Release();
	
	if (mobIter)
		mobIter->Release();
	
	if (pHeader)
		pHeader->Release();
	
	if (pFile)
	{  // Close file
		if (bFileOpen)
			pFile->Close();
		pFile->Release();
	}
	
	return hr;
}
 

extern "C" HRESULT CEnumAAFMobSlots_test()
{
	HRESULT hr = AAFRESULT_NOT_IMPLEMENTED;
 	aafWChar * pFileName = L"EnumAAFMobSlotsTest.aaf";

	try
	{
		hr = CreateAAFFile(	pFileName );
		if(hr == AAFRESULT_SUCCESS)
			hr = ReadAAFFile( pFileName );
	}
	catch (...)
	{
	  cerr << "CEnumAAFMobSlots_test...Caught general C++"
		" exception!" << endl; 
	  hr = AAFRESULT_TEST_FAILED;
	}

	// Cleanup our object if it exists.

  	// When all of the functionality of this class is tested, we can return success
//	if (SUCCEEDED(hr))
//	{
//		cout << "The following IEnumAAFPluginDescriptor methods have not been tested:" << endl;       
//		cout << "     Next" << endl; 
//		cout << "     Clone" << endl; 
//		hr = AAFRESULT_TEST_PARTIAL_SUCCESS;
//	}

	return hr;
}
