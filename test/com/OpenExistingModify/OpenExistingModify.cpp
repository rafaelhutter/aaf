// @doc INTERNAL
// @com This file implements tests for variour file kinds
//=---------------------------------------------------------------------=
//
// $Id: OpenExistingModify.cpp,v 1.1 2005/02/03 18:43:22 stuart_hc Exp $ $Name:  $
//
// The contents of this file are subject to the AAF SDK Public
// Source License Agreement (the "License"); You may not use this file
// except in compliance with the License.  The License is available in
// AAFSDKPSL.TXT, or you may obtain a copy of the License from the AAF
// Association or its successor.
//
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See
// the License for the specific language governing rights and limitations
// under the License.
//
// The Original Code of this file is Copyright 1998-2004, Licensor of the
// AAF Association.
//
// The Initial Developer of the Original Code of this file and the
// Licensor of the AAF Association is Avid Technology.
// All rights reserved.
//
//=---------------------------------------------------------------------=

#include "AAF.h"

#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include "AAFStoredObjectIDs.h"
#include "AAFResult.h"
#include "AAFDefUIDs.h"
#include "AAFFileMode.h"
#include "AAFFileKinds.h"

#include "CAAFBuiltinDefs.h"

using namespace std;

static void RemoveTestFile(const aafWChar* pFileName)
{
  char cFileName[FILENAME_MAX];

  size_t status = wcstombs(cFileName, pFileName, FILENAME_MAX);
  if (status != (size_t)-1) {
    remove(cFileName);
  }
}

inline void checkResult(HRESULT r)
{
  if (FAILED(r))
    throw r;
}


inline void checkExpression(bool expression, HRESULT r=AAFRESULT_TEST_FAILED)
{
  if (!expression)
    throw r;
}

#define CR(x) checkResult(x)

static const   aafMobID_t  TEST_MobID = {{0x06, 0x0c, 0x2b, 0x34, 0x02, 0x05, 0x11, 0x01, 0x01, 0x00, 0x10, 0x00}, 0x13, 0x00, 0x00, 0x00, {0xfd3cc302, 0x03fe, 0x11d4, {0x8e, 0x3d, 0x00, 0x90, 0x27, 0xdf, 0xca, 0x7c}}};
static const   aafMobID_t  TEST_SourceMobID = {{0x06, 0x0c, 0x2b, 0x34, 0x02, 0x05, 0x11, 0x01, 0x01, 0x00, 0x10, 0x00}, 0x13, 0x00, 0x00, 0x00, {0xfd3cc402, 0x03fe, 0x11d4, {0x8e, 0x3d, 0x00, 0x90, 0x27, 0xdf, 0xca, 0x7c}}};

static aafProductVersion_t TestVersion = { 1, 1, 0, 0, kAAFVersionUnknown };
static aafProductIdentification_t TestProductID = {
	L"AAF Developers Desk",			// companyName
	L"OpenExistingModify",			// productName
	NULL,							// productVersionString
	UnitTestProductID,				// productID
	NULL,							// platform
	&TestVersion					// productVersion
	};

#if 0
static HRESULT ReadAAFContents(IAAFFile *pFile)
{
	IAAFHeader* pHeader = NULL;
	IEnumAAFMobs* mobIter = NULL;
	IAAFMob* pMob = NULL;
	aafNumSlots_t numMobs, n;
	aafWChar name[500];
	aafMobID_t mobID;
	aafFileRev_t testRev;
	HRESULT hr = S_OK;

	try
	{
		// Get the header
		checkResult(pFile->GetHeader(&pHeader));

		// Expect to find a single Mob
		checkResult(pHeader->CountMobs(kAAFAllMob, &numMobs));
		checkExpression (1 == numMobs, AAFRESULT_TEST_FAILED);

		checkResult(pHeader->GetMobs (NULL, &mobIter));
		for(n = 0; n < numMobs; n++)
		{
			checkResult(mobIter->NextOne (&pMob));
			checkResult(pMob->GetName (name, sizeof(name)));
			checkResult(pMob->GetMobID (&mobID));
			// Check that the properties are as we wrote them
			checkExpression(wcscmp( name, MOB_NAME_TEST) == 0,
                      AAFRESULT_TEST_FAILED);
			checkExpression(memcmp(&mobID, &TEST_MobID, sizeof(mobID)) == 0,
                      AAFRESULT_TEST_FAILED);

			pMob->Release();
			pMob = NULL;
    	}

		mobIter->Release();
		mobIter = NULL;

		checkResult(pFile->GetRevision(&testRev));
		checkExpression(kAAFRev2 == testRev, AAFRESULT_TEST_FAILED);

		checkResult(pFile->Close());
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
		cout << "*** ReadAAFContents: caught error hr=0x" << hex << hr << dec << endl;
	}

	// Clean up
	if (mobIter)
		mobIter->Release();

	if (pMob)
		pMob->Release();

	if (pHeader)
		pHeader->Release();

	printf("   ok   ");
	return hr;
}

