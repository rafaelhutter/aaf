//=---------------------------------------------------------------------=
//
// $Id: TestIndirect.cpp,v 1.3 2007/08/07 17:19:31 philipn Exp $ $Name:  $
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
// The Original Code of this file is Copyright 1998-2007, Licensor of the
// AAF Association.
//
// The Initial Developer of the Original Code of this file and the
// Licensor of the AAF Association is British Broadcasting Corporation.
// All rights reserved.
//
//=---------------------------------------------------------------------=


#include "XMLStoredFormatTest.h"


static bool testRestore(const wchar_t* fileName)
{
    bool passed = true;
    
    IAAFFile* pFile = 0;
    IAAFHeader* pHeader = 0;
    IAAFDictionary* pDictionary = 0;
    IAAFContentStorage* pStorage = 0;
    IEnumAAFMobs* pMobs = 0;
    IAAFMob* pMob = 0;
    
    try
    {
        pFile = openFileForReading(fileName);
    }
    catch (...)
    {
        return false;
    }
    
    try
    {
        // get the Mob containing the test data
        checkResult(pFile->GetHeader(&pHeader));
        checkResult(pHeader->GetDictionary(&pDictionary));
        checkResult(pHeader->GetContentStorage(&pStorage));
        aafSearchCrit_t searchCrit;
        searchCrit.searchTag = kAAFByMobKind;
        searchCrit.tags.mobKind = kAAFAllMob;
        checkResult(pStorage->GetMobs(&searchCrit, &pMobs));
        checkResult(pMobs->NextOne(&pMob));
        
        IAAFObject* pObject = 0;
        IAAFClassDef* pClassDef = 0;
        IAAFPropertyDef* pPropertyDef = 0;
        IAAFPropertyValue* pPropertyValue = 0;
        IAAFTypeDef* pType = 0;
        IAAFTypeDefIndirect* pIndirectType = 0;
        IAAFPropertyValue* pActualPropertyValue = 0;
        IAAFTypeDef* pActualType = 0;
        IAAFTypeDefInt* pIntType = 0;
        IAAFTypeDefEnum* pEnumType = 0;
        IAAFTypeDefRecord* pRecordType = 0;

        try
        {
            printf("    * Baseline: ");

            const aafUID_t propId = 
                {0x00000000,0x0000,0x0000,{0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
            aafInt8 testValue = -128;
            
            checkResult(pMob->QueryInterface(IID_IAAFObject, (void **)&pObject));
            
            checkResult(pDictionary->LookupClassDef(kAAFClassID_Mob, &pClassDef));
            checkResult(pClassDef->LookupPropertyDef(propId, &pPropertyDef));
            
            checkResult(pObject->GetPropertyValue(pPropertyDef, &pPropertyValue));
            checkResult(pPropertyValue->GetType(&pType));
            checkResult(pType->QueryInterface(IID_IAAFTypeDefIndirect, (void **)&pIndirectType));
            
            checkResult(pIndirectType->GetActualValue(pPropertyValue, &pActualPropertyValue));
            checkResult(pActualPropertyValue->GetType(&pActualType));
            checkResult(pActualType->QueryInterface(IID_IAAFTypeDefInt, (void **)&pIntType));
            
            aafInt8 value;
            checkResult(pIntType->GetInteger(pActualPropertyValue, (aafUInt8*)&value, 1));

            if (value == testValue)
            {
                printf("passed\n");
            }
            else
            {
                printf("FAILED\n");
                passed = false;
            }
        }
        catch (...)
        {
            printf("FAILED\n");
            passed = false;
        }
        release(pRecordType);
        release(pEnumType);
        release(pIntType);
        release(pActualType);
        release(pActualPropertyValue);
        release(pIndirectType);
        release(pType);
        release(pPropertyValue);
        release(pPropertyDef);
        release(pClassDef);
        release(pObject);
        
        try
        {
            printf("    * Non-baseline: ");

            const aafUID_t propId = 
                {0x10000000,0x0000,0x0000,{0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
            aafUInt8 testValue = 1;
            const wchar_t* testName = L"AAA";
            
            checkResult(pMob->QueryInterface(IID_IAAFObject, (void **)&pObject));
            
            checkResult(pDictionary->LookupClassDef(kAAFClassID_Mob, &pClassDef));
            checkResult(pClassDef->LookupPropertyDef(propId, &pPropertyDef));
            
            checkResult(pObject->GetPropertyValue(pPropertyDef, &pPropertyValue));
            checkResult(pPropertyValue->GetType(&pType));
            checkResult(pType->QueryInterface(IID_IAAFTypeDefIndirect, (void **)&pIndirectType));
            
            checkResult(pIndirectType->GetActualValue(pPropertyValue, &pActualPropertyValue));
            checkResult(pActualPropertyValue->GetType(&pActualType));
            checkResult(pActualType->QueryInterface(IID_IAAFTypeDefEnum, (void **)&pEnumType));
            
            aafInt64 value;
            aafCharacter name[256];
            checkResult(pEnumType->GetIntegerValue(pActualPropertyValue, &value));
            checkResult(pEnumType->GetNameFromValue(pActualPropertyValue, name, 256));

            if (value == testValue && wcscmp(name, testName) == 0)
            {
                printf("passed\n");
            }
            else
            {
                printf("FAILED\n");
                passed = false;
            }
        }
        catch (...)
        {
            printf("FAILED\n");
            passed = false;
        }
        release(pRecordType);
        release(pEnumType);
        release(pIntType);
        release(pActualType);
        release(pActualPropertyValue);
        release(pIndirectType);
        release(pType);
        release(pPropertyValue);
        release(pPropertyDef);
        release(pClassDef);
        release(pObject);
        
        try
        {
            printf("    * Baseline record: ");

            const aafUID_t propId = 
                {0x20000000,0x0000,0x0000,{0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
            aafProductVersion_t testValue = {0,1,0,0,kAAFVersionUnknown};
            
            checkResult(pMob->QueryInterface(IID_IAAFObject, (void **)&pObject));
            
            checkResult(pDictionary->LookupClassDef(kAAFClassID_Mob, &pClassDef));
            checkResult(pClassDef->LookupPropertyDef(propId, &pPropertyDef));
            
            checkResult(pObject->GetPropertyValue(pPropertyDef, &pPropertyValue));
            checkResult(pPropertyValue->GetType(&pType));
            checkResult(pType->QueryInterface(IID_IAAFTypeDefIndirect, (void **)&pIndirectType));

            checkResult(pIndirectType->GetActualValue(pPropertyValue, &pActualPropertyValue));
            checkResult(pActualPropertyValue->GetType(&pActualType));
            checkResult(pActualType->QueryInterface(IID_IAAFTypeDefRecord, (void **)&pRecordType));
            
            aafProductVersion_t value;
            checkResult(pRecordType->GetStruct(pActualPropertyValue, (aafUInt8*)&value, sizeof(aafProductVersion_t)));

            if (memcmp(&testValue, &value, sizeof(aafProductVersion_t)) == 0)
            {
                printf("passed\n");
            }
            else
            {
                printf("FAILED\n");
                passed = false;
            }
        }
        catch (...)
        {
            printf("FAILED\n");
            passed = false;
        }
        release(pRecordType);
        release(pEnumType);
        release(pIntType);
        release(pActualType);
        release(pActualPropertyValue);
        release(pIndirectType);
        release(pType);
        release(pPropertyValue);
        release(pPropertyDef);
        release(pClassDef);
        release(pObject);
        
    }
    catch (...)
    {
        passed = false;
    }

    // cleanup    
    release(pMob);
    release(pMobs);
    release(pStorage);
    release(pDictionary);
    release(pHeader);
    checkResult(pFile->Close());
    release(pFile);

    report(passed);
    
    return passed;
}


int testIndirect()
{
    printf("Rule 5.5: Indirect\n");
    
    bool passed = true;
    try
    {
        printf("a) Restore\n");
        passed = testRestore(L"input/testIndirect.xml") && passed;
        
        printf("b) Roundtrip\n");
        printf("FAILED (not tested): Type referenced in Indirect type value and no where else is not cloned - fix due for v1.2\n");
        //saveCopy(L"input/testIndirect.xml", L"tmp.xml");
        //passed = testRestore(L"tmp.xml") && passed;

        printf("c) Modify\n");
        copyAndTouch(L"input/testIndirect.xml", L"tmp.xml");
        passed = testRestore(L"tmp.xml") && passed;
    }
    catch (...)
    {
        passed = false;
        printf("FAILED: Exception thrown\n");
    }

    printf("\n");
    
    return passed ? 1 : 2; // 1 -> known failures
}


