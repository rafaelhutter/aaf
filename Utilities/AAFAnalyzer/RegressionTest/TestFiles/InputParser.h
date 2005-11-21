//=---------------------------------------------------------------------=
//
// $Id$
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
//=---------------------------------------------------------------------=

#ifndef __INPUTPARSER_H_
#define __INPUTPARSER_H_

//Regression Test Files files
#include "TestFileBuilder.h"

//Ax files
#include <AxTypes.h>

//Boost files
#include <boost/shared_ptr.hpp>

//STL files
#include <stack>
#include <map>

class AxObject;
class AxMob;
class AxComponent;
class AxSegment;

namespace aafanalyzer {

class SlotInfo;

using namespace std;
using namespace boost;

class InputParser
{
public:

    InputParser( const char* outFile );
    ~InputParser();
    
    typedef pair<const AxString, const bool> OptionalStringAttrib;
    typedef pair<const int, const bool> OptionalIntAttrib;

    static void __StartElement(void* userData, const char* name, const char** atts);
    static void __EndElement(void* userData, const char* name);
    static void __EndData(void* userData, const char* s, int len);

    void StartElement(const AxString& name, const char** attribs);
    void EndElement(const AxString& name);
    void EndData(const AxString& contents);
    
    void ParseXML( const char* filename ) const;
    
 private:
 
    typedef shared_ptr<AxMob>(TestFileBuilder::*ptrToAddFunction)( const AxString& name, bool isNamed, aafRational_t rationalParam );
    typedef shared_ptr<AxComponent>(TestFileBuilder::*ptrToCreateFunction)( TestFileBuilder::TrackType essenceType, const aafUID_t& uidParam );
    typedef void(TestFileBuilder::*ptrToAttachSlotFunction)( AxMob& parent, AxSegment& axSegment, aafRational_t editRate, const AxString& name, bool isNamed, int physicalTrackNum, bool isNumbered );
    typedef void(TestFileBuilder::*ptrToAttachParameterFunction)( AxOperationGroup& axOpGroup, const aafUID_t& paramDefId, aafUInt32 intParam1, aafUInt32 intParam2 );

    //Components can be divided into 4 distinct classes based upon the types of
    //objects that may be attached.  Each class has its own signature for attaching:
    
    //Type A (No objects can be attached)
    //Filler, Event, GPITrigger, Timecode, TimecodeStream, TimecodeStream12M, Edgecode, ScopeReference
    //NOTE: No fill function is needed because no objects are attached.
    //NOTE: Event and TimecodeStream are abstract classes, no special
    //      if functions are needed to create descendant classes, they will be
    //      no different from a normal create function.  Fill functions for
    //      these descendant classes will be classified as needed, but, in many
    //      cases will be Type A.
    
    //Type B (One Mob Referenced)
    //SourceReference, SourceClip
    typedef void(TestFileBuilder::*ptrToFillFunctionB) ( shared_ptr<AxSegment> axSegment, AxMob& axMob );
    //NOTE: SourceReference is an abstract class.  Although the general create
    //      function will be sufficient to create new descendant classes, a
    //      specialized fill function will be needed.  In many cases, that
    //      fill function will be classified as Type B, therefore, a map is
    //      still being used although at the present it contains only one
    //      element.
    
    //Type C (Interchange Objects can be attached (one or many in an array) )
    //Transition, Sequence, CommentMarker, DescriptiveMarker, Pulldown
    typedef void(TestFileBuilder::*ptrToFillFunctionC) ( shared_ptr<AxObject> axObject, AxObject& axObj );
    
    //Type D (Multiple InterchangeObjects assigned to different properties)
    //OperationGroup, Selector, EssenceGroups
    typedef void(TestFileBuilder::*ptrToFillFunctionD) ( shared_ptr<AxSegment> axSegment, AxObject& axObj, int property );

    typedef pair<AxString, shared_ptr<AxComponent> > ComponentPair;

    stack<shared_ptr<AxMob> > _mobStack;
    stack<ComponentPair> _componentStack;
    stack<SlotInfo> _slotStack;

    map<AxString, ptrToAddFunction> _materialTypeMap;
    map<AxString, ptrToCreateFunction> _createSegmentMap;
    map<AxString, ptrToAttachSlotFunction> _attachSlotMap;

    map<AxString, ptrToFillFunctionB> _fillSegmentMapB;
    map<AxString, ptrToFillFunctionC> _fillSegmentMapC;
    map<AxString, ptrToFillFunctionD> _fillSegmentMapD;
    
    map<AxString, ptrToAttachParameterFunction> _attachParameterMap;

    map<AxString, TestFileBuilder::TrackType> _essenceMap;
    map<AxString, AxString> _optRationalParam;
    map<AxString, aafUID_t> _effectMap;
    map<AxString, aafUID_t> _parameterTypeMap;
    
    TestFileBuilder _testFile;
    
    const AxString GetStringAttribValue( const AxString& attrib, const char** attribs, const unsigned int size, const AxString& default_val ) const;
    const int GetIntAtribValue( const AxString& attrib, const char** attribs, const unsigned int size, const int default_val ) const;
    const int GetBoolAtribValue( const AxString& attrib, const char** attribs, const unsigned int size, const int default_val ) const;
    const OptionalStringAttrib GetOptionalStringAttribValue( const AxString& attrib, const char** attribs, const unsigned int size, const AxString& default_val ) const;
    const OptionalIntAttrib GetOptionalIntAttribValue( const AxString& attrib, const char** attribs, const unsigned int size, const int default_val ) const;

};

} // end of namespace diskstream

#endif /*__INPUTPARSER_H_*/
