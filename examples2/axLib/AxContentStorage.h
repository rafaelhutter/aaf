#ifndef __AxContentStorage_h__
#define __AxContentStorage_h__

//=---------------------------------------------------------------------=
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
// The Original Code of this file is Copyright 1998-2001, Licensor of the
// AAF Association.
// 
//=---------------------------------------------------------------------=

#include "AxTypes.h"
#include "AxSmartPointer.h"
#include "AxObject.h"

#include <iosfwd>

class AxContentStorage : public AxObject {

public:
	
	AxContentStorage( IAAFContentStorageSP spIaafContentStorage );
	virtual ~AxContentStorage();

	IAAFMobSP LookupMob( const aafMobID_t& );

	IEnumAAFMobsSP GetMobs(  const aafSearchCrit_t* pCriteria = 0 ) const;

	IEnumAAFEssenceDataSP EnumEssenceData() const;

	std::wostream& dump( std::wostream& os ) const;

private:

	AxContentStorage();
	AxContentStorage( const AxContentStorage& );
	AxContentStorage& operator=( const AxContentStorage& );

	mutable IAAFContentStorageSP _spIaafContentStorage;
};

std::wostream& operator<<( std::wostream& os, const AxContentStorage& obj );

#endif
