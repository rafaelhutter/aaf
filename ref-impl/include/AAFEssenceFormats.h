//=---------------------------------------------------------------------=
//
// $Id: AAFEssenceFormats.h,v 1.12 2004/09/10 17:12:39 stuart_hc Exp $ $Name:  $
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

// Following are the well-known essence format codes in AAF.
//
//

const aafUID_t kAAFCompression = { 0x6AEFC4F1, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expPixelFormat */
const aafUID_t kAAFPixelFormat = { 0x6AEFC4F2, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expPixelFormat */
const aafUID_t kAAFFrameLayout = { 0x6AEFC4F3, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; 		/* operand.expFrameLayout */
const aafUID_t kAAFFieldDominance = { 0x6AEFC4F4, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expFieldDom */
const aafUID_t kAAFStoredRect = { 0x6AEFC4F5, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; 		/* operand.expRect */
const aafUID_t kAAFDisplayRect = { 0x6AEFC4F6, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };  		/* operand.expRect */
const aafUID_t kAAFSampledRect = { 0x6AEFC4F7, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };  		/* operand.expRect */
const aafUID_t kAAFPixelSize = { 0x6AEFC4F8, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; 			/* operand.expInt16 */
const aafUID_t kAAFAspectRatio = { 0x6AEFC4F9, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expRational */
const aafUID_t kAAFAlphaTransparency = { 0x6AEFC4FA, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };  /* operand.expInt32 */
const aafUID_t kAAFGamma = { 0x6AEFC4FC, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };				/* operand.expRational */
const aafUID_t kAAFImageAlignmentFactor = { 0x6AEFC4FD, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; /* operand.expInt32 */
const aafUID_t kAAFVideoLineMap = { 0x6AEFC4FE, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expLineMap */
const aafUID_t kAAFWillTransferLines = { 0x6AEFC4FF, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };	/* operand.expBoolean */
const aafUID_t kAAFIsCompressed = { 0x6AEFC500, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expBoolean */
const aafUID_t kAAFLineLength = { 0x6AEFC501, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };			/* operand.expInt32 */
		/* the following group is exclusive to RGBA codec */
const aafUID_t kAAFRGBCompLayout = { 0x6AEFC502, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expCompArray */
const aafUID_t kAAFRGBCompSizes = { 0x6AEFC503, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };		/* operand.expCompSizeArray */
const aafUID_t kAAFRGBPalette = { 0x6AEFC504, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };			/* operand.expPointer */
const aafUID_t kAAFRGBPaletteLayout = { 0x6AEFC505, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };	/* operand.expCompArray */
const aafUID_t kAAFRGBPaletteSizes = { 0x6AEFC506, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };	/* operand.expCompSizeArray */	
			/* the following group is exclusive to CDCI codec */
const aafUID_t kAAFCDCICompWidth = { 0x6AEFC507, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };	    /* operand.expInt32 */
const aafUID_t kAAFCDCIHorizSubsampling = { 0x6AEFC508, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };/* operand.expUInt32 */
const aafUID_t kAAFCDCIColorSiting = { 0x6AEFC509, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };    /* operand.expColorSiting */
const aafUID_t kAAFCDCIBlackLevel = { 0x6AEFC50A, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };     /* operand.expUInt32 */
const aafUID_t kAAFCDCIWhiteLevel = { 0x6AEFC50B, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };     /* operand.expUInt32 */
const aafUID_t kAAFCDCIColorRange = { 0x6AEFC50C, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };     /* operand.expUInt32 */
const aafUID_t kAAFCDCIPadBits = { 0x6AEFC50D, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };	    /* operand.expInt16 */
const aafUID_t kAAFFieldStartOffset = { 0xa81aa82b, 0x42d4, 0x11d4, { 0xa3, 0x68, 0x0, 0x90, 0x27, 0xdf, 0xca, 0x6a } };
const aafUID_t kAAFFieldEndOffset = { 0xa81aa82c, 0x42d4, 0x11d4, { 0xa3, 0x68, 0x0, 0x90, 0x27, 0xdf, 0xca, 0x6a } };

/*  the following is to make codec able codec to create CDCI media readable by some legacy applications
    Before setting this parameter make sure that the following 
*/
const aafUID_t kAAFLegacyCDCI = { 0x6AEFC512, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };

const aafUID_t kAAFResolutionID = { 0x6AEFC513, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };

const aafUID_t kAAFAudioSampleBits = { 0x6AEFC50E, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; 
const aafUID_t kAAFMaxSampleBytes = { 0x33F26181, 0xD27F, 0x11d2, { 0x80, 0x99, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; 
const aafUID_t kAAFSampleRate = { 0x6AEFC50F, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; 
const aafUID_t kAAFSampleFormat = { 0x6AEFC510, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } }; 
const aafUID_t kAAFNumChannels = { 0x6AEFC511, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };

// In the event that the pixel buffer needs to be padded to a platform dependent alignment
// use the following essence format to specify the padding bytes per row,
// Typical values are in the range from 0 to 3, with a default is 0. 
// The declaration type should be aafUInt16.
const aafUID_t kAAFPadBytesPerRow = { 0xc146bde0, 0x4c0d, 0x11d3, { 0xbf, 0xdc, 0x0, 0x10, 0x4b, 0xc9, 0x15, 0x6d } };

// This is the compression "quality" setting defined by the Independent JPEG Group.
// Valid values are 25 to 100 with 75 as the default. Less than 25 and the compressed
// data will be very lossy non-baseline JPEG image. The image quality does not improve
// much past 95.
const aafUID_t kAAFCompressionQuality = { 0xc146bde1, 0x4c0d, 0x11d3, { 0xbf, 0xdc, 0x0, 0x10, 0x4b, 0xc9, 0x15, 0x6d } };

// kAAFLegacyDV is used as a boolean specifier to select whether the CDCI codec
// should create properties needed by legacy applications
const aafUID_t kAAFLegacyDV = { 0x6AEFC514, 0x9041, 0x11d2, { 0x80, 0x88, 0x00, 0x60, 0x08, 0x14, 0x3E, 0x6F } };