static HRESULT OpenAAFFile(const aafUID_t *written_kind, aafWChar * pFileName)
{
	IAAFFile* pFile = NULL;
	HRESULT hr = S_OK;

	try
	{
		for (int i = 0; read_api[i].api != APIunknown; i++)
		{
			// Determine the method to use to open the file
			if (read_api[i].api == FileOpenExistingRead)
			{
				checkResult(AAFFileOpenExistingRead(pFileName, 0, &pFile));

				checkResult(ReadAAFContents(pFile));
			}
			else if (read_api[i].api == FileOpenExistingModify)
			{
				checkResult(AAFFileOpenExistingModify(pFileName, 0, &TestProductID, &pFile));

				checkResult(ReadAAFContents(pFile));
			}
			else if (read_api[i].api == RawStorage)
			{
				for (int j = 0; filekind[j].kind != NULL; j++)
				{
					HRESULT r = S_OK;
	
					IAAFRawStorage* pRawStorage = 0;
					checkResult(AAFCreateRawStorageDisk(
									pFileName,
									kAAFFileExistence_existing,
									kAAFFileAccess_read,
									&pRawStorage));
					r = AAFCreateAAFFileOnRawStorage (
									pRawStorage,
									kAAFFileExistence_existing,
									kAAFFileAccess_read,
									filekind[j].kind,
									0,
									0,
									&pFile);
					pRawStorage->Release();
					if (r == AAFRESULT_FILEKIND_NOT_REGISTERED)
					{
						printf("    x   ");
						continue;
					}

					// For RawStorage some combinations are designed not to work
					// such as reading a 4K file using a 512 implementation
					// or reading a 512 file using a 4K implementation.
					if ((filekind[j].kind == &kAAFFileKind_Aaf512Binary ||
						filekind[j].kind == &kAAFFileKind_AafS512Binary ||
						filekind[j].kind == &kAAFFileKind_AafM512Binary ||
						filekind[j].kind == &kAAFFileKind_AafG512Binary) &&
							(
							written_kind == &kAAFFileKind_Aaf4KBinary ||
							written_kind == &kAAFFileKind_AafS4KBinary ||
							written_kind == &kAAFFileKind_AafM4KBinary ||
							written_kind == &kAAFFileKind_AafG4KBinary))
					{
						printf("   ic   ");
						continue;
					}
					if ((filekind[j].kind == &kAAFFileKind_Aaf4KBinary ||
						filekind[j].kind == &kAAFFileKind_AafS4KBinary ||
						filekind[j].kind == &kAAFFileKind_AafM4KBinary ||
						filekind[j].kind == &kAAFFileKind_AafG4KBinary) &&
							(written_kind == NULL ||	// NULL (meaning NM default) is 512
							written_kind == &kAAFFileKind_Aaf512Binary ||
							written_kind == &kAAFFileKind_AafS512Binary ||
							written_kind == &kAAFFileKind_AafM512Binary ||
							written_kind == &kAAFFileKind_AafG512Binary))
					{
						printf("   ic   ");
						continue;
					}

					checkResult(pFile->Open());

					checkResult(ReadAAFContents(pFile));
				}
			}
			else
			{
				printf(" *** Did not find valid api to open file %ls i=%d\n", pFileName, i);
				checkExpression(false, AAFRESULT_TEST_FAILED);
			}

		}
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
		cout << "*** OpenAAFFile: caught error hr=0x" << hex << hr << dec << endl;
	}
	return hr;
}
#endif

