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

//Regression Test Files files
#include "InputParser.h"
#include "SlotInfo.h"

//Expat files
#include <expat.h>

//Ax files
#include <AxMob.h>
#include <AxComponent.h>

//AAF files
#include <AAFOperationDefs.h>
#include <AAFParameterDefs.h>

//STL files
#include <fstream>
#include <sstream>

namespace {

using namespace aafanalyzer;

} // end of namespace


//======================================================================
//======================================================================
//======================================================================

namespace aafanalyzer 
{

using namespace std;
using namespace boost;

InputParser::InputParser( const char* outFile )
    : _testFile( outFile )
{   
    
    /*
     *  Note: This class uses maps of function pointer to call the appropriate
     *       functions in the TestFileBuilder class.  The same could be
     *       accomplished using large if statements.  This method was chosen
     *       to reduce the average runtime (map search instead of linear if
     *       statement comparison) and to eliminate the need to maintain multiple
     *       if statements (only maps need to be maintained).
     * 
     * Also: The XML elements eof, oof, timecode, edgecode, aaf-file and bad-eoc
     *       are not contained in any of these maps.  This has been done because
     *       there are not a sufficient number of function (>2) with the same
     *       signature to warrant the map for the first 4 elements.  If more
     *       functions with the same signatures are implemented, maps may easily
     *       be added.  The last two elements are included in the DTD so XML
     *       test files will compile they do not have any effect on the file
     *       structure.
     * 
     */

    //Pointers to functions to add Mobs to an AAF file.
    _materialTypeMap[L"top-level"] = &TestFileBuilder::AddTopLevel;
    _materialTypeMap[L"lower-level"] = &TestFileBuilder::AddLowerLevel;
    _materialTypeMap[L"sub-clip"] = &TestFileBuilder::AddSubClip;
    _materialTypeMap[L"adjusted-clip"] = &TestFileBuilder::AddAdjustedClip;
    _materialTypeMap[L"template-clip"] = &TestFileBuilder::AddTemplateClip;
    _materialTypeMap[L"clip"] = &TestFileBuilder::AddClip;
    _materialTypeMap[L"file-source"] = &TestFileBuilder::AddFileSource;
    _materialTypeMap[L"recording-source"] = &TestFileBuilder::AddRecordingSource;
    _materialTypeMap[L"import-source"] = &TestFileBuilder::AddImportSource;
    _materialTypeMap[L"tape-source"] = &TestFileBuilder::AddTapeSource;
    _materialTypeMap[L"film-source"] = &TestFileBuilder::AddFilmSource;
    
    //Pointers to functions to create empty segments.
    _createSegmentMap[L"source-clip"] = &TestFileBuilder::CreateSourceClip;
    _createSegmentMap[L"timecode"] = &TestFileBuilder::CreateTimecode;
    _createSegmentMap[L"edgecode"] = &TestFileBuilder::CreateEdgecode;
    _createSegmentMap[L"eoc"] = &TestFileBuilder::CreateEOC;
    _createSegmentMap[L"oof"] = &TestFileBuilder::CreateOOF;
    _createSegmentMap[L"transition"] = &TestFileBuilder::CreateTransition;
    _createSegmentMap[L"sequence"] = &TestFileBuilder::CreateSequence;
    _createSegmentMap[L"video-dissolve"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"smpte-video-wipe"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-speed-control"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-repeat"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-flip"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-flop"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-flip-flop"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-position"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-crop"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-scale"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-rotate"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"video-corner-pinning"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"alpha-with-video-key"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"separate-alpha-key"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"luminance-key"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"chroma-key"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"mono-audio-gain"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"mono-audio-pan"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"mono-audio-dissolve"] = &TestFileBuilder::CreateOperationGroup;
    _createSegmentMap[L"two-parameter-mono-audio-dissolve"] = &TestFileBuilder::CreateOperationGroup;
    
    
    //Pointers to functions to fill segments.
    _fillSegmentMapB[L"source-clip"] = &TestFileBuilder::InitializeSourceClip;
    _fillSegmentMapC[L"transition"] = &TestFileBuilder::AddToTransition;
    _fillSegmentMapC[L"sequence"] = &TestFileBuilder::AddToSequence;
    _fillSegmentMapD[L"video-dissolve"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"smpte-video-wipe"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-speed-control"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-repeat"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-flip"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-flop"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-flip-flop"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-position"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-crop"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-scale"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-rotate"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"video-corner-pinning"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"alpha-with-video-key"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"separate-alpha-key"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"luminance-key"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"chroma-key"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"mono-audio-gain"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"mono-audio-pan"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"mono-audio-dissolve"] = &TestFileBuilder::AddToOperationGroup;
    _fillSegmentMapD[L"two-parameter-mono-audio-dissolve"] = &TestFileBuilder::AddToOperationGroup;
    
    //Pointers to functions to attach slots.
    _attachSlotMap[L"timeline-mob-slot"] = &TestFileBuilder::AttachTimelineSlot;
    _attachSlotMap[L"static-mob-slot"] = &TestFileBuilder::AttachStaticSlot;
    _attachSlotMap[L"event-mob-slot"] = &TestFileBuilder::AttachEventSlot;
    
    //Pointer to functions to attach parameters.
    _attachParameterMap[L"speed-ratio"] = &TestFileBuilder::AttachConstantRationalParameter;
    
    //Mapping from XML Essence Types to Track Types
    _essenceMap[L"none"] = TestFileBuilder::NONE;
    _essenceMap[L"audio"] = TestFileBuilder::AUDIO;
    _essenceMap[L"picture"] = TestFileBuilder::PICTURE;
    _essenceMap[L"timecode"] = TestFileBuilder::TIMECODE;
    _essenceMap[L"edgecode"] = TestFileBuilder::EDGECODE;
    
    //Set up optional rational parameter names
    _optRationalParam[L"top-level"] = L"";
    _optRationalParam[L"lower-level"] = L"";
    _optRationalParam[L"sub-clip"] = L"";
    _optRationalParam[L"adjusted-clip"] = L"";
    _optRationalParam[L"template-clip"] = L"";
    _optRationalParam[L"clip"] = L"";
    _optRationalParam[L"file-source"] = L"sample-rate";
    _optRationalParam[L"recording-source"] = L"";
    _optRationalParam[L"import-source"] = L"";
    _optRationalParam[L"tape-source"] = L"";
    _optRationalParam[L"film-source"] = L"";
    
    //Set up the effect parameters
    _effectMap[L"source-clip"] = kAAFOperationDef_Unknown;
    _effectMap[L"timecode"] = kAAFOperationDef_Unknown;
    _effectMap[L"edgecode"] = kAAFOperationDef_Unknown;
    _effectMap[L"eoc"] = kAAFOperationDef_Unknown;
    _effectMap[L"oof"] = kAAFOperationDef_Unknown;
    _effectMap[L"transition"] = kAAFOperationDef_Unknown;
    _effectMap[L"sequence"] = kAAFOperationDef_Unknown;

    _effectMap[L"video-dissolve"] = kAAFOperationDef_VideoDissolve;
    _effectMap[L"smpte-video-wipe"] = kAAFOperationDef_SMPTEVideoWipe;
    _effectMap[L"video-speed-control"] = kAAFOperationDef_VideoSpeedControl;
    _effectMap[L"video-repeat"] = kAAFOperationDef_VideoRepeat;
    _effectMap[L"video-flip"] = kAAFOperationDef_Flip;
    _effectMap[L"video-flop"] = kAAFOperationDef_Flop;
    _effectMap[L"video-flip-flop"] = kAAFOperationDef_FlipFlop;
    _effectMap[L"video-position"] = kAAFOperationDef_VideoPosition;
    _effectMap[L"video-crop"] = kAAFOperationDef_VideoCrop;
    _effectMap[L"video-scale"] = kAAFOperationDef_VideoScale;
    _effectMap[L"video-rotate"] = kAAFOperationDef_VideoRotate;
    _effectMap[L"video-corner-pinning"] = kAAFOperationDef_VideoCornerPinning;
    _effectMap[L"alpha-with-video-key"] = kAAFOperationDef_VideoAlphaWithinVideoKey;
    _effectMap[L"separate-alpha-key"] = kAAFOperationDef_VideoSeparateAlphaKey;
    _effectMap[L"luminance-key"] = kAAFOperationDef_VideoLuminanceKey;
    _effectMap[L"chroma-key"] = kAAFOperationDef_VideoChromaKey;
    _effectMap[L"mono-audio-gain"] = kAAFOperationDef_MonoAudioGain;
    _effectMap[L"mono-audio-pan"] = kAAFOperationDef_MonoAudioPan;
    _effectMap[L"mono-audio-dissolve"] = kAAFOperationDef_MonoAudioDissolve;
    _effectMap[L"two-parameter-mono-audio-dissolve"] = kAAFOperationDef_TwoParameterMonoAudioDissolve;
    
    //Set up the parameter parameters
    _parameterTypeMap[L"speed-ratio"] = kAAFParameterDef_SpeedRatio;

}

InputParser::~InputParser()
{}

void InputParser::ParseXML( const char* filename ) const
{
    
    //Variables needed to run the expat XML parser.
    const int BUF_SIZE = 256;
    int done;
    char buffer[BUF_SIZE] = "";
    
    //Create the XML parser
    XML_Parser parser = XML_ParserCreate(NULL);
 
    //Setup event handlers for the XML parser.
    try
    {
        XML_SetElementHandler( parser, InputParser::__StartElement, InputParser::__EndElement);
        XML_SetCharacterDataHandler(parser, InputParser::__EndData);
    }
    catch (...)
    {
        XML_ParserFree(parser);
        throw string("Could not set parser's handler functions.");
    }

    //Use this object as the data used by all handlers.
    XML_SetUserData( parser, (void*)this );

    //Open the XML file for input.
    ifstream inp;
    inp.open( filename );

    //Parse the file using a buffer of the specified size.
    while ( !inp.eof() )
    {
        
        inp.read(buffer, BUF_SIZE);
        const size_t len = inp.gcount();
               
        done = len < sizeof(buffer);

        if ( !XML_Parse(parser, buffer, len, done) )
        {
            ostringstream msg;
            msg << "expat error: " << XML_ErrorString(XML_GetErrorCode(parser));
            throw string( msg.str() );
        }
    }
    
    XML_ParserFree(parser);
    
}

//Called when an open tag is encountered.
void InputParser::StartElement(const AxString& name, const char** attribs)
{
       
    /* 
     * On average elements in the material type map will occur in an XML file
     * more often so search it first.  The next most often used elements will
     * be in the segment map so search it second.  EOC reference will be the
     * third most often elements, followed by timecodes, then edgecodes and
     * finally oof references.
     * 
     */
    if ( _materialTypeMap.find( name ) != _materialTypeMap.end() )
    {
        wostringstream ss;
        AxString atrName;
        
        //Find the mob name.
        OptionalStringAttrib mobName = GetOptionalStringAttribValue( L"name", attribs, 6, L"" );

        OptionalIntAttrib rationalNumerator = GetOptionalIntAttribValue( _optRationalParam[name] + L"-numerator", attribs, 6, 1 );
        OptionalIntAttrib rationalDenominator = GetOptionalIntAttribValue( _optRationalParam[name] + L"-denominator", attribs, 6, 1 );
        aafRational_t optRational;
        optRational.numerator = rationalNumerator.first;
        optRational.denominator = rationalDenominator.first;
        
        //Call the appropriate add function.
        shared_ptr<AxMob> spMob = (_testFile.*_materialTypeMap[name])( mobName.first, mobName.second, optRational );

        if ( !_componentStack.empty() )
        {
            //Call the appropriate initialize/add function.
            shared_ptr<AxSegment> axSegment = dynamic_pointer_cast<AxSegment>( _componentStack.top().second );
            (_testFile.*_fillSegmentMapB[_componentStack.top().first])( axSegment, *spMob );
        }
        
        //Push the new mob onto the stack.
        _mobStack.push( spMob );
        
    }
    else if ( _createSegmentMap.find( name ) != _createSegmentMap.end() )
    {
        AxString essenceType = GetStringAttribValue( L"track-type", attribs, 2, L"none" );

        //Create an empty segment and push it onto the segment stack
        shared_ptr<AxComponent> spComponent = (_testFile.*_createSegmentMap[name])( _essenceMap[essenceType], _effectMap[name] );

        if ( !_slotStack.empty() )
        {
            if ( _slotStack.top().componentsSinceSlot != 0 )
            {
                if ( _fillSegmentMapC.find( _componentStack.top().first ) != _fillSegmentMapC.end() )
                {
                    (_testFile.*_fillSegmentMapC[_componentStack.top().first])( _componentStack.top().second, *spComponent );
                }
                else if ( _fillSegmentMapD.find( _componentStack.top().first ) != _fillSegmentMapD.end() )
                {
                    shared_ptr<AxSegment> axSegment = dynamic_pointer_cast<AxSegment>( _componentStack.top().second );
                    (_testFile.*_fillSegmentMapD[_componentStack.top().first])( axSegment, *spComponent, 1 );
                }
            }
            _slotStack.top().componentsSinceSlot++;
        }
        _componentStack.push( ComponentPair( name, spComponent ) );

    }
    else if ( _attachSlotMap.find( name ) != _attachSlotMap.end() )
    {
        aafRational_t editRate;
        editRate.numerator = GetIntAtribValue(L"edit-rate-numerator", attribs, 8, 1);
        editRate.denominator = GetIntAtribValue(L"edit-rate-denominator", attribs, 8, 1);
        OptionalStringAttrib segName = GetOptionalStringAttribValue( L"name", attribs, 8, L"" );
        OptionalIntAttrib physicalTrackNum = GetOptionalIntAttribValue( L"physical-track-number", attribs, 8, 1 );
        _slotStack.push( SlotInfo( name, editRate, segName, physicalTrackNum ) );
    }
    else if ( _attachParameterMap.find( name ) != _attachParameterMap.end() )
    {
        //Only one of these values should be defined, at most one non-zero will
        //be returned.  Sum the three to get the actual value of the parameter.
        aafUInt32 numerator = GetIntAtribValue( L"numerator", attribs, 4, 0 );
        aafUInt32 boolParam = GetBoolAtribValue( L"bool-value", attribs, 4, false );
        aafUInt32 intParam = GetIntAtribValue( L"int-value", attribs, 4, 0 );
        aafUInt32 param1 = numerator + boolParam + intParam;
        
        aafUInt32 param2 = GetIntAtribValue( L"denominator", attribs, 4, 0 );
        
        shared_ptr<AxOperationGroup> axOpGroup = dynamic_pointer_cast<AxOperationGroup>( _componentStack.top().second );
        (_testFile.*_attachParameterMap[name])( *axOpGroup, _parameterTypeMap[name], param1, param2 );
        
    }
    else
    {
        //Do nothing
    }
       
}

//Called when a close tag is encountered.
void InputParser::EndElement(const AxString& name)
{

    if ( _materialTypeMap.find( name ) != _materialTypeMap.end() )
    {
        _mobStack.pop();
    }
    else if ( _attachSlotMap.find( name ) != _attachSlotMap.end() )
    {
        //Call the appropriate attach function.
        shared_ptr<AxSegment> axSegment = dynamic_pointer_cast<AxSegment>( _componentStack.top().second );
        (_testFile.*_attachSlotMap[name])( 
            *(_mobStack.top()), 
            *axSegment, 
            _slotStack.top().editRate, 
            _slotStack.top().name.first, 
            _slotStack.top().name.second,
            _slotStack.top().physicalTrackNum.first, 
            _slotStack.top().physicalTrackNum.second
           );
        _componentStack.pop();
        _slotStack.pop();
    }
    else if ( _createSegmentMap.find( name ) != _createSegmentMap.end() )
    {
        if ( !_slotStack.empty() )
        {
            _slotStack.top().componentsSinceSlot--;
            if ( _slotStack.top().componentsSinceSlot != 0 )
            {
                _componentStack.pop();
            }
        }
        else
        {
            _componentStack.pop();
        }
    }
    
}

//Called after a string of data (between tags) has been loaded.
void InputParser::EndData(const AxString& contents)
{
    //Currently there is not data espected.
}

//These three static functions are the callback functions called by the expat
//XML parser.  In all cases, the userData parameter refers to the object that'
//owns the XML parser.  The functions simply convert the necessary parameters
//to simpler data types and forward them to the necessary member function.
void InputParser::__StartElement(void *userData, const char *name, const char **atts)
{
    InputParser* me = (InputParser*) userData;
    wostringstream msg;
    msg << name;
    
    //Note: The atts parameter is not changed to a shared pointer because
    //      the memory management is done by the XML parser.    
    me->StartElement( msg.str().c_str(), atts );
}

void InputParser::__EndElement(void *userData, const char *name)
{
    InputParser* me = (InputParser*) userData;
    wostringstream msg;
    msg << name;
    me->EndElement( msg.str().c_str() );
}

void InputParser::__EndData(void *userData, const char *s, int len)
{
     if (len != 0)
    {
        InputParser* me = (InputParser*) userData;
        wostringstream msg;
        msg << s;
        me->EndData( msg.str().substr(0, len).c_str() );
    }
}

const AxString InputParser::GetStringAttribValue( const AxString& attrib, const char** attribs, const unsigned int size, const AxString& default_val ) const
{
    
    for ( unsigned int i = 0; i < size; i += 2 )
    {
        wostringstream ss;
        AxString atrName;
        
        ss << attribs[i];
        atrName = ss.str().c_str();
        if ( atrName == attrib )
        {
            ss.str( L"" );
            ss << attribs[i+1];
            return ss.str().c_str();
        }
        else if ( atrName.length() == 0 )
        {
            break;
        }
    }
    
    return default_val;
}

const int InputParser::GetIntAtribValue( const AxString& attrib, const char** attribs, const unsigned int size, const int default_val ) const
{

    for ( unsigned int i = 0; i < size; i += 2 )
    {
        wostringstream wss;
        AxString atrName;
        
        wss << attribs[i];
        atrName = wss.str().c_str();
        if ( atrName == attrib )
        {
            stringstream ss;
            int retVal;
            
            ss << attribs[i+1];
            ss >> retVal;
            return retVal;
        }
        else if ( atrName.length() == 0 )
        {
            break;
        }
    }
 
    return default_val;
}

const int InputParser::GetBoolAtribValue( const AxString& attrib, const char** attribs, const unsigned int size, const int default_val ) const
{

    for ( unsigned int i = 0; i < size; i += 2 )
    {
        wostringstream wss;
        AxString atrName;
        
        wss << attribs[i];
        atrName = wss.str().c_str();
        if ( atrName == attrib )
        {
            wstringstream ss;
            
            ss << attribs[i+1];
            AxString boolStr =  ss.str().c_str();
            if ( boolStr == L"true" )
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if ( atrName.length() == 0 )
        {
            break;
        }
    }
 
    return default_val;
}

const InputParser::OptionalStringAttrib InputParser::GetOptionalStringAttribValue( const AxString& attrib, const char** attribs, const unsigned int size, const AxString& default_val ) const
{
    
    for ( unsigned int i = 0; i < size; i += 2 )
    {
        wostringstream ss;
        AxString atrName;
        
        ss << attribs[i];
        atrName = ss.str().c_str();
        if ( atrName == attrib )
        {
            ss.str( L"" );
            ss << attribs[i+1];
            return OptionalStringAttrib( ss.str().c_str(), true );
        }
        else if ( atrName.length() == 0 )
        {
            break;
        }
    }
    
    return OptionalStringAttrib( default_val, false );
}

const InputParser::OptionalIntAttrib InputParser::GetOptionalIntAttribValue( const AxString& attrib, const char** attribs, const unsigned int size, const int default_val ) const
{

    for ( unsigned int i = 0; i < size; i += 2 )
    {
        wostringstream wss;
        AxString atrName;
        
        wss << attribs[i];
        atrName = wss.str().c_str();
        if ( atrName == attrib )
        {
            stringstream ss;
            int retVal;
            
            ss << attribs[i+1];
            ss >> retVal;
            return OptionalIntAttrib( retVal, true );
        } 
        else if ( atrName.length() == 0 )
        {
            break;
        }
    }
 
    return OptionalIntAttrib( default_val, false );
}

} // end of namespace diskstream

int main( int argc, char** argv )
{

    using namespace std;

    AxCmdLineArgs args( argc, argv );

    //Input file is the second last argument.
    pair<bool,const char*> inputArg = args.get( argc-2, 1 );
    
    //Requirements Filename is last argument.
    pair<bool, const char*> outputArg = args.get( argc-1, 2 );
        
    //Convert the XML file to an AAF file.
    try
    {
        InputParser parser( outputArg.second );
        parser.ParseXML( inputArg.second );
    }
    catch (AxString ex)
    {
        wcout << ex << endl;
    }

    return 0;
}