static HRESULT CreateAAFFile(aafWChar *filename)
{
	HRESULT			hr = S_OK;

	try
	{
		RemoveTestFile(filename);

		// Open new file
		IAAFFile		*pFile = NULL;
		TestProductID.productVersionString = L"CreateAAFFile";
		checkResult( AAFFileOpenNewModifyEx(
						filename,
						&kAAFFileKind_Aaf4KBinary,
						0,
						&TestProductID,
						&pFile) );

		// Get the header & dictionary
		IAAFHeader		*pHeader = NULL;
		IAAFDictionary	*pDictionary = NULL;
		checkResult(pFile->GetHeader(&pHeader));
		checkResult(pHeader->GetDictionary(&pDictionary));
		CAAFBuiltinDefs defs(pDictionary);

		// Create a MasterMob
		IAAFMob			*pMob = NULL;
		checkResult(defs.cdMasterMob()->CreateInstance(IID_IAAFMob, (IUnknown **)&pMob));
		checkResult(pMob->SetMobID(TEST_MobID));
		checkResult(pMob->SetName(L"CreateAAFFile - MasterMob"));
		checkResult(pHeader->AddMob(pMob));
		pMob->Release();

		// Create a SourceMob 
		IAAFSourceMob			*pSourceMob = NULL;
		checkResult(defs.cdSourceMob()->CreateInstance(IID_IAAFSourceMob, (IUnknown **)&pSourceMob));
		checkResult(pSourceMob->QueryInterface(IID_IAAFMob, (void **)&pMob));
		checkResult(pMob->SetMobID(TEST_SourceMobID));
		checkResult(pMob->SetName(L"CreateAAFFile - SourceMob"));

		IAAFEssenceDescriptor	*edesc = NULL;
		IAAFAIFCDescriptor		*pAIFCDesc = NULL;
		checkResult(defs.cdAIFCDescriptor()->CreateInstance(IID_IAAFEssenceDescriptor, (IUnknown **)&edesc));
		checkResult(edesc->QueryInterface(IID_IAAFAIFCDescriptor, (void **)&pAIFCDesc));
		aafUInt8	buf[] = {0x00};
		checkResult(pAIFCDesc->SetSummary(sizeof(buf), buf));
		checkResult(pSourceMob->SetEssenceDescriptor(edesc));
		checkResult(pHeader->AddMob(pMob));
		pAIFCDesc->Release();
		edesc->Release();
		pSourceMob->Release();
		pMob->Release();

		pDictionary->Release();
		pHeader->Release();

		// Save & close the file
		checkResult(pFile->Save());
		checkResult(pFile->Close());
		checkResult(pFile->Release());

		cout << "CreateAAFFile() - created new file" << endl;
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
		cout << "*** CreateAAFFile: caught error hr=0x" << hex << hr << dec << endl;
	}

	return hr;
}

static HRESULT ModifyAAFFile(aafWChar *filename, int level)
{
	HRESULT			hr = S_OK;

	try
	{
		// Open existing file for modification
		IAAFFile		*pFile = NULL;
		TestProductID.productVersionString = L"ModifyAAFFile";
		checkResult( AAFFileOpenExistingModify(
						filename,
						0,					// modeFlags
						&TestProductID,
						&pFile) );

		cout << "ModifyAAFFile() - appended Identification" << endl;

		// Get the header & dictionary
		IAAFHeader		*pHeader = NULL;
		IAAFDictionary	*pDictionary = NULL;
		checkResult(pFile->GetHeader(&pHeader));
		checkResult(pHeader->GetDictionary(&pDictionary));
		CAAFBuiltinDefs defs(pDictionary);

		// Search for Mobs
		IAAFMob			*pFileMob = NULL;
		IEnumAAFMobs	*pFileMobIter = NULL;
		aafSearchCrit_t				criteria;
		criteria.searchTag = kAAFByMobKind;
		criteria.tags.mobKind = kAAFFileMob;		// Search by File Mob
	
		checkResult(pHeader->GetMobs(&criteria, &pFileMobIter));
	
		while (AAFRESULT_SUCCESS == pFileMobIter->NextOne(&pFileMob))
		{
			if (level == 0)
				break;

			IAAFEssenceDescriptor	*edesc = NULL;
			IAAFSourceMob			*pSourceMob = NULL;
			CR(pFileMob->QueryInterface(IID_IAAFSourceMob, (void **)&pSourceMob));
			CR(pSourceMob->GetEssenceDescriptor(&edesc));

			// Change the Name property
			CR(pFileMob->SetName(L"ModifyAAFFile - modified Name"));
			cout << "ModifyAAFFile() - changed FileMob's Name property" << endl;

			if (level == 1)
				break;

			// Change descriptor's properties
			IAAFAIFCDescriptor		*pAIFCDesc = NULL;
			CR(edesc->QueryInterface(IID_IAAFAIFCDescriptor, (void **)&pAIFCDesc));
			aafUInt8				AIFCsum[] = {0xa1,0xfc};
			CR(pAIFCDesc->SetSummary(sizeof(AIFCsum), AIFCsum));
			pAIFCDesc->Release();
			edesc->Release();
			cout << "ModifyAAFFile() - changed AIFCDescriptor's Summary" << endl;

			if (level == 2)
				break;

			// Change descriptor to new one (overwriting old one)
			IAAFFileDescriptor		*pFileDesc = NULL;
			IAAFWAVEDescriptor		*pWAVEDesc = NULL;
			IAAFEssenceDescriptor	*pNewEdesc = NULL;

			CR(defs.cdWAVEDescriptor()->CreateInstance(
					IID_IAAFFileDescriptor, (IUnknown **)&pFileDesc));
			CR(pFileDesc->QueryInterface(IID_IAAFWAVEDescriptor, (void **)&pWAVEDesc));
			CR(pFileDesc->QueryInterface(IID_IAAFEssenceDescriptor, (void **)&pNewEdesc));
			aafUInt8				WAVEsum[] = {0x1a,0x1e,0xee,0xee};
			CR(pWAVEDesc->SetSummary(sizeof(WAVEsum), WAVEsum));
			CR(pSourceMob->SetEssenceDescriptor(pNewEdesc));
			pNewEdesc->Release();
			pWAVEDesc->Release();
			pFileDesc->Release();
			cout << "ModifyAAFFile() - replaced AIFCDescriptor with WAVEDescriptor" << endl;

			if (level == 3)
				break;

			// Add EssenceData
			IAAFEssenceData			*pEssenceData = NULL;
			aafUInt32				bytesWritten = 0;
			aafUInt8				essdata[] = "Zaphod Beeblebrox";
			CR(defs.cdEssenceData()->CreateInstance(
					IID_IAAFEssenceData, (IUnknown **)&pEssenceData));
			CR(pEssenceData->SetFileMob(pSourceMob));
			CR(pHeader->AddEssenceData(pEssenceData));
			CR(pEssenceData->Write(sizeof(essdata), essdata, &bytesWritten));
			pEssenceData->Release();
			cout << "ModifyAAFFile() - added EssenceData" << endl;


			pSourceMob->Release();
		}
		pFileMob->Release();
		pFileMobIter->Release();

		pDictionary->Release();
		pHeader->Release();

		// Save & close the file
		checkResult(pFile->Save());
		checkResult(pFile->Close());
		checkResult(pFile->Release());
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
		cout << "*** ModifyAAFFile: caught error hr=0x" << hex << hr << dec << endl;
	}

	return hr;
}

static HRESULT ReadAAFFile(aafWChar *filename)
{
	HRESULT			hr = S_OK;

	try
	{
	}
	catch (HRESULT& rResult)
	{
		hr = rResult;
		cout << "*** ReadAAFFile: caught error hr=0x" << hex << hr << dec << endl;
	}

	return hr;
}

// Usage
//
// OpenExistingModify [-l level] [input.aaf]
//
//		-l   "level" of modification to perform - lower values perform fewer changes
//

void printUsage(const char *progname)
{
	fprintf(stderr, "Usage:\n\t%s [options] [file_to_modify.aaf]\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "\tWhen no file argument is provided a test file will be written first\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "\t-l   \"level\" of modification to perform (default is all levels)\n");
	fprintf(stderr, "\t      0 - open then close, causing Identification to be appended\n");
	fprintf(stderr, "\t      1 - change Mob's Name property\n");
	fprintf(stderr, "\t      2 - change AIFCDescriptor's Summary property\n");
	fprintf(stderr, "\t      3 - replace AIFCDescriptor with WAVEDescriptor\n");
	fprintf(stderr, "\t      4 - add EssenceData\n");
}

int main(int argc, char* argv[])
{
	aafWChar		filename[FILENAME_MAX] = L"openExistingModify_test.aaf";
	int				level = INT_MAX;
	bool			create_input_file = true;

	int i = 1;
	if (argc > 1)
	{
		while (i < argc)
		{
			if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
			{
				printUsage(argv[0]);
				return 0;
			}
			else if (!strcmp(argv[i], "-l"))
			{
				
				level = atoi(argv[i+1]);
				i += 2;
			}
			else
			{
				break;
			}
		}
	}

	if (argc - i == 1)			// one arg left - must be filename
	{
		if ((mbstowcs(filename, argv[i], sizeof(filename))) == (size_t)(-1))
		{
			fprintf(stderr, "%s: mbstowcs conversion failed\n", argv[i]);
			return 1;
		}
		create_input_file = false;
	}
	else if ( argc - i > 1)
	{
		printUsage(argv[0]);
		return 1;
	}

	try
	{
		if (create_input_file)
		{
			checkResult(CreateAAFFile(filename));

			checkResult(ModifyAAFFile(filename, level));

			checkResult(ReadAAFFile(filename));
		}
		else
		{
			// So far, only appending Identification is supported
			// for arbitrary files
			checkResult(ModifyAAFFile(filename, 0));
		}
	}
	catch (HRESULT& r)
	{
		cerr << "[main] Error : Caught HRESULT 0x" << hex << r << dec << endl;
		return 1;
	}
	return 0;
}
