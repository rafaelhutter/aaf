// KLV/MXF file dumper.
//
// Tim Bingham - October 2002 - Tim_Bingham@avid.com

// This program is deliberately constructed as one file with no dependencies
// so that it can easily be ported to a new host. You'll be able to make some
// progress understanding an MXF file even if all you have is this source and
// a C++ compiler.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <list>

#if defined (_MSC_VER) && defined(_M_IX86) && defined(_WIN32)
#define MXF_COMPILER_MSC_INTEL_WINDOWS
#define MXF_OS_WINDOWS
#elif defined(__GNUC__) && defined(__i386__) && defined(__linux__)
#define MXF_COMPILER_GCC_INTEL_LINUX
#define MXF_OS_UNIX
#elif defined(__MWERKS__) && defined(__POWERPC__) && defined(macintosh)
#define MXF_COMPILER_MWERKS_PPC_MACOS
#define MXF_OS_MACOS
#elif defined(__MWERKS__) && defined(__MACH__)
#define MXF_COMPILER_MWERKS_PPC_MACOSX
#define MXF_OS_MACOSX
#elif defined(__GNUC__) && defined(__ppc__) && defined(__APPLE__)
#define MXF_COMPILER_GCC_PPC_MACOSX
#define MXF_OS_MACOSX
#elif defined(mips) && defined(sgi)
#define MXF_COMPILER_SGICC_MIPS_SGI
#define MXF_OS_UNIX
#elif defined(__GNUC__) && defined(__i386__) && defined(__FreeBSD__)
#define MXF_COMPILER_GCC_INTEL_FREEBSD
#define MXF_OS_UNIX
#else
#error "Unknown compiler"
#endif

#if defined(MXF_OS_MACOS)
#define MXF_USE_CONSOLE
#endif

#if defined(MXF_USE_CONSOLE)
#include <console.h>
#endif

#if defined(MXF_COMPILER_MSC_INTEL_WINDOWS)
typedef unsigned char          mxfUInt08;
typedef unsigned short int     mxfUInt16;
typedef unsigned int           mxfUInt32;
typedef unsigned _int64        mxfUInt64;

#define MXFPRIu08 "u"
#define MXFPRIu16 "hu"
#define MXFPRIu32 "u"
#define MXFPRIu64 "I64u"
#define MXFPRIx08 "x"
#define MXFPRIx16 "hx"
#define MXFPRIx32 "x"
#define MXFPRIx64 "I64x"
#elif defined(MXF_COMPILER_GCC_INTEL_LINUX)
typedef unsigned char          mxfUInt08;
typedef unsigned short int     mxfUInt16;
typedef unsigned long int      mxfUInt32;
typedef unsigned long long int mxfUInt64;

#define MXFPRIu08 "u"
#define MXFPRIu16 "hu"
#define MXFPRIu32 "lu"
#define MXFPRIu64 "llu"
#define MXFPRIx08 "x"
#define MXFPRIx16 "hx"
#define MXFPRIx32 "lx"
#define MXFPRIx64 "llx"
#elif defined(MXF_COMPILER_MWERKS_PPC_MACOS)
typedef unsigned char          mxfUInt08;
typedef unsigned short int     mxfUInt16;
typedef unsigned long int      mxfUInt32;
typedef unsigned long long int mxfUInt64;

#define MXFPRIu08 "u"
#define MXFPRIu16 "hu"
#define MXFPRIu32 "lu"
#define MXFPRIu64 "llu"
#define MXFPRIx08 "x"
#define MXFPRIx16 "hx"
#define MXFPRIx32 "lx"
#define MXFPRIx64 "llx"
#elif defined(MXF_COMPILER_MWERKS_PPC_MACOSX)
typedef unsigned char          mxfUInt08;
typedef unsigned short int     mxfUInt16;
typedef unsigned long int      mxfUInt32;
typedef unsigned long long int mxfUInt64;

#define MXFPRIu08 "u"
#define MXFPRIu16 "hu"
#define MXFPRIu32 "lu"
#define MXFPRIu64 "llu"
#define MXFPRIx08 "x"
#define MXFPRIx16 "hx"
#define MXFPRIx32 "lx"
#define MXFPRIx64 "llx"
#elif defined(MXF_COMPILER_GCC_PPC_MACOSX)
typedef unsigned char          mxfUInt08;
typedef unsigned short int     mxfUInt16;
typedef unsigned long int      mxfUInt32;
typedef unsigned long long int mxfUInt64;

#define MXFPRIu08 "u"
#define MXFPRIu16 "hu"
#define MXFPRIu32 "lu"
#define MXFPRIu64 "llu"
#define MXFPRIx08 "x"
#define MXFPRIx16 "hx"
#define MXFPRIx32 "lx"
#define MXFPRIx64 "llx"
#elif defined(MXF_COMPILER_SGICC_MIPS_SGI)
typedef unsigned char          mxfUInt08;
typedef unsigned short int     mxfUInt16;
typedef unsigned long int      mxfUInt32;
typedef unsigned long long int mxfUInt64;

#define MXFPRIu08 "u"
#define MXFPRIu16 "hu"
#define MXFPRIu32 "lu"
#define MXFPRIu64 "llu"
#define MXFPRIx08 "x"
#define MXFPRIx16 "hx"
#define MXFPRIx32 "lx"
#define MXFPRIx64 "llx"
#elif defined (MXF_COMPILER_GCC_INTEL_FREEBSD)
typedef unsigned char          mxfUInt08;
typedef unsigned short int     mxfUInt16;
typedef unsigned long int      mxfUInt32;
typedef unsigned long long int mxfUInt64;

#define MXFPRIu08 "u"
#define MXFPRIu16 "hu"
#define MXFPRIu32 "lu"
#define MXFPRIu64 "llu"
#define MXFPRIx08 "x"
#define MXFPRIx16 "hx"
#define MXFPRIx32 "lx"
#define MXFPRIx64 "llx"
#endif

typedef mxfUInt64 mxfLength;
typedef mxfUInt08 mxfByte;
typedef mxfByte mxfKey[16];
typedef mxfUInt16 mxfLocalKey;

typedef struct aafUIDTag {
  mxfUInt32 Data1;
  mxfUInt16 Data2;
  mxfUInt16 Data3;
  mxfUInt08 Data4[8];
} aafUID;

typedef struct mxfRationalTag {
  mxfUInt32 numerator;
  mxfUInt32 denominator;
} mxfRational;

typedef enum ModeTag {
  unspecifiedMode,
  klvMode,
  localSetMode,
  mxfMode,
  aafMode,
  klvValidateMode,
  setValidateMode,
  mxfValidateMode,
  aafValidateMode} Mode;
Mode mode = unspecifiedMode;

bool reorder(void);
mxfUInt08 hostByteOrder(void);

// Primitives
#if defined(MXF_OS_WINDOWS)
#include <windows.h>
typedef HANDLE mxfFile;
#elif defined(MXF_OS_MACOSX)
#include <CoreServices/CoreServices.h>
typedef SInt16 mxfFile;
#else
typedef FILE* mxfFile;
#endif

mxfFile openExistingRead(char* fileName);
void close(mxfFile infile);
void setPosition(mxfFile infile, const mxfUInt64 position);
mxfUInt64 position(mxfFile infile);
size_t read(mxfFile infile, void* buffer, size_t count);
mxfUInt64 size(mxfFile infile);

void skipBytes(const mxfUInt64 byteCount, mxfFile infile);

void readMxfUInt08(mxfByte& b, mxfFile infile);
void readMxfUInt16(mxfUInt16& i, mxfFile infile);
void readMxfUInt32(mxfUInt32& i, mxfFile infile);
void readMxfUInt64(mxfUInt64& i, mxfFile infile);
void readMxfRational(mxfRational& r, mxfFile infile);
void readMxfLabel(mxfKey& k, mxfFile infile);
void readMxfKey(mxfKey& k, mxfFile infile);
bool readOuterMxfKey(mxfKey& k, mxfFile infile);
int readBERLength(mxfUInt64& i, mxfFile infile);
int readMxfLength(mxfLength& l, mxfFile infile);
void readMxfLocalKey(mxfLocalKey& k, mxfFile infile);

void reorder(mxfUInt16& i);
void reorder(mxfUInt32& i);
void reorder(mxfUInt64& i);
void reorder(aafUID& u);

void printDecField(FILE* f, mxfUInt08& i);
void printDecField(FILE* f, mxfUInt16& i);
void printDecField(FILE* f, mxfUInt32& i);
void printDecField(FILE* f, mxfUInt64& i);

void printDecFieldPad(FILE* f, mxfUInt08& i);
void printDecFieldPad(FILE* f, mxfUInt16& i);
void printDecFieldPad(FILE* f, mxfUInt32& i);
void printDecFieldPad(FILE* f, mxfUInt64& i);

void printDec(FILE* f, mxfUInt08& i);
void printDec(FILE* f, mxfUInt16& i);
void printDec(FILE* f, mxfUInt32& i);
void printDec(FILE* f, mxfUInt64& i);

void printHexField(FILE* f, mxfUInt08& i);
void printHexField(FILE* f, mxfUInt16& i);
void printHexField(FILE* f, mxfUInt32& i);
void printHexField(FILE* f, mxfUInt64& i);

void printHexFieldPad(FILE* f, mxfUInt08& i);
void printHexFieldPad(FILE* f, mxfUInt16& i);
void printHexFieldPad(FILE* f, mxfUInt32& i);
void printHexFieldPad(FILE* f, mxfUInt64& i);

void printHex(FILE* f, mxfUInt08& i);
void printHex(FILE* f, mxfUInt16& i);
void printHex(FILE* f, mxfUInt32& i);
void printHex(FILE* f, mxfUInt64& i);

void printMxfKey(const mxfKey& k, FILE* f);
void printMxfLength(mxfLength& l, FILE* f);
void printMxfLocalKey(mxfLocalKey& k, FILE* f);

void printMxfUInt08(FILE* f, const char* label, mxfUInt08& i);
void printMxfUInt16(FILE* f, const char* label, mxfUInt16& i);
void printMxfUInt32(FILE* f, const char* label, mxfUInt32& i);
void printMxfUInt64(FILE* f, const char* label, mxfUInt64& i);

void dumpMxfUInt08(const char* label, mxfFile infile);
void dumpMxfUInt16(const char* label, mxfFile infile);
void dumpMxfUInt32(const char* label, mxfFile infile);
void dumpMxfUInt64(const char* label, mxfFile infile);
void dumpMxfRational(const char* label, mxfFile infile);
void dumpMxfLabel(const char* label, mxfFile infile);
void dumpMxfOperationalPattern(const char* label, mxfFile infile);

void printOperationalPattern(mxfKey& k, FILE* outfile);

void klvDumpFile(mxfFile infile);
void setDumpFile(mxfFile infile);
void mxfDumpFile(mxfFile infile);
void aafDumpFile(mxfFile infile);

bool lookupMXFKey(mxfKey& k, size_t& index);
bool lookupAAFKey(mxfKey& k, size_t& index);
bool findAAFKey(mxfKey& k, size_t& index, char** flag);

bool lookupMXFLocalKey(mxfLocalKey& k, size_t& index);
bool lookupAAFLocalKey(mxfLocalKey& k, size_t& index);

bool isEssenceElement(mxfKey& k);

void checkKey(mxfKey& k);
bool isNullKey(mxfKey& k);
bool isDark(mxfKey& k, Mode mode);
bool isFill(mxfKey& k);
bool isPartition(mxfKey& key);
bool isHeader(mxfKey& key);
bool isFooter(mxfKey& key);

void printFill(mxfKey& k, mxfLength& len, mxfFile infile);

void skipV(mxfLength& length, mxfFile infile);

void checkPartitionLength(mxfUInt64& length);

// Size of the fixed portion of a partition
mxfUInt64 partitionFixedSize =
  sizeof(mxfUInt16) + // Major Version
  sizeof(mxfUInt16) + // Minor Version
  sizeof(mxfUInt32) + // KAGSize
  sizeof(mxfUInt64) + // ThisPartition
  sizeof(mxfUInt64) + // PreviousPartition
  sizeof(mxfUInt64) + // FooterPartition
  sizeof(mxfUInt64) + // HeaderByteCount
  sizeof(mxfUInt64) + // IndexByteCount
  sizeof(mxfUInt32) + // IndexSID
  sizeof(mxfUInt64) + // BodyOffset
  sizeof(mxfUInt32) + // BodySID
  sizeof(mxfKey)    + // Operational pattern
  sizeof(mxfUInt32) + // Essence container label count
  sizeof(mxfUInt32);  // Essence container label size

void print(char* format, ...);

void vprint(char* format, va_list ap);

void error(char* format, ...);

void verror(char* format, va_list ap);

void fatalError(char* format, ...);

void warning(char* format, ...);

void vwarning(char* format, va_list ap);

void message(char* format, ...);

void vmessage(char* format, va_list ap);

const char* baseName(char* fullName);

void setProgramName(char* programName);

char* programName(void);

void mxfError(char* format, ...);

void mxfWarning(char* format, ...);

void print(char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
}

void vprint(char* format, va_list ap)
{
  vfprintf(stderr, format, ap);
}

void error(char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s : Error : ", programName());
  vfprintf(stderr, format, ap);
  va_end(ap);
}

void verror(char* format, va_list ap)
{
  fprintf(stderr, "%s : Error : ", programName());
  vfprintf(stderr, format, ap);
}

void fatalError(char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s : Error : ", programName());
  vfprintf(stderr, format, ap);
  exit(EXIT_FAILURE);
  va_end(ap);
}

void warning(char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s : Warning : ", programName());
  vfprintf(stderr, format, ap);
  va_end(ap);
}

void vwarning(char* format, va_list ap)
{
  fprintf(stderr, "%s : Warning : ", programName());
  vfprintf(stderr, format, ap);
}

void message(char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s : ", programName());
  vfprintf(stderr, format, ap);
  va_end(ap);
}

void vmessage(char* format, va_list ap)
{
  fprintf(stderr, "%s : ", programName());
  vfprintf(stderr, format, ap);
}

char progName[FILENAME_MAX];

void setProgramName(char* programName)
{
  const char* base = baseName(programName);
  const char* suffix = strrchr(base, '.');
  size_t length;
  if (suffix != 0) {
    length = suffix - base;
  } else {
    length = strlen(base);
  }
  if (length >= FILENAME_MAX) {
    length = FILENAME_MAX - 1;
  }
  strncpy(progName, base, length);
  progName[length] = '\0';
}

char* programName(void)
{
  return progName;
}

mxfUInt32 errors = 0;

void mxfError(char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  verror(format, ap);
  va_end(ap);
  errors = errors + 1;
  if (errors > 100) {
    error("More than 100 errors encountered - giving up.\n");
    exit(EXIT_FAILURE);
  }
}

mxfUInt32 warnings = 0;

void mxfWarning(char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  vwarning(format, ap);
  va_end(ap);
  warnings = warnings + 1;
}

bool verbose = false;
bool debug = false;

mxfUInt64 keyPosition;   // position/address of current key

// Frame wrapped essence
bool frames = false;     // if true, treat essence as frame wrapped.
bool iFlag = false;      // if true, only print maxFrames frames
mxfUInt32 maxFrames = 0;

// Frame index tables
bool cFlag = true;      // if true, only print maxIndex entries
mxfUInt32 maxIndex = 0;

// Essence elements
bool lFlag = false;      // if true, limit/no-limit specified on command line
bool limitBytes = false; // if true, only print limit bytes
mxfUInt32 limit = 0;

// Help
bool hFlag = false;

#if defined(MXF_OS_WINDOWS)
mxfFile openExistingRead(char* fileName)
{
  HANDLE result = CreateFile(fileName,
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             0,
                             OPEN_EXISTING,
                             0,
                             0);
  if (result == INVALID_HANDLE_VALUE) {
    result = 0;
  }
  return result;
}

void close(mxfFile infile)
{
  BOOL result = CloseHandle(infile);
  if (!result) {
    fatalError("CloseHandle() failed.\n");
  }
}

void setPosition(mxfFile infile, const mxfUInt64 position)
{
  LARGE_INTEGER li;
  li.QuadPart = position;
  li.LowPart = SetFilePointer(infile, li.LowPart, &li.HighPart, FILE_BEGIN);
  if ((li.LowPart == -1) && GetLastError() != NO_ERROR) {
    fatalError("SetFilePointer() failed.\n");
  }
}

size_t read(mxfFile infile, void* buffer, size_t count)
{
  DWORD bytesRead;
  BOOL result = ReadFile(infile, buffer, count, &bytesRead, 0);
  if (!result) {
    fatalError("ReadFile() failed.\n");
  }
  return bytesRead;
}

mxfUInt64 position(mxfFile infile)
{
  mxfUInt64 result;
  LARGE_INTEGER li;
  li.QuadPart = 0;
  li.LowPart = SetFilePointer(infile, li.LowPart, &li.HighPart, FILE_CURRENT);
  if ((li.LowPart == -1) && GetLastError() != NO_ERROR) {
    fatalError("SetFilePointer() failed.\n");
  }
  result = li.QuadPart;
  return result;
}

mxfUInt64 size(mxfFile infile)
{
  mxfUInt64 result;
  ULARGE_INTEGER li;
  li.LowPart = GetFileSize(infile, &li.HighPart);
  if ((li.LowPart == -1) && GetLastError() != NO_ERROR) {
    fatalError("GetFileSize() failed.\n");
  }
  result = li.QuadPart;
  return result;
}

#elif defined(MXF_OS_MACOSX)
mxfFile openExistingRead(char* fileName)
{
  const UInt8* name = reinterpret_cast<UInt8*>(fileName);
  FSRef fref;
  OSErr status = FSPathMakeRef(name, &fref, 0);
  if (status != noErr) {
    fatalError("FSPathMakeRef() failed (%d).\n", status);
  }
  HFSUniStr255 fkName;
  status = FSGetDataForkName(&fkName);
  if (status != noErr) {
    fatalError("FSGetDataForkName() failed (%d).\n", status);
  }
  SInt16 result;
  status = FSOpenFork(&fref, fkName.length, fkName.unicode, fsRdPerm, &result);
  if (status != noErr) {
    fatalError("FSOpenFork() failed (%d).\n", status);
  }
  return result;
}

void close(mxfFile infile)
{
  OSErr status = FSCloseFork(infile);
  if (status != noErr) {
    fatalError("FSCloseFork() failed (%d).\n", status);
  }
}

void setPosition(mxfFile infile, const mxfUInt64 position)
{
  SInt64 pos = position;
  OSErr status = FSSetForkPosition(infile, fsFromStart, pos);
  if (status != noErr) {
    fatalError("FSSetForkPosition() failed (%d).\n", status);
  }
}

size_t read(mxfFile infile, void* buffer, size_t count)
{
  ByteCount bytesRead;
  OSErr status = FSReadFork(infile, fsAtMark, 0, count, buffer, &bytesRead);
  if ((status != eofErr) && (status != noErr)) {
    fatalError("FSReadFork() failed (%d).\n", status);
  }
  return bytesRead;
}

mxfUInt64 position(mxfFile infile)
{
  SInt64 position;
  OSErr status = FSGetForkPosition(infile, &position);
  if (status != noErr) {
    fatalError("FSGetForkPosition() failed (%d).\n", status);
  }
  mxfUInt64 result = position;
  return result;
}

mxfUInt64 size(mxfFile infile)
{
  SInt64 size;
  OSErr status = FSGetForkSize(infile, &size);
  if (status != noErr) {
    fatalError("FSGetForkSize() failed (%d).\n", status);
  }
  mxfUInt64 result = size;
  return result;
}

#else

#if defined(__GLIBC__) && defined(__GNUC_MINOR__)
#if (__GLIBC__ >= 2) && (__GLIBC_MINOR__ >=3)
// fseeko present in 2.3 but not in 2.2
#define MXF_SEEKO
#endif
#endif

mxfFile openExistingRead(char* fileName)
{
  return  fopen(fileName, "rb");
}

void close(mxfFile infile)
{
  fclose(infile);
}

void seek64(mxfFile infile, const mxfUInt64 position, int whence)
{
#if defined(MXF_COMPILER_GCC_INTEL_LINUX) && defined(MXF_SEEKO)
  int status = fseeko(infile, position, whence);
#elif defined(MXF_COMPILER_MWERKS_PPC_MACOS)
  int status = _fseek(infile, position, whence);
#elif defined(MXF_COMPILER_MWERKS_PPC_MACOSX)
  int status = _fseek(infile, position, whence);
#elif defined(MXF_COMPILER_GCC_PPC_MACOSX)
  int status = fseeko(infile, position, whence);
#elif defined(MXF_COMPILER_SGICC_MIPS_SGI)
  int status = fseeko64(infile, position, whence);
#else
  long offset = static_cast<long>(position);
  if (position != static_cast<mxfUInt64>(offset)) {
    fatalError("Offset too large.\n");
  }
  int status = fseek(infile, offset, whence);
#endif
  if (status != 0) {
    fatalError("Failed to seek.\n");
  }
}

void setPosition(mxfFile infile, const mxfUInt64 position)
{
  seek64(infile, position, SEEK_SET);
}

size_t read(mxfFile infile, void* buffer, size_t count)
{
  return fread(buffer, 1, count, infile);
}

mxfUInt64 tell64(mxfFile infile)
{
#if defined(MXF_COMPILER_GCC_INTEL_LINUX) && defined(MXF_SEEKO)
  mxfUInt64 result = ftello(infile);
#elif defined(MXF_COMPILER_MWERKS_PPC_MACOS)
  mxfUInt64 result = _ftell(infile);
#elif defined(MXF_COMPILER_MWERKS_PPC_MACOSX)
  mxfUInt64 result = _ftell(infile);
#elif defined(MXF_COMPILER_GCC_PPC_MACOSX)
  mxfUInt64 result = ftello(infile);
#elif defined(MXF_COMPILER_SGICC_MIPS_SGI)
  mxfUInt64 result = ftello64(infile);
#else
  mxfUInt64 result = ftell(infile);
#endif
  if (result == (mxfUInt64)-1) {
    fatalError("Failed to tell.\n");
  }
  return result;
}

mxfUInt64 position(mxfFile infile)
{
  return tell64(infile);
}

mxfUInt64 size(mxfFile infile)
{
  mxfUInt64 savedPosition = position(infile);
  seek64(infile, 0, SEEK_END);
  mxfUInt64 result = position(infile);
  setPosition(infile, savedPosition);
  return result;
}
#endif

bool findPattern(mxfUInt64& position,
                 mxfByte* pattern,
                 size_t patternSize,
                 mxfUInt32 limit,
                 mxfFile infile);

bool findPattern(mxfUInt64& position,
                 mxfByte* pattern,
                 size_t patternSize,
                 mxfUInt32 limit,
                 mxfFile infile)
{
  bool found = false;
  mxfUInt64 pos = 0;
  int c = 0;
  size_t i = 0;
  do {
    mxfByte b;
    c = read(infile, &b, 1);
    if (c == 1) {
      if (b != pattern[i]) {
        pos = pos + i + 1;
        i = 0;
      } else {
        if (i < patternSize - 1) {
          i = i + 1;
        } else {
          position = pos;
          found = true;
        }
      }
    }
  } while ((!found) && (pos < limit) && (c == 1));
  return found;
}

mxfByte headerPrefix[] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01,
                           0x01, 0x0d, 0x01, 0x02, 0x01, 0x01, 0x02};

bool findHeader(mxfFile infile, mxfUInt64& headerPosition);

bool findHeader(mxfFile infile, mxfUInt64& headerPosition)
{
  return findPattern(headerPosition,
                     headerPrefix,
                     sizeof(headerPrefix),
                     64 * 1024,
                     infile);
}

bool isMxfFile(mxfFile infile, mxfUInt64& headerPosition);

bool isMxfFile(mxfFile infile, mxfUInt64& headerPosition)
{
  bool result = false;

  if (findHeader(infile, headerPosition)) {
    setPosition(infile, headerPosition);
    mxfKey k;
    readMxfKey(k, infile);
    if (isHeader(k)) {
      result = true;
    }
  }

  return result;
}

void skipBytes(const mxfUInt64 byteCount, mxfFile infile)
{
  setPosition(infile, position(infile) + byteCount);
}

void readMxfUInt08(mxfByte& b, mxfFile infile)
{
  int c = read(infile, &b, sizeof(mxfByte));
  if (c != sizeof(mxfByte)) {
    fatalError("Failed to read byte.\n");
  }
}

void readMxfUInt16(mxfUInt16& i, mxfFile infile)
{
  int c = read(infile, &i, sizeof(mxfUInt16));
  if (c != sizeof(mxfUInt16)) {
    fatalError("Failed to read mxfUInt16.\n");
  }
  if (reorder()) {
    reorder(i);
  }
}

void readMxfUInt32(mxfUInt32& i, mxfFile infile)
{
  int c = read(infile, &i, sizeof(mxfUInt32));
  if (c != sizeof(mxfUInt32)) {
    fatalError("Failed to read mxfUInt32.\n");
  }
  if (reorder()) {
    reorder(i);
  }
}

void readMxfUInt64(mxfUInt64& i, mxfFile infile)
{
  int c = read(infile, &i, sizeof(mxfUInt64));
  if (c != sizeof(mxfUInt64)) {
    fatalError("Failed to read mxfUInt64.\n");
  }
  if (reorder()) {
    reorder(i);
  }
}

void readMxfRational(mxfRational& r, mxfFile infile)
{
  readMxfUInt32(r.numerator, infile);
  readMxfUInt32(r.denominator, infile);
}

void readMxfLabel(mxfKey& k, mxfFile infile)
{
  int c = read(infile, &k, sizeof(mxfKey));
  if (c != sizeof(mxfKey)) {
    fatalError("Failed to read label.\n");
  }
}

void readMxfKey(mxfKey& k, mxfFile infile)
{
  keyPosition = position(infile);
  int c = read(infile, &k, sizeof(mxfKey));
  if (c != sizeof(mxfKey)) {
    fatalError("Failed to read key.\n");
  }
}

bool readOuterMxfKey(mxfKey& k, mxfFile infile)
{
  bool result = true;
  keyPosition = position(infile);
  int c = read(infile, &k, sizeof(mxfKey));
  if (c == sizeof(mxfKey)) {
    result = true;
  } else if (c == 0) {
    result = false;
  } else {
    fatalError("Failed to read key.\n");
  }
  return result;
}

int readBERLength(mxfUInt64& i, mxfFile infile)
{
  int bytesRead = 0;
  mxfUInt08 b;
  readMxfUInt08(b, infile);
  bytesRead = bytesRead + 1;
  if (b == 0x80) {
    // unknown length
    i = 0;
  } else if ((b & 0x80) != 0x80) {
    // short form
    i = b;
  } else {
    // long form
    int length = b & 0x7f;
    i = 0;
    for (int k = 0; k < length; k++) {
      readMxfUInt08(b, infile);
      bytesRead = bytesRead + 1;
      i = i << 8;
      i = i + b;
    }
  }
  return bytesRead;
}

int readMxfLength(mxfLength& l, mxfFile infile)
{
  mxfUInt64 x = 0;
  int bytesRead = readBERLength(x, infile);
  if (bytesRead > 9) {
    mxfError("Invalid BER encoded length"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
  }
  if (x == 0) {
    mxfWarning("Length is zero"
               " (following key at offset 0x%"MXFPRIx64").\n",
               keyPosition);
  }
  l = x;
  return bytesRead;
}

void readMxfLocalKey(mxfLocalKey& k, mxfFile infile)
{
  readMxfUInt16(k, infile);
}

void reorder(mxfUInt16& i)
{
  mxfUInt08* p = (mxfUInt08*)&i;
  mxfUInt08 temp;

  temp = p[0];
  p[0] = p[1];
  p[1] = temp;
}

void reorder(mxfUInt32& i)
{
  mxfUInt08* p = (mxfUInt08*)&i;
  mxfUInt08 temp;

  temp = p[0];
  p[0] = p[3];
  p[3] = temp;

  temp = p[1];
  p[1] = p[2];
  p[2] = temp;
}

void reorder(mxfUInt64& i)
{
  mxfUInt08* p = (mxfUInt08*)&i;
  mxfUInt08 temp;

  temp = p[0];
  p[0] = p[7];
  p[7] = temp;

  temp = p[1];
  p[1] = p[6];
  p[6] = temp;

  temp = p[2];
  p[2] = p[5];
  p[5] = temp;

  temp = p[3];
  p[3] = p[4];
  p[4] = temp;
}

void reorder(aafUID& u)
{
  reorder(u.Data1);
  reorder(u.Data2);
  reorder(u.Data3);
  // no need to reorder Data4
}

void printDecField(FILE* f, mxfUInt08& i)
{
  fprintf(f, "%3"MXFPRIu08, i);
}

void printDecField(FILE* f, mxfUInt16& i)
{
  fprintf(f, "%5"MXFPRIu16, i);
}

void printDecField(FILE* f, mxfUInt32& i)
{
  fprintf(f, "%10"MXFPRIu32, i);
}

void printDecField(FILE* f, mxfUInt64& i)
{
  fprintf(f, "%10"MXFPRIu64, i); // tjb - should be 20
}

void printDecFieldPad(FILE* f, mxfUInt08& i)
{
  fprintf(f, "%03"MXFPRIu08, i);
}

void printDecFieldPad(FILE* f, mxfUInt16& i)
{
  fprintf(f, "%05"MXFPRIu16, i);
}

void printDecFieldPad(FILE* f, mxfUInt32& i)
{
  fprintf(f, "%010"MXFPRIu32, i);
}

void printDecFieldPad(FILE* f, mxfUInt64& i)
{
  fprintf(f, "%010"MXFPRIu64, i); // tjb - should be 20
}

void printDec(FILE* f, mxfUInt08& i)
{
  fprintf(f, "%"MXFPRIu08, i);
}

void printDec(FILE* f, mxfUInt16& i)
{
  fprintf(f, "%"MXFPRIu16, i);
}

void printDec(FILE* f, mxfUInt32& i)
{
  fprintf(f, "%"MXFPRIu32, i);
}

void printDec(FILE* f, mxfUInt64& i)
{
  fprintf(f, "%"MXFPRIu64, i);
}

void printHexField(FILE* f, mxfUInt08& i)
{
  fprintf(f, "%2"MXFPRIx08, i);
}

void printHexField(FILE* f, mxfUInt16& i)
{
  fprintf(f, "%4"MXFPRIx16, i);
}

void printHexField(FILE* f, mxfUInt32& i)
{
  fprintf(f, "%8"MXFPRIx32, i);
}

void printHexField(FILE* f, mxfUInt64& i)
{
  fprintf(f, "%16"MXFPRIx64, i);
}

void printHexFieldPad(FILE* f, mxfUInt08& i)
{
  fprintf(f, "%02"MXFPRIx08, i);
}

void printHexFieldPad(FILE* f, mxfUInt16& i)
{
  fprintf(f, "%04"MXFPRIx16, i);
}

void printHexFieldPad(FILE* f, mxfUInt32& i)
{
  fprintf(f, "%08"MXFPRIx32, i);
}

void printHexFieldPad(FILE* f, mxfUInt64& i)
{
  fprintf(f, "%016"MXFPRIx64, i);
}

void printHex(FILE* f, mxfUInt08& i)
{
  fprintf(f, "%"MXFPRIx08, i);
}

void printHex(FILE* f, mxfUInt16& i)
{
  fprintf(f, "%"MXFPRIx16, i);
}

void printHex(FILE* f, mxfUInt32& i)
{
  fprintf(f, "%"MXFPRIx32, i);
}

void printHex(FILE* f, mxfUInt64& i)
{
  fprintf(f, "%"MXFPRIx64, i);
}

void printMxfKey(const mxfKey& k, FILE* f)
{
  for (size_t i = 0; i < sizeof(mxfKey); i++) {
    unsigned int b = k[i];
    fprintf(f, "%02x", b);
    if (i < (sizeof(mxfKey) - 1)) {
      fprintf(f, ".");
    }
  }
}

void printMxfLength(mxfLength& l, FILE* f)
{
  printDecField(f, l);
  fprintf(f, " ");
  fprintf(f, "(");
  printHex(f, l);
  fprintf(f, ")");
}

void printMxfLocalKey(mxfLocalKey& k, FILE* f)
{
  unsigned int msb = (k & 0xff00) >> 8;
  unsigned int lsb = (k & 0x00ff);
  fprintf(f, "%02x.%02x", msb, lsb);
}

void printMxfUInt08(FILE* f, const char* label, mxfUInt08& i)
{
  fprintf(f, "%20s = ", label);
  printHexFieldPad(f, i);
  fprintf(f, "\n");
}

void printMxfUInt16(FILE* f, const char* label, mxfUInt16& i)
{
  fprintf(f, "%20s = ", label);
  printHexFieldPad(f, i);
  fprintf(f, "\n");
}

void printMxfUInt32(FILE* f, const char* label, mxfUInt32& i)
{
  fprintf(f, "%20s = ", label);
  printHexFieldPad(f, i);
  fprintf(f, "\n");
}

void printMxfUInt64(FILE* f, const char* label, mxfUInt64& i)
{
  fprintf(f, "%20s = ", label);
  printHexFieldPad(f, i);
  fprintf(f, "\n");
}

void dumpMxfUInt08(const char* label, mxfFile infile)
{
  mxfUInt08 i;
  readMxfUInt08(i, infile);
  printMxfUInt08(stdout, label, i);
}

void dumpMxfUInt16(const char* label, mxfFile infile)
{
  mxfUInt16 i;
  readMxfUInt16(i, infile);
  printMxfUInt16(stdout, label, i);
}

void dumpMxfUInt32(const char* label, mxfFile infile)
{
  mxfUInt32 i;
  readMxfUInt32(i, infile);
  printMxfUInt32(stdout, label, i);
}

void dumpMxfUInt64(const char* label, mxfFile infile)
{
  mxfUInt64 i;
  readMxfUInt64(i, infile);
  printMxfUInt64(stdout, label, i);
}

void dumpMxfRational(const char* label, mxfFile infile)
{
  mxfRational r;
  readMxfRational(r, infile);
  fprintf(stdout, "%20s = ", label);
  fprintf(stdout, "( ");
  printDecField(stdout, r.numerator);
  fprintf(stdout, " / ");
  printDecField(stdout, r.denominator);
  fprintf(stdout, " )");
  fprintf(stdout, "\n");
}

void dumpMxfLabel(const char* label, mxfFile infile)
{
  mxfKey k;
  readMxfLabel(k, infile);
  fprintf(stdout, "%20s = ", label);
  printMxfKey(k, stdout);
  fprintf(stdout, "\n");
}

void dumpMxfOperationalPattern(const char* label, mxfFile infile)
{
  mxfKey k;
  readMxfLabel(k, infile);
  fprintf(stdout, "%20s = ", label);
  printMxfKey(k, stdout);
  fprintf(stdout, "\n");
  fprintf(stdout, "%20s = ", "");
  printOperationalPattern(k, stdout);
  fprintf(stdout, "\n");
}

void printGeneralizedOperationalPattern(mxfKey& k, FILE* outfile)
{
  mxfByte itemComplexity = k[12];
  if ((itemComplexity >= 1) && (itemComplexity <= 3)) {
    char* itemCplxName;
    switch (itemComplexity) {
    case 1:
      itemCplxName = "Single Item";
      break;
    case 2:
      itemCplxName = "Play-list Items";
      break;
    case 3:
      itemCplxName = "Edit Items";
      break;
    default:
      itemCplxName = "Unknown";
      break;
    }

    mxfByte packageComplexity = k[13];
    char* packageCplxName;
    switch (packageComplexity) {
    case 1:
      packageCplxName = "Single Package";
      break;
    case 2:
      packageCplxName = "Ganged Packages";
      break;
    case 3:
      packageCplxName = "Alternate Versions";
      break;
    default:
      packageCplxName = "Unknown";
     break;
    }
    // mxfByte qualifiers = k[14]; // tjb not yet decoded
    fprintf(outfile, "[%s, %s]", itemCplxName, packageCplxName);
  }
}

void printAtomOperationalPattern(mxfKey& k, FILE* outfile)
{
  // tjb - should check for specific Atom patterns
  fprintf(outfile, "[Specialized - Atom]");
}

void printSpecializedOperationalPattern(mxfKey& k, FILE* outfile)
{
  mxfByte itemComplexity = k[12];
  if (itemComplexity == 0x10) {
    printAtomOperationalPattern(k, outfile);
  } else {
    fprintf(outfile, "[Specialized - Unknown]");
  }
}

void printOperationalPattern(mxfKey& k, FILE* outfile)
{
  mxfByte itemComplexity = k[12];
  if ((itemComplexity >= 1) && (itemComplexity <= 3)) {
    printGeneralizedOperationalPattern(k, outfile);
  } else if ((itemComplexity >= 0x10) && (itemComplexity <= 0x7f)) {
    printSpecializedOperationalPattern(k, outfile);
  } else {
    fprintf(outfile, "[Unknown]");
  }
}

bool reorder(void)
{
  bool result;
  if (hostByteOrder() == 'B') {
    result = false;
  } else {
    result = true;
  }
  return result;
}

mxfUInt08 hostByteOrder(void)
{
  mxfUInt16 word = 0x1234;
  mxfUInt08  byte = *((mxfUInt08*)&word);
  mxfUInt08 result;

  if (byte == 0x12) {
    result = 'B';
  } else {
    result = 'L';
  }
  return result;
}

const mxfKey NullKey =
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Define values of MXF keys

#define MXF_LABEL(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
                 {a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p}

#define MXF_DEFINE_PACK_KEY(n, k)     const mxfKey n = k;
#define MXF_DEFINE_KEY(n, k)          const mxfKey n = k;
#define MXF_CLASS(name, id, parent, concrete) const mxfKey name = id;

#include "MXFMetaDictionary.h"
// keys not in MXFMetaDictionary.h
const mxfKey V10RandomIndexMetadata =
  {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, 0x01,
   0x0d, 0x01, 0x02, 0x01, 0x01, 0x11, 0x00, 0x00};
const mxfKey V10IndexTableSegment =
  {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01,
   0x0d, 0x01, 0x02, 0x01, 0x01, 0x10, 0x00, 0x00};
const mxfKey SystemMetadata =
  {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, 0x01,
   0x0d, 0x01, 0x03, 0x01, 0x04, 0x01, 0x01, 0x00};
const mxfKey BogusFill =
  {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01,
   0x03, 0x01, 0x02, 0x10, 0x01, 0x01, 0x01, 0x00};
//
const mxfKey MXFGenericAudioEssenceDescriptor =
  {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01,
   0x0d, 0x01, 0x01, 0x01, 0x01, 0x01, 0x42, 0x00};
const mxfKey MXFWaveAudioEssenceDescriptor =
  {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01,
   0x0d, 0x01, 0x01, 0x01, 0x01, 0x01, 0x48, 0x00};
const mxfKey MXFAES3AudioEssenceDescriptor =
  {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01,
   0x0d, 0x01, 0x01, 0x01, 0x01, 0x01, 0x47, 0x00};
//
const mxfKey ObjectDirectory =
  {0x96, 0x13, 0xb3, 0x8a, 0x87, 0x34, 0x87, 0x46,
   0xf1, 0x02, 0x96, 0xf0, 0x56, 0xe0, 0x4d, 0x2a};

// Define map key <-> key name

#define MXF_DEFINE_PACK_KEY(n, k)     {#n, &n},
#define MXF_DEFINE_KEY(n, k)          {#n, &n},
#define MXF_CLASS(name, id, parent, concrete) {"MXF"#name, &name},

struct Key {
  const char* _name;
  const mxfKey* _key;
} mxfKeyTable [] = {
#include "MXFMetaDictionary.h"
  // keys not in MXFMetaDictionary.h
  {"V10RandomIndexMetadata", &V10RandomIndexMetadata},
  {"V10IndexTableSegment", &V10IndexTableSegment},
  {"SystemMetadata", &SystemMetadata},
  {"BogusFill", &BogusFill},
  //
  {"MXFGenericAudioEssenceDescriptor", &MXFGenericAudioEssenceDescriptor},
  {"MXFWaveAudioEssenceDescriptor", &MXFWaveAudioEssenceDescriptor},
  {"MXFAES3AudioEssenceDescriptor", &MXFAES3AudioEssenceDescriptor},
  //
  {"ObjectDirectory", &ObjectDirectory},
  //
  {"bogus", 0}
};

size_t mxfKeyTableSize = (sizeof(mxfKeyTable)/sizeof(mxfKeyTable[0])) - 1;

#define MXF_PROPERTY(name, id, tag, type, mandatory, isuid, container) \
{#name, tag},

struct {
  const char* _name;
  mxfUInt16 _key;
} mxfLocalKeyTable [] = {
#include "MXFMetaDictionary.h"
  // Sentinel
  {"bogus",                0x00}
};

size_t mxfLocalKeyTableSize =
                    (sizeof(mxfLocalKeyTable)/sizeof(mxfLocalKeyTable[0])) - 1;

bool lookupMXFLocalKey(mxfLocalKey& k, size_t& index)
{
  bool result = false;
  for (size_t i = 0; i < mxfLocalKeyTableSize; i++) {
    if (mxfLocalKeyTable[i]._key == k) {
      index = i;
      result = true;
      break;
    }
  }
  return result;
}

#define AAF_CLASS(name, id, parent, concrete) {"AAF"#name, {0}, id},

struct AAFKey {
  const char* _name;
  mxfKey _key;
  aafUID _aafKey;
} aafKeyTable [] = {
#include "AAFMetaDictionary.h"
  // keys not in AAFMetaDictionary.h
  {"Root", {0},
// {B3B398A5-1C90-11d4-8053-080036210804}
{0xb3b398a5, 0x1c90, 0x11d4,{0x80, 0x53, 0x08, 0x00, 0x36, 0x21, 0x08, 0x04}}},
  // Avid specific class extensions
// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
  {"AvidRLEDescriptor", {0},
{0xb54e6c75, 0xdf3a, 0x11d3,{0xa0, 0x78, 0x00, 0x60, 0x94, 0xeb, 0x75, 0xcb}}},
// {AD7212AF-21F4-11d5-B3B7-0006294303FA}
  {"AvidMPGIDescriptor", {0},
{0xad7212af, 0x21f4, 0x11d5,{0xb3, 0xb7, 0x00, 0x06, 0x29, 0x43, 0x03, 0xfa}}},
// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
  {"AvidSD2Descriptor", {0},
{0xf0c92c05, 0xefad, 0x11d3,{0xa0, 0x7a, 0x00, 0x60, 0x94, 0xeb, 0x75, 0xcb}}},
// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
  {"AvidMCMobReference", {0},
{0x6619f8e0, 0xfe77, 0x11d3,{0xa0, 0x84, 0x00, 0x60, 0x94, 0xeb, 0x75, 0xcb}}},
// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
  {"AvidAttrEffect", {0},
{0xc1be981a, 0x0037, 0x11d4,{0xa0, 0x85, 0x00, 0x60, 0x94, 0xeb, 0x75, 0xcb}}},
// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
  {"AvidCCParamObject", {0},
{0xd0541d8c, 0x00ec, 0x11d4,{0xa0, 0x86, 0x00, 0x60, 0x94, 0xeb, 0x75, 0xcb}}},
// {13E0A981-0412-11d4-9FF9-0004AC969F50}
  {"AvidTKMNTrackerData", {0},
{0x13e0a981, 0x0412, 0x11d4,{0x9f, 0xf9, 0x00, 0x04, 0xac, 0x96, 0x9f, 0x50}}},
// {30A42454-069E-11d4-9FFB-0004AC969F50}
  {"AvidTKMNTrackedParam", {0},
{0x30a42454, 0x069e, 0x11d4,{0x9f, 0xfb, 0x00, 0x04, 0xac, 0x96, 0x9f, 0x50}}},
// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
  {"AvidSourceFileDescriptor", {0},
{0xa4d2a4a6, 0x04bc, 0x11d4,{0xa0, 0x87, 0x00, 0x60, 0x94, 0xeb, 0x75, 0xcb}}},
  //
  {"AAFGenericSoundEssenceDescriptor", {0},
{0x0D010101, 0x0101, 0x4200,{0x06, 0x0E, 0x2B, 0x34, 0x02, 0x06, 0x01, 0x01}}},
  {"AAFMultipleDescriptor", {0},
{0x0D010101, 0x0101, 0x4400,{0x06, 0x0E, 0x2B, 0x34, 0x02, 0x06, 0x01, 0x01}}},
  // Sentinel
  {"bogus", {0},
// {00000000-0000-0000-0000-000000000000}
{0x00000000, 0x0000, 0x0000,{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}}
};

size_t aafKeyTableSize = (sizeof(aafKeyTable)/sizeof(aafKeyTable[0])) - 1;

int compareKey(const void* k1, const void* k2)
{
  const char* n1 = (*(Key**)k1)->_name;
  const char* n2 = (*(Key**)k2)->_name;
  return strcmp(n1, n2);
}

void dumpKeyTable(void)
{
    size_t i;
    Key** t = new Key*[mxfKeyTableSize];
    for (i = 0; i < mxfKeyTableSize; i++) {
      t[i] = &mxfKeyTable[i];
    }
    qsort(t, mxfKeyTableSize, sizeof(Key*), compareKey);
    for (i = 0; i < mxfKeyTableSize; i++) {
      fprintf(stdout, "%s\n", t[i]->_name);
      fprintf(stdout, "  ");
      printMxfKey(*t[i]->_key, stdout);
      fprintf(stdout, "\n");
    }
    delete [] t;
}

int compareAAFKey(const void* k1, const void* k2)
{
  const char* n1 = (*(AAFKey**)k1)->_name;
  const char* n2 = (*(AAFKey**)k2)->_name;
  return strcmp(n1, n2);
}

void dumpAAFKeyTable(void)
{
    size_t i;
    AAFKey** t = new AAFKey*[aafKeyTableSize];
    for (i = 0; i < aafKeyTableSize; i++) {
      t[i] = &aafKeyTable[i];
    }
    qsort(t, aafKeyTableSize, sizeof(AAFKey*), compareAAFKey);
    for (i = 0; i < aafKeyTableSize; i++) {
      fprintf(stdout, "%s\n", t[i]->_name);
      fprintf(stdout, "  ");
      printMxfKey(t[i]->_key, stdout);
      fprintf(stdout, "\n");
    }
    delete [] t;
}

void aafUIDToMxfKey(mxfKey& key, aafUID& aafID);

void aafUIDToMxfKey(mxfKey& key, aafUID& aafID)
{
  // Bottom half of key <- top half of aafID
  //
  key[ 0] = aafID.Data4[0];
  key[ 1] = aafID.Data4[1];
  key[ 2] = aafID.Data4[2];
  key[ 3] = aafID.Data4[3];
  key[ 4] = aafID.Data4[4];
  key[ 5] = aafID.Data4[5];
  key[ 6] = aafID.Data4[6];
  key[ 7] = aafID.Data4[7];

  // Top half of key <- bottom half of aafID
  //
  key[ 8] = (aafID.Data1 & 0xff000000) >> 24;
  key[ 9] = (aafID.Data1 & 0x00ff0000) >> 16;
  key[10] = (aafID.Data1 & 0x0000ff00) >>  8;
  key[11] = (aafID.Data1 & 0x000000ff);

  key[12] = (aafID.Data2 & 0xff00) >> 8;
  key[13] = (aafID.Data2 & 0x00ff);

  key[14] = (aafID.Data3 & 0xff00) >> 8;
  key[15] = (aafID.Data3 & 0x00ff);

  // If aafID is an AAF class AUID, map it to a SMPTE 336M local set key
  //
  mxfByte classIdPrefix[] = {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x06};
  if (memcmp(&key, &classIdPrefix, sizeof(classIdPrefix)) == 0) {
    key[5]  = 0x53;
  }
}

void initAAFKeyTable(void);

void initAAFKeyTable(void)
{
  for (size_t i = 0; i < aafKeyTableSize; i++) {
    aafUIDToMxfKey(aafKeyTable[i]._key, aafKeyTable[i]._aafKey);
  }
}

bool lookupAAFKey(mxfKey& k, size_t& index)
{
  bool result = false;
  for (size_t i = 0; i < aafKeyTableSize; i++) {
    if (memcmp(k, aafKeyTable[i]._key, sizeof(mxfKey)) == 0) {
      index = i;
      result = true;
      break;
    }
  }
  return result;
}

bool aafKeysAsSets = true;
bool bogusKeysAsSets = true;
bool darkKeysAsSets = false;

bool findAAFKey(mxfKey& k, size_t& index, char** flag)
{
  bool found = lookupAAFKey(k, index);
  if (found) {
    *flag = ""; // A valid SMPTE label
  } else {
    if (aafKeysAsSets) {
      // This could be an AUID/GUID that cannot be mapped to a SMPTE label.
      // Force the mapping and try again.
      mxfKey x;
      memcpy(x, k, sizeof(x));
      x[5] = 0x53;
      found = lookupAAFKey(x, index);
      if (found) {
        *flag = " +"; // A valid key but not a SMPTE label
      } else {
        if (bogusKeysAsSets) {
          // This could be a bogus key (Intel byte order GUID)
          // Fix it up and try again.
          aafUID a;
          memcpy(&a, k, sizeof(a));
          if (hostByteOrder() == 'B') {
            reorder(a);
          }
          mxfKey b;
          aafUIDToMxfKey(b, a);
          found = lookupAAFKey(b, index);
          if (found) {
            *flag = " -"; // A bogus key
          }
        }
      }
    }
  }
  return found;
}

bool isMxfKey(mxfKey& k);

bool isMxfKey(mxfKey& k)
{
  size_t index;
  bool result = lookupMXFKey(k, index);
  return result;
}

bool isAAFKey(mxfKey& k);

bool isAAFKey(mxfKey& k)
{
  size_t index;
  char* flag;
  bool found = findAAFKey(k, index, &flag);
  return found;
}

const mxfLocalKey nullMxfLocalKey = 0x00;

void checkSizes(void);

void checkSizes(void)
{
  if (sizeof(mxfLength) != 8) {
    fatalError("Wrong sizeof(mxfLength).\n");
  }
  if (sizeof(mxfKey) != 16) {
    fatalError("Wrong sizeof(mxfKey).\n");
  }
  if (sizeof(mxfByte) != 1) {
    fatalError("Wrong sizeof(mxfByte).\n");
  }
}

char map(int c);
void init(mxfUInt64 start, int base);
void flush(void);
void dumpByte(mxfByte byte);

// Interpret values 0x00 - 0x7f as ASCII characters.
//
static const char table[128] = {
'.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
'.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
'.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
'.',  '.',  '.',  '.',  '.',  '.',  '.',  '.',
' ',  '!',  '"',  '#',  '$',  '%',  '&', '\'',
'(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
'0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',
'8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
'@',  'A',  'B',  'C',  'D',  'E',  'F',  'G',
'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',
'X',  'Y',  'Z',  '[', '\\',  ']',  '^',  '_',
'`',  'a',  'b',  'c',  'd',  'e',  'f',  'g',
'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
'x',  'y',  'z',  '{',  '|',  '}',  '~',  '.'};

char map(int c)
{
  char result;
  if (c < 0x80) {
    result = table[c & 0x7f];
  } else {
    result = '.';
  }
  return result;
}

unsigned char buffer[16];
size_t bufferIndex;
size_t bufferStart;
size_t align;
mxfUInt32 address;
size_t addressBase;

void init(mxfUInt64 start, int base)
{
  bufferIndex = 0;
  bufferStart = bufferIndex;
  address = static_cast<mxfUInt32>(start);
  align = address % 16;
  addressBase = 10;
  if (base == 16) {
    addressBase = 16;
  }
}

void flush(void)
{
  if (bufferIndex > 0) {
    if (addressBase == 10) {
      printDecField(stdout, address);
    } else {
      printHexField(stdout, address);
    }
    fprintf(stdout, "  ");
    for (size_t x = 0; x < bufferStart; x++) {
      fprintf(stdout, "   ");
    }
    for (size_t i = bufferStart; i < bufferIndex; i++) {
      fprintf(stdout, "%02x ", buffer[i]);
    }
    fprintf(stdout, "   ");
    for (size_t j = bufferStart; j < 16 - bufferIndex; j++) {
      fprintf(stdout, "   ");
    }
    for (size_t z = 0; z < bufferStart; z++) {
      fprintf(stdout, " ");
    }
    for (size_t k = bufferStart; k < bufferIndex; k++) {
      char c = map(buffer[k]);
      fprintf(stdout, "%c", c);
    }
    fprintf(stdout, "\n");
  }
}

void dumpByte(mxfByte byte)
{
  if (bufferIndex == 16) {
    flush();
    bufferIndex = 0;
    bufferStart = bufferIndex;
    address = address + 16;
  } else if (bufferIndex == 0) {
    if (align != 0) {
      // align
      bufferIndex = align;
      bufferStart = bufferIndex;
      address = address - align;
    }
  }
  buffer[bufferIndex++] = byte;
}

void printCommonOptions(void);

void printCommonOptions(void)
{
  fprintf(stderr, "--help                = ");
  fprintf(stderr, "print this message and exit (-h)\n");
  fprintf(stderr, "\n");
}

void printCommonDumpOptions(void);

void printCommonDumpOptions(void)
{
  fprintf(stderr, "  --key-addresses     = ");
  fprintf(stderr, "print key addresses ");
  fprintf(stderr, "- always absolute [default] (-j)\n");

  fprintf(stderr, "  --no-key-addresses  = ");
  fprintf(stderr, "don't print key addresses\n");

  fprintf(stderr, "  --relative          = ");
  fprintf(stderr, "relative addresses ");
  fprintf(stderr, "- value start = 0 [default] (-r)\n");

  fprintf(stderr, "  --absolute          = ");
  fprintf(stderr, "absolute addresses ");
  fprintf(stderr, "- file start = 0 (-b)\n");

  fprintf(stderr, "  --decimal           = ");
  fprintf(stderr, "print addresses in ");
  fprintf(stderr, "decimal (-t)\n");

  fprintf(stderr, "  --hexadecimal       = ");
  fprintf(stderr, "print addresses in ");
  fprintf(stderr, "hexadecimal [default] (-x)\n");

  fprintf(stderr, "  --symbolic          = ");
  fprintf(stderr, "dump the names of keys if known [default] (-y)\n");

  fprintf(stderr, "  --no-symbolic       = ");
  fprintf(stderr, "don't dump the names of keys (-n)\n");
  fprintf(stderr, "\n");

  fprintf(stderr, "  --verbose           = ");
  fprintf(stderr, "print more detailed information (-v)\n");

  fprintf(stderr, "  --debug             = ");
  fprintf(stderr, "print information useful in debugging this program (-d)\n");
  fprintf(stderr, "\n");
}

void printCommonValidateOptions(void);

void printCommonValidateOptions(void)
{
}

void printFormatOptions(void);

void printFormatOptions(void)
{
  fprintf(stderr, "  --limit-bytes <n>   = ");
  fprintf(stderr, "truncate essence ");
  fprintf(stderr, "to <n> bytes [default n = 0] (-l)\n");

  fprintf(stderr, "  --no-limit-bytes    = ");
  fprintf(stderr, "do not truncate ");
  fprintf(stderr, "essence (-e)\n");

  fprintf(stderr, "  --frames            = ");
  fprintf(stderr, "assume essence is ");
  fprintf(stderr, "wrapped frames (-p)\n");

  fprintf(stderr, "  --limit-entries <n> = ");
  fprintf(stderr, "print only the first <n> ");
  fprintf(stderr, "index table entries (-c)\n");

  fprintf(stderr, "  --no-limit-entries  = ");
  fprintf(stderr, "print all ");
  fprintf(stderr, "index table entries\n");

  fprintf(stderr, "  --show-fill         = ");
  fprintf(stderr, "dump fill bytes (-f)\n");

  fprintf(stderr, "  --show-dark         = ");
  fprintf(stderr, "dump dark data (-w)\n");

  fprintf(stderr, "  --show-run-in       = ");
  fprintf(stderr, "dump run-in\n");
}

void printRawOptions(void);

void printRawOptions(void)
{
  fprintf(stderr, "  --limit-bytes <n>   = ");
  fprintf(stderr, "truncate values ");
  fprintf(stderr, "to <n> bytes (-l)\n");

  fprintf(stderr, "  --no-limit-bytes    = ");
  fprintf(stderr, "do not truncate ");
  fprintf(stderr, "values [default] (-e)\n");
}

void printAAFOptions(void);

void printAAFOptions(void)
{
  printFormatOptions();
}

void printMXFOptions(void);

void printMXFOptions(void)
{
  printFormatOptions();
}

void printSetOptions(void);

void printSetOptions(void)
{
  printFormatOptions();
}

void printKLVOptions(void);

void printKLVOptions(void)
{
  printRawOptions();
}

void printKLVValidateOptions(void);

void printKLVValidateOptions(void)
{
}

void printSetValidateOptions(void);

void printSetValidateOptions(void)
{
}

void printMXFValidateOptions(void);

void printMXFValidateOptions(void)
{
}

void printAAFValidateOptions(void);

void printAAFValidateOptions(void)
{
}

void printUsage(void);

void printUsage(void)
{
  fprintf(stderr,
          "%s : Usage : %s OPTIONS <file>\n",
          programName(),
          programName());

  fprintf(stderr, "--aaf-dump            = ");
  fprintf(stderr, "dump AAF (-a)\n");

  fprintf(stderr, "--mxf-dump            = ");
  fprintf(stderr, "dump MXF [default] (-m)\n");

  fprintf(stderr, "--set-dump            = ");
  fprintf(stderr, "dump local sets (-s)\n");

  fprintf(stderr, "--klv-dump            = ");
  fprintf(stderr, "dump raw KLV (-k)\n");

  fprintf(stderr, "--aaf-validate        = ");
  fprintf(stderr, "validate AAF\n");

  fprintf(stderr, "--mxf-validate        = ");
  fprintf(stderr, "validate MXF\n");

  fprintf(stderr, "--set-validate        = ");
  fprintf(stderr, "validate local sets\n");

  fprintf(stderr, "--klv-validate        = ");
  fprintf(stderr, "validate KLV\n");

  fprintf(stderr, "--help                = ");
  fprintf(stderr, "print detailed help (-h)\n");
}

void printAAFUsage(void);

void printAAFUsage(void)
{
  fprintf(stderr, "--aaf-dump            = ");
  fprintf(stderr, "dump AAF (-a)\n");
  printAAFOptions();
  fprintf(stderr, "\n");
}

void printMXFUsage(void);

void printMXFUsage(void)
{
  fprintf(stderr, "--mxf-dump            = ");
  fprintf(stderr, "dump MXF [default] (-m)\n");
  printMXFOptions();
  fprintf(stderr, "\n");
}

void printSetUsage(void);

void printSetUsage(void)
{
  fprintf(stderr, "--set-dump            = ");
  fprintf(stderr, "dump local sets (-s)\n");
  printSetOptions();
  fprintf(stderr, "\n");
}

void printKLVUsage(void);

void printKLVUsage(void)
{
  fprintf(stderr, "--klv-dump            = ");
  fprintf(stderr, "dump raw KLV (-k)\n");
  printKLVOptions();
  fprintf(stderr, "\n");
}

void printKLVValidateUsage(void);

void printKLVValidateUsage(void)
{
  fprintf(stderr, "--klv-validate        = ");
  fprintf(stderr, "validate KLV\n");
  printKLVValidateOptions();
  fprintf(stderr, "\n");
}

void printSetValidateUsage(void);

void printSetValidateUsage(void)
{
  fprintf(stderr, "--set-validate        = ");
  fprintf(stderr, "validate local sets\n");
  printSetValidateOptions();
  fprintf(stderr, "\n");
}

void printMXFValidateUsage(void);

void printMXFValidateUsage(void)
{
  fprintf(stderr, "--mxf-validate        = ");
  fprintf(stderr, "validate MXF\n");
  printMXFValidateOptions();
  fprintf(stderr, "\n");
}

void printAAFValidateUsage(void);

void printAAFValidateUsage(void)
{
  fprintf(stderr, "--aaf-validate        = ");
  fprintf(stderr, "validate AAF\n");
  printAAFValidateOptions();
  fprintf(stderr, "\n");
}

void printFullUsage(void);

void printFullUsage(void)
{
  fprintf(stderr,
          "%s : Usage : %s OPTIONS <file>\n",
          programName(),
          programName());

  printAAFUsage();
  printMXFUsage();
  printSetUsage();
  printKLVUsage();

  printCommonDumpOptions();

  printAAFValidateUsage();
  printMXFValidateUsage();
  printSetValidateUsage();
  printKLVValidateUsage();

  printCommonOptions();
}

void printHelp(void);

void printHelp(void)
{
  if (mode == klvMode) {
    printKLVUsage();
    printCommonDumpOptions();
    printCommonOptions();
  } else if (mode == localSetMode) {
    printSetUsage();
    printCommonDumpOptions();
    printCommonOptions();
  } else if (mode == mxfMode) {
    printMXFUsage();
    printCommonDumpOptions();
    printCommonOptions();
  } else if (mode == aafMode) {
    printAAFUsage();
    printCommonDumpOptions();
    printCommonOptions();
  } else if (mode == klvValidateMode) {
    printKLVValidateUsage();
    printCommonValidateOptions();
    printCommonOptions();
  } else if (mode == setValidateMode) {
    printSetValidateUsage();
    printCommonValidateOptions();
    printCommonOptions();
  } else if (mode == mxfValidateMode) {
    printMXFValidateUsage();
    printCommonValidateOptions();
    printCommonOptions();
  } else if (mode == aafValidateMode) {
    printAAFValidateUsage();
    printCommonValidateOptions();
    printCommonOptions();
  } else {
    printFullUsage();
  }
}

const char* baseName(char* fullName)
{
  char* result;
#if defined(MXF_OS_WINDOWS)
  const int delimiter = '\\';
#elif defined(MXF_OS_MACOS)
  const int delimiter = ':';
#else
  const int delimiter = '/';
#endif
  result = strrchr(fullName, delimiter);
  if (result == 0) {
    result = fullName;
  } else if (strlen(result) == 0) {
    result = fullName;
  } else {
    result++;
  }

  return result;
}

#define AAF_PROPERTY(name, id, tag, type, mandatory, uid, container) \
{#name, tag},

struct {
  const char* _name;
  mxfUInt16 _key;
} aafLocalKeyTable [] = {
#include "AAFMetaDictionary.h"
  // local keys not in AAFMetaDictionary.h
  {"MetaDictionary",       0x0001},
  {"Header",               0x0002},
  // All objects
  {"InstanceUID",          0x3c0a},
  // Preface
  {"Primary Package",      0x3b08},
  {"Operational Pattern",  0x3b09},
  {"Essence Containers",   0x3b0a},
  {"DM Schemes",           0x3b0b},
  // Index Table
  {"Edit Unit Byte Count", 0x3f05},
  {"IndexSID",             0x3f06},
  {"BodySID",              0x3f07},
  {"Slice Count",          0x3f08},
  {"Delta Entry Array",    0x3f09},
  {"Index Entry Array",    0x3f0a},
  {"Index Edit Rate",      0x3f0b},
  {"Index Start Position", 0x3f0c},
  {"Index Duration",       0x3f0d},
  // File Descriptor
  {"Linked Track ID",      0x3006},
  // Multiple Descriptor
  {"Sub Descriptor UIDs",  0x3f01},
  // Generic Sound Essence Descriptor
  {"Locked/Unlocked",      0x3d02},
  //
  // Generic Picture Essence Descriptor
  {"Signal Standard",      0x3215},
  {"Stored F2 Offset",     0x3216},
  {"Display F2 Offset",    0x3217},
  // CDCI Essence Descriptor
  {"Reversed Byte Order",  0x330b},
  // Sentinel
  {"bogus",                0x00}
};

size_t aafLocalKeyTableSize =
                    (sizeof(aafLocalKeyTable)/sizeof(aafLocalKeyTable[0])) - 1;

bool lookupAAFLocalKey(mxfLocalKey& k, size_t& index)
{
  bool result = false;
  for (size_t i = 0; i < aafLocalKeyTableSize; i++) {
    if (aafLocalKeyTable[i]._key == k) {
      index = i;
      result = true;
      break;
    }
  }
  return result;
}

bool symbolic = true;

void printMxfLocalKeySymbol(mxfLocalKey& k, mxfKey& enclosing);

void printMxfLocalKeySymbol(mxfLocalKey& k, mxfKey& enclosing)
{
  if (isMxfKey(enclosing)) {
    size_t i;
    bool found = lookupMXFLocalKey(k, i);
    if (found) {
      fprintf(stdout, "%s\n", mxfLocalKeyTable[i]._name);
    } else {
      fprintf(stdout, "Dark\n");
    }
  } else {
    fprintf(stdout, "Dark\n");
  }
}

void printAAFLocalKeySymbol(mxfLocalKey& k, mxfKey& enclosing);

void printAAFLocalKeySymbol(mxfLocalKey& k, mxfKey& enclosing)
{
  if (isAAFKey(enclosing)) {
    size_t i;
    bool found = lookupAAFLocalKey(k, i);
    if (found) {
      fprintf(stdout, "%s\n", aafLocalKeyTable[i]._name);
    } else {
      fprintf(stdout, "Dark\n");
    }
  } else {
    fprintf(stdout, "Dark\n");
  }
}

void printMxfLocalKeySymbol(mxfLocalKey& k, mxfKey& enclosing, FILE* f);

void printMxfLocalKeySymbol(mxfLocalKey& k, mxfKey& enclosing, FILE* f)
{
  if (symbolic) {
    if (mode == aafMode) {
      printAAFLocalKeySymbol(k, enclosing);
    } else if (mode == mxfMode) {
      printMxfLocalKeySymbol(k, enclosing);
    } else if (mode == klvMode) {
      printMxfLocalKeySymbol(k, enclosing);
    } else if (mode == localSetMode) {
      printMxfLocalKeySymbol(k, enclosing);
    } else {
      fprintf(f, "Unknown\n");
    }
  } else {
    fprintf(f, "\n");
  }
  fprintf(f, "  ");
  printMxfLocalKey(k, f);
}

bool lookupMXFKey(mxfKey& k, size_t& index)
{
  bool result = false;
  for (size_t i = 0; i < mxfKeyTableSize; i++) {
    if (memcmp(k, mxfKeyTable[i]._key, sizeof(mxfKey)) == 0) {
      index = i;
      result = true;
      break;
    }
  }
  return result;
}

bool keyAddresses = true;  // Print addresses of keys
bool relative = true;      // Print relative (not absolute) addresses
int base = 16;             // Base for key addreses

void printKeyAddress(FILE* f, mxfUInt64 keyAddress);

void printKeyAddress(FILE* f, mxfUInt64 keyAddress)
{
  fprintf(f, "( ");
  if (base == 10) {
    printDecField(f, keyAddress);
  } else {
    printHexFieldPad(f, keyAddress);
  }
  fprintf(f, " )");
}

void printMxfKeySymbol(mxfKey& k);

void printMxfKeySymbol(mxfKey& k)
{
  size_t i;
  bool found = lookupMXFKey(k, i);
  if (found) {
    fprintf(stdout, "%s", mxfKeyTable[i]._name);
  } else if (isEssenceElement(k)) {
    fprintf(stdout, "Essence Element");
  } else {
    fprintf(stdout, "Dark");
  }
  if (keyAddresses) {
    fprintf(stdout, " ");
    printKeyAddress(stdout, keyPosition);
  }
  fprintf(stdout, "\n");
}

void printAAFKeySymbol(mxfKey& k);

void printAAFKeySymbol(mxfKey& k)
{
  size_t i;
  char* flag;
  bool found = findAAFKey(k, i, &flag);
  if (found) {
    fprintf(stdout, "%s%s", aafKeyTable[i]._name, flag);
  } else {
    found = lookupMXFKey(k, i);
    if (found) {
      fprintf(stdout, "%s", mxfKeyTable[i]._name);
    } else if (isEssenceElement(k)) {
      fprintf(stdout, "Essence Element");
    } else {
      fprintf(stdout, "Dark");
    }
  }
  if (keyAddresses) {
    fprintf(stdout, " ");
    printKeyAddress(stdout, keyPosition);
  }
  fprintf(stdout, "\n");
}

void printMxfKeySymbol(mxfKey& k, FILE* f);

void printMxfKeySymbol(mxfKey& k, FILE* f)
{
  if (symbolic) {
    if (mode == aafMode) {
      printAAFKeySymbol(k);
    } else if (mode == mxfMode) {
      printMxfKeySymbol(k);
    } else if (mode == klvMode) {
      printMxfKeySymbol(k);
    } else if (mode == localSetMode) {
      printMxfKeySymbol(k);
    } else {
      fprintf(f, "Unknown\n");
    }
  } else {
    fprintf(f, "\n");
  }
  printMxfKey(k, f);
}

mxfUInt32 darkItems = 0;

void printKL(mxfKey& k, mxfLength& l);

void printKL(mxfKey& k, mxfLength& l)
{
  if (darkItems > 0) {
    fprintf(stdout, "\n");
    fprintf(stdout, "[ Omitted %"MXFPRIu32" dark KLV triplets ]\n", darkItems);
    darkItems = 0;
  }
  fprintf(stdout, "\n");
  fprintf(stdout, "[ K = ");
  printMxfKeySymbol(k, stdout);
  fprintf(stdout, ", ");
  fprintf(stdout, "L = ");
  printMxfLength(l, stdout);
  fprintf(stdout, " ]\n");
}

void printV(mxfLength& length,
            bool limitBytes,
            mxfUInt32 limit,
            mxfFile infile);

void printV(mxfLength& length,
            bool limitBytes,
            mxfUInt32 limit,
            mxfFile infile)
{
  mxfUInt64 start = position(infile);
  if (relative) {
    start = 0;
  }
  init(start, base);

  mxfLength count = 0;
  for (mxfLength i = 0; i < length; i++) {
    mxfByte b;
    readMxfUInt08(b, infile);
    if (limitBytes && (i == limit)) {
      break;
    }
    dumpByte(b);
    count = count + 1;
  }
  flush();
  if (count < length) {
    fprintf(stdout, "[ Value truncated from ");
    printDecField(stdout, length);
    fprintf(stdout, " bytes to ");
    printDecField(stdout, limit);
    fprintf(stdout, " bytes ]\n");

    mxfUInt64 skipLength = (length - count) - 1;
    skipV(skipLength, infile);
  }
}

void printRunIn(mxfUInt64& headerPosition,
                bool limitBytes,
                mxfUInt32 limit,
                mxfFile infile);

void printRunIn(mxfUInt64& headerPosition,
                bool limitBytes,
                mxfUInt32 limit,
                mxfFile infile)
{
  setPosition(infile, 0);
  fprintf(stdout, "\n");
  fprintf(stdout, "[ %"MXFPRIu64" bytes of run-in ]\n", headerPosition);
  if (lFlag) {
    printV(headerPosition, limitBytes, limit, infile);
  } else {
    printV(headerPosition, false, 64, infile);
  }
}

char* itemTypeName(mxfByte itemTypeId);

char* itemTypeName(mxfByte itemTypeId)
{
  char* result;
  switch (itemTypeId) {
  case 0x05:
    result = "CP Picture";
    break;
  case 0x06:
    result = "CP Sound";
    break;
  case 0x07:
    result = "CP Data";
    break;
  case 0x15:
    result = "GC Picture";
    break;
  case 0x16:
    result = "GC Sound";
    break;
  case 0x17:
    result = "GC Data";
    break;
  case 0x18:
    result = "GC Compound";
    break;
  default:
    result = "Unknown";
    break;
  }
  return result;
}

char* CPPictureElementTypeName(mxfByte type);

char* CPPictureElementTypeName(mxfByte type)
{
  char* result = "Unknown Picture";
  if (type == 0x01) {
    result = "MPEG-2 (D10)";
  } else if (type == 0x41) {
    result = "JFIF";
  } else if (type == 0x42) {
    result = "TIFF";
  } else if ((type < 0x01) || (type > 0x7f)) {
    result = "Illegal";
  }
  return result;
}

char* CPSoundElementTypeName(mxfByte type);

char* CPSoundElementTypeName(mxfByte type)
{
  char* result = "Unknown Sound";
  if (type == 0x10) {
    result = "AES3";
  } else if (type == 0x40) {
    result = "WAVE";
  } else if ((type < 0x01) || (type > 0x7f)) {
    result = "Illegal";
  }
  return result;
}

char* CPDataElementTypeName(mxfByte type);

char* CPDataElementTypeName(mxfByte type)
{
  char* result = "Unknown Data";
  if ((type < 0x01) || (type > 0x7f)) {
    result = "Illegal";
  }
  return result;
}

char* GCPictureElementTypeName(mxfByte type);

char* GCPictureElementTypeName(mxfByte type)
{
  char* result = "Unknown Picture";
  if (type == 0x01) {
    result = "Compressed HD (D11)";
  } else if (type == 0x02) {
    result = "Uncompressed (Frame Wrapped)";
  } else if (type == 0x02) {
    result = "Uncompressed (Clip Wrapped)";
  } else if (type == 0x04) {
    result = "Uncompressed (Line Wrapped)";
  } else if ((type < 0x01) || (type > 0x7f)) {
    result = "Illegal";
  }
  return result;
}

char* avidPictureElementTypeName(mxfByte type);

char* avidPictureElementTypeName(mxfByte type)
{
  char* result = "Unknown Picture";
  if (type == 0x01) {
    result = "Avid JFIF";
  } else if (type == 0x02) {
    result = "Avid DV";
  } else if (type == 0x03) {
    result = "Avid MPEG";
  } else if (type == 0x04) {
    result = "Avid Uncompressed SD";
  } else if (type == 0x05) {
    result = "Avid Uncompressed HD";
  } else if (type == 0x06) {
    result = "Avid Compressed HD";
  } else if (type == 0x07) {
    result = "Avid Packed 10-bit";
  } else if (type == 0x08) {
    result = "Avid RGBA";
  } else if (type == 0x09) {
    result = "Avid RLE";
  }
  return result;
}

char* GCSoundElementTypeName(mxfByte type);

char* GCSoundElementTypeName(mxfByte type)
{
  char* result = "Unknown Sound";
  if (type == 0x01) {
    result = "Broadcast Wave (Frame Wrapped)";
  } else if (type == 0x02) {
    result = "Broadcast Wave (Clip Wrapped)";
  } else if (type == 0x03) {
    result = "AES3( Frame Wrapped)";
  } else if (type == 0x04) {
    result = "AES3 (Clip Wrapped)";
  } else if ((type < 0x01) || (type > 0x7f)) {
    result = "Illegal";
  }
  return result;
}

char* avidSoundElementTypeName(mxfByte type);

char* avidSoundElementTypeName(mxfByte type)
{
  char* result = "Unknown Sound";
  if (type == 0x20) {
    result = "Avid Sound";
  }
  return result;
}

char* GCDataElementTypeName(mxfByte type);

char* GCDataElementTypeName(mxfByte type)
{
  char* result = "Unknown Data";
  if ((type < 0x01) || (type > 0x7f)) {
    result = "Illegal";
  }
  return result;
}

char* GCCompoundElementTypeName(mxfByte type);

char* GCCompoundElementTypeName(mxfByte type)
{
  char* result = "Unknown Compound";
  if (type == 0x01) {
    result = "DV-DIF (Frame Wrapped)";
  } else if (type == 0x02) {
    result = "DV-DIF (Clip Wrapped)";
  } else if ((type < 0x01) || (type > 0x7f)) {
    result = "Illegal";
  }
  return result;
}

char* elementTypeName(mxfByte itemTypeId, mxfByte type);

char* elementTypeName(mxfByte itemTypeId, mxfByte type)
{
  char* result = "Unknown";
  switch (itemTypeId) {
  case 0x05: // "CP Picture"
    result = CPPictureElementTypeName(type);
    break;
  case 0x06: // "CP Sound"
    result = CPSoundElementTypeName(type);
    break;
  case 0x07: // "CP Data"
    result = CPDataElementTypeName(type);
    break;
  case 0x15: // "GC Picture"
    result = GCPictureElementTypeName(type);
    break;
  case 0x16: // "GC Sound"
    result = GCSoundElementTypeName(type);
    break;
  case 0x17: // "GC Data"
    result = GCDataElementTypeName(type);
    break;
  case 0x18: // "GC Compound"
    result = GCCompoundElementTypeName(type);
    break;
  default:
    break;
  }
  return result;
}

char* avidElementTypeName(mxfByte itemTypeId, mxfByte type);

char* avidElementTypeName(mxfByte itemTypeId, mxfByte type)
{
  char* result = "Unknown";
  switch (itemTypeId) {
  case 0x15: // "GC Picture"
    result = avidPictureElementTypeName(type);
    break;
  case 0x16: // "GC Sound"
    result = avidSoundElementTypeName(type);
    break;
  default:
    break;
  }
  return result;
}

bool isPredefinedEssenceElement(mxfKey& k);

bool isPredefinedEssenceElement(mxfKey& k)
{
  // Prefix for MXF predefined MXF essence element labels
  mxfKey pe = {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x02, 0x01, 0x01,
               0x0d, 0x01, 0x03, 0x01, 0xff, 0xff, 0xff, 0xff};
  bool result;
  if (memcmp(&k, &pe, 12) == 0) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

bool isAvidEssenceElement(mxfKey& k);

bool isAvidEssenceElement(mxfKey& k)
{
  // Prefix for Avid defined essence element labels
  mxfKey ae = {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x02, 0x01, 0x01,
               0x0e, 0x04, 0x03, 0x01, 0xff, 0xff, 0xff, 0xff};
  bool result;
  if (memcmp(&k, &ae, 12) == 0) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

bool isEssenceElement(mxfKey& k)
{
  bool result;
  if (isPredefinedEssenceElement(k)) {
    result = true;
  } else if (isAvidEssenceElement(k)) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

void printEssenceKL(mxfKey& k, mxfLength& len);

void printEssenceKL(mxfKey& k, mxfLength& len)
{
  mxfByte itemTypeId = k[12];
  mxfByte elementTypeId = k[14];

  char* itemTypeIdName = itemTypeName(itemTypeId);
  char* elementTypeIdName;
  if (isAvidEssenceElement(k)) {
    elementTypeIdName = avidElementTypeName(itemTypeId, elementTypeId);
  } else {
    elementTypeIdName = elementTypeName(itemTypeId, elementTypeId);
  }

  int elementCount = k[13];
  int elementNumber = k[15];


  fprintf(stdout,
          "  Track          = %02x.%02x.%02x.%02x\n",
          k[12],
          k[13],
          k[14],
          k[15]);
  fprintf(stdout,
          "  Item Type      = \"%s\" (%02x)\n",
          itemTypeIdName,
          itemTypeId);
  fprintf(stdout,
          "  Element Type   = \"%s\" (%02x)\n",
          elementTypeIdName,
          elementTypeId);
  fprintf(stdout,
          "  Element Count  = %d (%02x)\n",
          elementCount,
          elementCount);
  fprintf(stdout,
          "  Element Number = %d (%02x)\n",
          elementNumber,
          elementNumber);
}

void printEssenceFrameFill(mxfKey& k,
                           mxfLength& length,
                           mxfUInt32 frameCount,
                           mxfFile infile);

void printEssenceFrameFill(mxfKey& k,
                           mxfLength& length,
                           mxfUInt32 frameCount,
                           mxfFile infile)
{
  if ((frameCount < maxFrames) || !iFlag) {
    printFill(k, length, infile);
  } else {
    skipV(length, infile);
  }
}

void printEssenceFrameValue(mxfKey& k,
                            mxfLength& length,
                            mxfUInt32 frameCount,
                            bool limitBytes,
                            mxfUInt32 limit,
                            mxfFile infile);

void printEssenceFrameValue(mxfKey& k,
                            mxfLength& length,
                            mxfUInt32 frameCount,
                            bool limitBytes,
                            mxfUInt32 limit,
                            mxfFile infile)
{
  if ((frameCount < maxFrames) || !iFlag) {
    fprintf(stdout, "\n");
    fprintf(stdout, "[ Frame = ");
    printDecField(stdout, frameCount);
    fprintf(stdout, "]");

    printKL(k, length);
    printV(length, limitBytes, limit, infile);
  } else {
    skipV(length, infile);
  }
}

void printEssenceFrames(mxfKey& k,
                        mxfLength& length,
                        bool limitBytes,
                        mxfUInt32 limit,
                        mxfFile infile);

void printEssenceFrames(mxfKey& k,
                        mxfLength& length,
                        bool limitBytes,
                        mxfUInt32 limit,
                        mxfFile infile)
{
  mxfUInt32 frameCount = 0;
  mxfLength total = 0;

  printEssenceKL(k, length);
  while (total < length) {
    mxfKey k;
    readMxfKey(k, infile);
    mxfLength len;
    int lengthLen = readMxfLength(len, infile);
    total = total + lengthLen;

    if (isFill(k)) {
      printEssenceFrameFill(k, len, frameCount, infile);
    } else {
      printEssenceFrameValue(k, len, frameCount, limitBytes, limit, infile);
      frameCount = frameCount + 1;
    }
    total = total + len;
  }
  if (iFlag && (maxFrames < frameCount)) {
    fprintf(stdout, "[ Essence frames truncated from ");
    printDecField(stdout, frameCount);
    fprintf(stdout, " frames to ");
    printDecField(stdout, maxFrames);
    fprintf(stdout, " frames ]\n");
  }
}

void printEssence(mxfKey& k,
                  mxfLength& length,
                  bool limitBytes,
                  mxfUInt32 limit,
                  mxfFile infile);

void printEssence(mxfKey& k,
                  mxfLength& length,
                  bool limitBytes,
                  mxfUInt32 limit,
                  mxfFile infile)
{
  printEssenceKL(k, length);
  printV(length, limitBytes, limit, infile);
}

void printEssenceElement(mxfKey& k,
                         mxfLength& length,
                         bool limitBytes,
                         mxfUInt32 limit,
                         mxfFile infile)
{
  if (frames) {
    printEssenceFrames(k, length, limitBytes, limit, infile);
  } else {
    printEssence(k, length, limitBytes, limit, infile);
  }
}

void skipV(mxfLength& length, mxfFile infile)
{
  // Seek past (length - 1) bytes then read a byte
  if (length != 0) {
    skipBytes(length - 1, infile);
    mxfUInt08 x;
    readMxfUInt08(x, infile);
  }
}

void printLocalKL(mxfLocalKey& k, mxfUInt16& l, mxfKey& enclosing);

void printLocalKL(mxfLocalKey& k, mxfUInt16& l, mxfKey& enclosing)
{
  fprintf(stdout, "  [ k = ");
  printMxfLocalKeySymbol(k, enclosing, stdout);
  fprintf(stdout,
          ", l = %5d (%04x) ]\n",
          l,
          l);
}

void printLocalV(mxfUInt16& length,
                 mxfLength& remainder,
                 mxfFile infile);

void printLocalV(mxfUInt16& length,
                 mxfLength& remainder,
                 mxfFile infile)
{
  mxfLength vLength;
  if (length < remainder) {
    vLength = length;
  } else {
    vLength = remainder;
  }
  printV(vLength, false, 0, infile);
  remainder = remainder - vLength;
  if (vLength < length) {
    mxfError("Local set KLV parse error - set exhausted printing value"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
  }
}

void checkLocalKey(mxfLocalKey& k);

void checkLocalKey(mxfLocalKey& k)
{
  if (k == nullMxfLocalKey) {
    mxfError("Null local key"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
  }
}

mxfUInt64 checkLength(mxfUInt64 length, mxfUInt64 fileSize, mxfUInt64 position)
{
  mxfUInt64 result;
  mxfUInt64 remainder = fileSize - position;
  if (length > remainder) {
    mxfError("Length points beyond end of file"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
    result = remainder;
  } else {
    result = length;
  }
  return result;
}

void checkKey(mxfKey& k)
{
  if (isNullKey(k)) {
    mxfError("Null key at offset 0x%"MXFPRIx64".\n",
             keyPosition);
  }
}

bool isDark(mxfKey& k, Mode mode)
{
  char* flag;
  bool result = false;
  size_t x;
  switch (mode) {
  case klvMode:
    result = false;
    break;
  case localSetMode:
    result = false;
    break;
  case mxfMode:
    result = !lookupMXFKey(k, x);
    break;
  case aafMode: {
      bool found = findAAFKey(k, x, &flag);
      if (!found) {
        found = lookupMXFKey(k, x);
      }
      result = !found;
    }
    break;
  default:
    result = false;
    break;
  }
  return result;
}

bool dumpFill = false;

bool isFill(mxfKey& k)
{
  bool result;
  if (memcmp(&KLVFill, &k, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&BogusFill, &k, sizeof(mxfKey)) == 0) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

void printFill(mxfKey& k, mxfLength& len, mxfFile infile)
{
  if (dumpFill) {
    printV(len, false, 0, infile);
  } else {
    skipV(len, infile);
  }
}

bool isNullKey(mxfKey& k)
{
  bool result;
  if (memcmp(NullKey, k, sizeof(mxfKey)) == 0) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

bool isLocalSet(mxfKey& k);

bool isLocalSet(mxfKey& k)
{
  char* flag;
  bool result;
  if (k[5] == 0x53) {
    result = true;
  } else {
    if (aafKeysAsSets) {
      size_t index;
      bool found = findAAFKey(k, index, &flag);
      if (found) {
        result = true;
      } else {
        result = false;
      }
    } else {
      result = false;
    }
  }
  return result;
}

void skipBogusBytes(const mxfUInt64 byteCount, mxfFile infile);

void skipBogusBytes(const mxfUInt64 byteCount, mxfFile infile)
{
  mxfUInt64 length = byteCount;
  fprintf(stdout, "[ * Error * %"MXFPRIu64" superfluous bytes ]\n", length);
  printV(length, false, 0, infile);
}

void printLocalKey(mxfLocalKey& identifier,
                   mxfLength& remainder,
                   mxfFile infile);

void printLocalKey(mxfLocalKey& identifier,
                   mxfLength& remainder,
                   mxfFile infile)
{
  if (remainder > 2) {
    readMxfLocalKey(identifier, infile);
    checkLocalKey(identifier);
    remainder = remainder - 2;
  } else {
    mxfError("Local set KLV parse error - set exhausted looking for local key"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
    skipBogusBytes(remainder, infile);
    remainder = 0;
  }
}

void printLocalLength(mxfUInt16& length,
                      mxfLength& remainder,
                      mxfFile infile);

void printLocalLength(mxfUInt16& length,
                      mxfLength& remainder,
                      mxfFile infile)
{
  if (remainder > 2) {
    readMxfUInt16(length, infile);
    remainder = remainder - 2;
  } else {
    mxfError("Local set KLV parse error -"
             " set exhausted looking for local length"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
    skipBogusBytes(remainder, infile);
    remainder = 0;
  }
}

void printLocalSet(mxfKey& k, mxfLength& len, mxfFile infile);

void printLocalSet(mxfKey& k, mxfLength& len, mxfFile infile)
{
  mxfLength remainder = len;
  while (remainder > 0) {
    // Key (local identifier)
    mxfLocalKey identifier;
    printLocalKey(identifier, remainder, infile);
    if (remainder == 0) {
      break;
    }

    // Length
    mxfUInt16 length;
    printLocalLength(length, remainder, infile);
    if (remainder == 0) {
      break;
    }

    // Value
    printLocalKL(identifier, length, k);
    printLocalV(length, remainder, infile);
  }
}

void checkElementSize(mxfUInt32 expectedSize,
                      mxfUInt32 actualSize,
                      mxfUInt32 elementCount);

void checkElementSize(mxfUInt32 expectedSize,
                      mxfUInt32 actualSize,
                      mxfUInt32 elementCount)
{
  if (actualSize != expectedSize) {
    if ((elementCount == 0) && (actualSize == 0)) {
      mxfWarning("Incorrect element size"
                 " - expected %"MXFPRIu32", found %"MXFPRIu32""
                 " (following key at offset 0x%"MXFPRIx64").\n",
                 expectedSize,
                 actualSize,
                 keyPosition);
    } else {
      mxfError("Incorrect element size"
               " - expected %"MXFPRIu32", found %"MXFPRIu32""
               " (following key at offset 0x%"MXFPRIx64").\n",
               expectedSize,
               actualSize,
               keyPosition);
    }
  }
}

void checkElementCount(mxfUInt32 elementCount,
                       mxfUInt32 elementSize,
                       mxfUInt64 length,
                       mxfUInt64 fixedSize);

void checkElementCount(mxfUInt32 elementCount,
                       mxfUInt32 elementSize,
                       mxfUInt64 length,
                       mxfUInt64 fixedSize)
{
  if (length >= fixedSize) {
    mxfUInt64 remaining = length - fixedSize;
    mxfUInt64 elementBytes = elementSize * elementCount;
    if (remaining < elementBytes) {
      mxfError("Element count too big for remaining bytes"
               " - %"MXFPRIu32" elements require %"MXFPRIu64" bytes,"
               " %"MXFPRIu64" bytes remaining"
               " (following key at offset 0x%"MXFPRIx64").\n",
               elementCount,
               elementBytes,
               remaining,
               keyPosition);
    } else if (remaining > elementBytes) {
      mxfError("Element count too small for remaining bytes"
               " - %"MXFPRIu32" elements require %"MXFPRIu64" bytes,"
              " %"MXFPRIu64" bytes remaining"
              " (following key at offset 0x%"MXFPRIx64").\n",
              elementCount,
              elementBytes,
              remaining,
              keyPosition);
    } // else correct
  } // else reported elsewhere
}

// In-memory representaton of a partition
typedef struct mxfPartitionTag {
  //
  mxfUInt16 _majorVersion;
  mxfUInt16 _minorVersion;
  mxfUInt32 _KAGSize;
  mxfUInt64 _thisPartition;
  mxfUInt64 _previousPartition;
  mxfUInt64 _footerPartition;
  mxfUInt64 _headerByteCount;
  mxfUInt64 _indexByteCount;
  mxfUInt32 _indexSID;
  mxfUInt64 _bodyOffset;
  mxfUInt32 _bodySID;
  mxfKey _operationalPattern;
  mxfUInt32 _elementCount;
  mxfUInt32 _elementSize;
  //
  mxfUInt64 _address; // Address of partition relative to header
  mxfUInt64 _length;
} mxfPartition;

typedef std::list<mxfPartition*> PartitionList;

mxfUInt64 headerPosition = 0;

void readPartition(PartitionList& partitions,
                   mxfUInt64 length,
                   mxfFile infile);

void readPartition(PartitionList& partitions,
                   mxfUInt64 length,
                   mxfFile infile)
{
  mxfPartition* p = new mxfPartition;

  p->_address = keyPosition - headerPosition;
  p->_length = length;

  readMxfUInt16(p->_majorVersion, infile);
  readMxfUInt16(p->_minorVersion, infile);
  readMxfUInt32(p->_KAGSize, infile);
  readMxfUInt64(p->_thisPartition, infile);
  readMxfUInt64(p->_previousPartition, infile);
  readMxfUInt64(p->_footerPartition, infile);
  readMxfUInt64(p->_headerByteCount, infile);
  readMxfUInt64(p->_indexByteCount, infile);
  readMxfUInt32(p->_indexSID, infile);
  readMxfUInt64(p->_bodyOffset, infile);
  readMxfUInt32(p->_bodySID, infile);
  readMxfLabel(p->_operationalPattern, infile);

  readMxfUInt32(p->_elementCount, infile);
  readMxfUInt32(p->_elementSize, infile);

  mxfUInt64 remaining = length - partitionFixedSize;
  skipBytes(remaining, infile);

  partitions.push_back(p);
}

void printPartitions(PartitionList& partitions);

void printPartitions(PartitionList& partitions)
{
  PartitionList::const_iterator it;
  for (it = partitions.begin(); it != partitions.end(); ++it) {
    mxfPartition* p = *it;
    fprintf(stderr, "%"MXFPRIx64"\n", p->_address);
  }
}

void checkField(mxfUInt64 expected,
                mxfUInt64 actual,
                mxfUInt64 keyAddress,
                char* label);

void checkField(mxfUInt64 expected,
                mxfUInt64 actual,
                mxfUInt64 keyAddress,
                char* label)
{
  if (actual != expected) {
    if (actual == 0) {
      mxfWarning("%s not defined"
                 " - expected 0x%"MXFPRIx64","
                 " (following key at offset 0x%"MXFPRIx64").\n",
                 label,
                 expected,
                 keyAddress);
    } else {
      mxfError("Incorrect value for %s"
               " - expected 0x%"MXFPRIx64", found 0x%"MXFPRIx64""
               " (following key at offset 0x%"MXFPRIx64").\n",
               label,
               expected,
               actual,
               keyAddress);
    }
  }
}

void checkOperationalPattern(mxfPartition* p);

void checkOperationalPattern(mxfPartition* p)
{
  if (isNullKey(p->_operationalPattern)) {
    mxfError("Null operational pattern"
             " (following key at offset 0x%"MXFPRIx64").\n",
             p->_address + headerPosition);
  }
}

void checkEssenceContainers(mxfPartition* p);

void checkEssenceContainers(mxfPartition* p)
{
  checkElementSize(sizeof(mxfKey), p->_elementSize, p->_elementCount);
  checkElementCount(p->_elementCount,
                    sizeof(mxfKey),
                    p->_length,
                    partitionFixedSize);
}

void checkPartition(mxfPartition* p, mxfUInt64 previous, mxfUInt64 footer);

void checkPartition(mxfPartition* p, mxfUInt64 previous, mxfUInt64 footer)
{
  // Major Version
  // Minor Version
  // KAGSize
  // ThisPartition
  checkField(p->_address,
             p->_thisPartition,
             p->_address + headerPosition,
             "ThisPartition");
  // PreviousPartition
  checkField(previous,
             p->_previousPartition,
             p->_address + headerPosition,
             "PreviousPartition");
  // FooterPartition
  if (footer != 0) {
    checkField(footer,
               p->_footerPartition,
               p->_address + headerPosition,
               "FooterPartition");
  }
  // HeaderByteCount
  // IndexByteCount
  // IndexSID
  // BodyOffset
  // BodySID
  // Operational Pattern
  checkOperationalPattern(p);
  // EssenceContainers
  checkEssenceContainers(p);
}

void checkPartitions(PartitionList& partitions, mxfUInt64 footer);

void checkPartitions(PartitionList& partitions, mxfUInt64 footer)
{
  mxfUInt64 previous = 0;
  PartitionList::const_iterator it;
  for (it = partitions.begin(); it != partitions.end(); ++it) {
    mxfPartition* p = *it;
    checkPartition(p, previous, footer);
    previous = p->_address;
  }
#if 0
  printPartitions(partitions);
#endif
}
void destroyPartitions(PartitionList& partitions);

void destroyPartitions(PartitionList& partitions)
{
  PartitionList::const_iterator it;
  for (it = partitions.begin(); it != partitions.end(); ++it) {
    mxfPartition* p = *it;
    delete p;
  }
}

// Note on partition keys -
//
//                              0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
// OpenHeader                  06 0e 2b 34 02 05 01 01 0d 01 02 01 01 02 01 00
// ClosedHeader                06 0e 2b 34 02 05 01 01 0d 01 02 01 01 02 02 00
// OpenCompleteHeader          06 0e 2b 34 02 05 01 01 0d 01 02 01 01 02 03 00
// ClosedCompleteHeader        06 0e 2b 34 02 05 01 01 0d 01 02 01 01 02 04 00
//
// OpenBodyPartition           06 0e 2b 34 02 05 01 01 0d 01 02 01 01 03 01 00
// ClosedBodyPartition         06 0e 2b 34 02 05 01 01 0d 01 02 01 01 03 02 00
// OpenCompleteBodyPartition   06 0e 2b 34 02 05 01 01 0d 01 02 01 01 03 03 00
// ClosedCompleteBodyPartition 06 0e 2b 34 02 05 01 01 0d 01 02 01 01 03 04 00
//
// Footer                      06 0e 2b 34 02 05 01 01 0d 01 02 01 01 04 02 00
// CompleteFooter              06 0e 2b 34 02 05 01 01 0d 01 02 01 01 04 04 00
//
//
// 1) The Footer may not be Open, so the following are illegal -
//
//    OpenFooter          = illegal
//    OpenCompleteFooter  = illegal
//
// 2) Since the Footer may not be Open, we omit Closed from these names -
//
//    Footer              = ClosedFooter
//    CompleteFooter      = ClosedCompleteFooter
//
// 3) The values are coded such that there aren't separate bits that indicate
//    Open/Closed and Complete/Incomplete
//

bool isPartition(mxfKey& key)
{
  bool result;
  if (memcmp(&OpenHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&OpenCompleteHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&ClosedHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&ClosedCompleteHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&OpenBodyPartition, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&OpenCompleteBodyPartition, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&ClosedBodyPartition, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&ClosedCompleteBodyPartition, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&Footer, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&CompleteFooter, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

bool isHeader(mxfKey& key)
{
  bool result;
  if (memcmp(&OpenHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&OpenCompleteHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&ClosedHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&ClosedCompleteHeader, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

bool isFooter(mxfKey& key)
{
  bool result;
  if (memcmp(&Footer, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else if (memcmp(&CompleteFooter, &key, sizeof(mxfKey)) == 0) {
    result = true;
  } else {
    result = false;
  }
  return result;
}

typedef struct mxfRIPEntryTag {
  mxfUInt32 _sid;
  mxfUInt64 _offset;
} mxfRIPEntry;

typedef std::list<mxfRIPEntry> RandomIndex;

void readRandomIndex(RandomIndex& rip, mxfLength length, mxfFile infile);


void readRandomIndex(RandomIndex& rip, mxfLength length, mxfFile infile)
{
  mxfUInt64 entryCount = length / (sizeof(mxfUInt32) + sizeof(mxfUInt64));
  for (mxfUInt32 i = 0; i < entryCount; i++) {
    mxfRIPEntry e;
    readMxfUInt32(e._sid, infile);
    readMxfUInt64(e._offset, infile);
    rip.push_back(e);
  }
  skipBytes(sizeof(mxfUInt32), infile);
}

void printRandomIndex(RandomIndex& rip);

void printRandomIndex(RandomIndex& rip)
{
  RandomIndex::const_iterator it;
  for (it = rip.begin(); it != rip.end(); ++it) {
    mxfRIPEntry e = *it;
    fprintf(stderr, "%"MXFPRIx32" : %"MXFPRIx64"\n", e._sid, e._offset);
  }
}

void checkRandomIndex(RandomIndex& rip, PartitionList& partitions);

void checkRandomIndex(RandomIndex& rip, PartitionList& partitions)
{
#if 0
  printRandomIndex(rip);
#endif
  // Check that we have the correct number of partitions.
  //
  size_t expectedPartitions = partitions.size();
  size_t actualPartitions = rip.size();
  if (actualPartitions != expectedPartitions) {
    mxfError("Invalid random index - incorrect partition count"
             " (partitions in file = %"MXFPRIu32","
             " partitions in random index = %"MXFPRIu32").\n",
             expectedPartitions,
             actualPartitions);
  }

  // Check that the partitions in the random index are in the correct order.
  //
  RandomIndex::const_iterator rit;
  mxfUInt64 previous;
  for (rit = rip.begin(); rit != rip.end(); ++rit) {
    if ((rit != rip.begin()) && (rit->_offset <= previous)) {
      mxfError("Invalid random index - partitions out of order"
               " (partition address = 0x%"MXFPRIx64","
               " address of previous partition = 0x%"MXFPRIx64").\n",
               rit->_offset,
               previous);
    }
    previous = rit->_offset;
 }

  // Check that each entry in the random index corresponds to a
  // partition in the file.
  //
  for (rit = rip.begin(); rit != rip.end(); ++rit) {
    bool found = false;
    mxfRIPEntry e = *rit;
    PartitionList::const_iterator pit;
    for (pit = partitions.begin(); pit != partitions.end(); ++pit) {
      mxfPartition* p = *pit;
      if (e._offset == p->_address) {
        found = true;
        break;
      }
    }
    if (!found) {
      mxfError("Invalid random index entry - no such partition"
               " (SID = %"MXFPRIu32", address = 0x%"MXFPRIx64").\n",
               e._sid,
               e._offset);
    }
  }

  // Check that each partition in the file has an entry in the random index.
  //
  PartitionList::const_iterator pit;
  for (pit = partitions.begin(); pit != partitions.end(); ++pit) {
    bool found = false;
    mxfPartition* p = *pit;
    RandomIndex::const_iterator rit;
    for (rit = rip.begin(); rit != rip.end(); ++rit) {
      mxfRIPEntry e = *rit;
      if (p->_address == e._offset) {
         found = true;
        break;
      }
    }
    if (!found) {
      mxfError("Invalid random index - missing partition",
               " (partition address = 0x%"MXFPRIx64").\n",
               p->_address);
    }
  }

}

mxfUInt64 checkFooterPosition(mxfUInt64 footer, mxfUInt64 keyPosition);

mxfUInt64 checkFooterPosition(mxfUInt64 footer, mxfUInt64 keyPosition)
{
  if (footer != 0) {
    mxfError("More than one footer"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
  }
  return keyPosition;
}

void checkPartitionLength(mxfUInt64& length)
{
  mxfUInt64 entrySize = sizeof(mxfKey);     // Essence container label
  if (length < partitionFixedSize) {
    // Valid length >= size of fixed portion
    mxfError("Invalid partition length - length too small"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
  } else {
    mxfUInt64 labelBytes = length - partitionFixedSize;
    mxfUInt64 labels = labelBytes / entrySize;
    if ((labels * entrySize ) != labelBytes) {
      mxfError("Invalid partition length -"
               " %"MXFPRIu64" != %"MXFPRIu64" + (N * %"MXFPRIu64")"
               " (following key at offset 0x%"MXFPRIx64").\n",
               length,
               partitionFixedSize,
               entrySize,
               keyPosition);
    }
  }
}

void printPartition(mxfKey& k, mxfLength& len, mxfFile infile);

void printPartition(mxfKey& k, mxfLength& len, mxfFile infile)
{
  checkPartitionLength(len);
  dumpMxfUInt16("Major Version", infile);
  dumpMxfUInt16("Minor Version", infile);
  dumpMxfUInt32("KAGSize", infile);
  dumpMxfUInt64("ThisPartition", infile);
  dumpMxfUInt64("PreviousPartition", infile);
  dumpMxfUInt64("FooterPartition", infile);
  dumpMxfUInt64("HeaderByteCount", infile);
  dumpMxfUInt64("IndexByteCount", infile);
  dumpMxfUInt32("IndexSID", infile);
  dumpMxfUInt64("BodyOffset", infile);
  dumpMxfUInt32("BodySID", infile);
  dumpMxfOperationalPattern("Operational Pattern", infile);
  fprintf(stdout, "%20s = ", "EssenceContainers");
  mxfUInt32 elementCount;
  readMxfUInt32(elementCount, infile);
  fprintf(stdout, "[ count = ");
  printDecField(stdout, elementCount);
  fprintf(stdout, " ]\n");
  mxfUInt32 elementSize;
  readMxfUInt32(elementSize, infile);
  checkElementSize(sizeof(mxfKey), elementSize, elementCount);
  checkElementCount(elementCount, sizeof(mxfKey), len, partitionFixedSize);
  for (mxfUInt32 i = 0; i < elementCount; i++) {
    mxfKey essence;
    readMxfLabel(essence, infile);
    fprintf(stdout, "  ");
    printDecField(stdout, i);
    fprintf(stdout, " : ");
    printMxfKey(essence, stdout);
    fprintf(stdout, "\n");
  }
}

void printHeaderPartition(mxfKey& k, mxfLength& len, mxfFile infile);

void printHeaderPartition(mxfKey& k, mxfLength& len, mxfFile infile)
{
  printPartition(k, len, infile);
}

void printBodyPartition(mxfKey& k, mxfLength& len, mxfFile infile);

void printBodyPartition(mxfKey& k, mxfLength& len, mxfFile infile)
{
  printPartition(k, len, infile);
}

void printFooterPartition(mxfKey& k, mxfLength& len, mxfFile infile);

void printFooterPartition(mxfKey& k, mxfLength& len, mxfFile infile)
{
  printPartition(k, len, infile);
}

void printIndexTable(mxfKey& k, mxfLength& len, mxfFile infile);

void printIndexTable(mxfKey& k, mxfLength& len, mxfFile infile)
{
  mxfLength remainder = len;
  while (remainder > 0) {
    mxfLocalKey identifier;
    readMxfLocalKey(identifier, infile);
    mxfUInt16 length;
    readMxfUInt16(length, infile);
    remainder = remainder - 4;

    if (identifier == 0x3c0a) {
      // InstanceUID
      dumpMxfLabel("InstanceUID", infile);
      remainder = remainder - 16;
    } else if (identifier == 0x3f05) {
      // Edit Unit Byte Count
      dumpMxfUInt32("Edit Unit Byte Count", infile);
      remainder = remainder - 4;
    } else if (identifier == 0x3f06) {
      // IndexSID
      dumpMxfUInt32("IndexSID", infile);
      remainder = remainder - 4;
    } else if (identifier == 0x3f07) {
      // BodySID
      dumpMxfUInt32("BodySID", infile);
      remainder = remainder - 4;
    } else if (identifier == 0x3f08) {
      // Slice Count
      dumpMxfUInt08("SliceCount", infile);
      remainder = remainder - 1;
    } else if (identifier == 0x3f0a) {
      // Entry array
      mxfUInt32 entryCount;
      readMxfUInt32(entryCount, infile);
      mxfUInt32 entrySize;
      readMxfUInt32(entrySize, infile);
      mxfUInt32 sliceCount = (entrySize - 11) / 4;
      remainder = remainder - 8;

      fprintf(stdout, "     IndexEntryArray = ");
      fprintf(stdout, "[ Number of entries = ");
      printDecField(stdout, entryCount);
      fprintf(stdout, "\n");
      fprintf(stdout, "                       ");
      fprintf(stdout, "  Entry size        = ");
      printDecField(stdout, entrySize);
      fprintf(stdout, " ]\n");

      if (entryCount > 0) {
        fprintf(stdout, "         ");
        fprintf(stdout, "        Temporal   Anchor  Flags        Stream\n");
        fprintf(stdout, "         ");
        fprintf(stdout, "          Offset   Offset               Offset\n");
      }

      mxfUInt32 count = 0;
      for (mxfUInt32 i = 0; i < entryCount; i++) {
        mxfUInt08 temporalOffset; // signed
        readMxfUInt08(temporalOffset, infile);
        mxfUInt08 anchorOffset; // signed
        readMxfUInt08(anchorOffset, infile);
        mxfUInt08 flags;
        readMxfUInt08(flags, infile);
        mxfUInt64 streamOffset;
        readMxfUInt64(streamOffset, infile);

        if (!cFlag || (i < maxIndex)) {
          fprintf(stdout, "    ");
          printDecField(stdout, i);
          fprintf(stdout, " :");
          fprintf(stdout, "    ");
          printDecField(stdout, temporalOffset);
          fprintf(stdout, "    ");
          printDecField(stdout, anchorOffset);
          fprintf(stdout, "     ");
          printHexFieldPad(stdout, flags);
          fprintf(stdout, "    ");
          printDecField(stdout, streamOffset);
          fprintf(stdout, "\n");

          for (mxfUInt32 s = 0; s < sliceCount; s++) {
            mxfUInt32 sliceOffset;
            readMxfUInt32(sliceOffset, infile);
            // Not yet printed
          }
          count = count + 1;
        }
        remainder = remainder - (11 + (4 * sliceCount));
      }
      if (cFlag && (count < entryCount)) {
        fprintf(stdout, "[ Index table truncated from ");
        printDecField(stdout, entryCount);
        fprintf(stdout, " entries to ");
        printDecField(stdout, maxIndex);
        fprintf(stdout, " entries ]\n");
      }
    } else if (identifier == 0x3f0b) {
      // Index Edit Rate
      dumpMxfRational("Index Edit Rate", infile);
      remainder = remainder - 8;
    } else if (identifier == 0x3f0c) {
      // Index Start Position
      dumpMxfUInt64("Index Start Position", infile);
      remainder = remainder - 8;
    } else if (identifier == 0x3f0d) {
      // Index Duration
      dumpMxfUInt64("Index Duration", infile);
      remainder = remainder - 8;
    } else {
      checkLocalKey(identifier);
      printLocalKL(identifier, length, k);
      printLocalV(length, remainder, infile);
    }
  }
}

void printV10IndexTable(mxfKey& k, mxfLength& len, mxfFile infile);

void printV10IndexTable(mxfKey& k, mxfLength& len, mxfFile infile)
{
  mxfLength remainder = len;
  while (remainder > 0) {
    mxfLocalKey identifier;
    readMxfLocalKey(identifier, infile);
    mxfUInt16 length;
    readMxfUInt16(length, infile);
    remainder = remainder - 4;

    if (identifier == 0x3c0a) {
      // InstanceUID
      dumpMxfLabel("InstanceUID", infile);
      remainder = remainder - 16;
    } else if (identifier == 0x3f05) {
      // Edit Unit Byte Count
      dumpMxfUInt32("Edit Unit Byte Count", infile);
      remainder = remainder - 4;
    } else if (identifier == 0x3f06) {
      // IndexSID
      dumpMxfUInt32("IndexSID", infile);
      remainder = remainder - 4;
    } else if (identifier == 0x3f07) {
      // BodySID
      dumpMxfUInt32("BodySID", infile);
      remainder = remainder - 4;
    } else if (identifier == 0x3f08) {
      // Slice Count
      dumpMxfUInt08("SliceCount", infile);
      remainder = remainder - 1;
    } else if (identifier == 0x3f0a) {
      // Entry array
      mxfUInt32 entryCount;
      readMxfUInt32(entryCount, infile);
      mxfUInt32 entrySize;
      readMxfUInt32(entrySize, infile);
      mxfUInt32 sliceCount = (entrySize - 11) / 4;
      remainder = remainder - 8;

      fprintf(stdout, "     IndexEntryArray = ");
      fprintf(stdout, "[ Number of entries = ");
      printDecField(stdout, entryCount);
      fprintf(stdout, "\n");
      fprintf(stdout, "                       ");
      fprintf(stdout, "  Entry size        = ");
      printDecField(stdout, entrySize);
      fprintf(stdout, " ]\n");

      if (entryCount > 0) {
        fprintf(stdout, "         ");
        fprintf(stdout, "        Temporal   Anchor  Flags        Stream\n");
        fprintf(stdout, "         ");
        fprintf(stdout, "          Offset   Offset               Offset\n");
      }

      mxfUInt32 count = 0;
      for (mxfUInt32 i = 0; i < entryCount; i++) {
        mxfUInt08 temporalOffset; // signed
        readMxfUInt08(temporalOffset, infile);
        mxfUInt08 anchorOffset; // signed
        readMxfUInt08(anchorOffset, infile);
        mxfUInt08 flags;
        readMxfUInt08(flags, infile);
        mxfUInt64 streamOffset;
        readMxfUInt64(streamOffset, infile);

        if (!cFlag || (i < maxIndex)) {
          fprintf(stdout, "    ");
          printDecField(stdout, i);
          fprintf(stdout, " :");
          fprintf(stdout, "    ");
          printDecField(stdout, temporalOffset);
          fprintf(stdout, "    ");
          printDecField(stdout, anchorOffset);
          fprintf(stdout, "     ");
          printHexFieldPad(stdout, flags);
          fprintf(stdout, "    ");
          printDecField(stdout, streamOffset);
          fprintf(stdout, "\n");

          for (mxfUInt32 s = 0; s < sliceCount; s++) {
            mxfUInt32 sliceOffset;
            readMxfUInt32(sliceOffset, infile);
            // Not yet printed
          }
          count = count + 1;
        }
        remainder = remainder - (11 + (4 * sliceCount));
      }
      if (cFlag && (count < entryCount)) {
        fprintf(stdout, "[ Index table truncated from ");
        printDecField(stdout, entryCount);
        fprintf(stdout, " entries to ");
        printDecField(stdout, maxIndex);
        fprintf(stdout, " entries ]\n");
      }
    } else if (identifier == 0x3f0b) {
      // Index Edit Rate
      dumpMxfRational("Index Edit Rate", infile);
      remainder = remainder - 8;
    } else if (identifier == 0x3f0c) {
      // Index Start Position
      dumpMxfUInt64("Index Start Position", infile);
      remainder = remainder - 8;
    } else if (identifier == 0x3f0d) {
      // Index Duration
      dumpMxfUInt64("Index Duration", infile);
      remainder = remainder - 8;
    } else {
      checkLocalKey(identifier);
      printLocalKL(identifier, length, k);
      printLocalV(length, remainder, infile);
    }
  }
}

// Size of the fixed portion of a primer
mxfUInt64 primerFixedSize =
  sizeof(mxfUInt32) + // Element count
  sizeof(mxfUInt32);  // Element size

void checkPrimerLength(mxfUInt64& length);

void checkPrimerLength(mxfUInt64& length)
{
  // Size of a primer entry
  mxfUInt64 entrySize = sizeof(mxfLocalKey) + // Local key
                        sizeof(mxfKey);       // Global key

  if (length < primerFixedSize) {
    // Valid length >= size of fixed portion
    mxfError("Invalid primer length - length is too small"
             " (following key at offset 0x%"MXFPRIx64").\n",
             keyPosition);
  } else {
    // Valid length == 8 + (N * 18)
    mxfUInt64 entryBytes = length - primerFixedSize;
    mxfUInt64 entries = entryBytes / entrySize;
    if ((entries * entrySize) != entryBytes) {
      mxfError("Invalid primer length -"
               " %"MXFPRIu64" != %"MXFPRIu64" + (N * %"MXFPRIu64")"
               " (following key at offset 0x%"MXFPRIx64").\n",
               length,
               primerFixedSize,
               entrySize,
               keyPosition);
    }
  }
}

void printPrimer(mxfKey& k, mxfLength& len, mxfFile infile);

void printPrimer(mxfKey& k, mxfLength& len, mxfFile infile)
{
  checkPrimerLength(len);
  mxfUInt32 elementCount;
  readMxfUInt32(elementCount, infile);
  mxfUInt32 elementSize;
  readMxfUInt32(elementSize, infile);
  checkElementSize(sizeof(mxfUInt16) + sizeof(mxfKey),
                   elementSize,
                   elementCount);
  checkElementCount(elementCount,
                    sizeof(mxfUInt16) + sizeof(mxfKey),
                    len,
                    primerFixedSize);

  fprintf(stdout, "  [ Number of entries = ");
  printDecField(stdout, elementCount);
  fprintf(stdout, ", Entry size        = ");
  printDecField(stdout, elementSize);
  fprintf(stdout, " ]\n");

  if (elementCount > 0) {
    fprintf(stdout, "  Local Tag      UID\n");
  }

  for (mxfUInt32 j = 0; j < elementCount; j++) {
    mxfLocalKey identifier;
    readMxfLocalKey(identifier, infile);
    mxfKey longIdentifier;
    readMxfLabel(longIdentifier, infile);
    fprintf(stdout, "  ");
    printMxfLocalKey(identifier, stdout);
    fprintf(stdout, "     :    ");
    printMxfKey(longIdentifier, stdout);
    fprintf(stdout, "\n");
  }
}

void printSystemMetadata(mxfKey& k, mxfLength& len, mxfFile infile);

void printSystemMetadata(mxfKey& k, mxfLength& len, mxfFile infile)
{
  printV(len, false, 0, infile);
}

void printObjectDirectory(mxfKey& k, mxfLength& len, mxfFile infile);

void printObjectDirectory(mxfKey& k, mxfLength& len, mxfFile infile)
{
  mxfUInt64 entryCount;
  readMxfUInt64(entryCount, infile);
  mxfUInt08 entrySize;
  readMxfUInt08(entrySize, infile);

  fprintf(stdout, "  [ Number of entries = ");
  printDecField(stdout, entryCount);
  fprintf(stdout, ", Entry size        = ");
  printDecField(stdout, entrySize);
  fprintf(stdout, " ]\n");

  if (entryCount > 0) {
    fprintf(stdout, "  Object");
    fprintf(stdout, "                                           Offset");
    fprintf(stdout, "            Flags\n");
  }

  for (mxfUInt64 i = 0; i < entryCount; i++) {
    mxfKey instance;
    readMxfLabel(instance, infile);
    mxfUInt64 offset;
    readMxfUInt64(offset, infile);
    mxfUInt08 flags;
    readMxfUInt08(flags, infile);

    fprintf(stdout, "  ");
    printMxfKey(instance, stdout);
    fprintf(stdout, "  ");
    printHexFieldPad(stdout, offset);
    fprintf(stdout, "  ");
    printHexFieldPad(stdout, flags);
    fprintf(stdout, "\n");
  }
}

void printRandomIndex(mxfKey& k, mxfLength& len, mxfFile infile);

void printRandomIndex(mxfKey& k, mxfLength& len, mxfFile infile)
{
  mxfUInt64 entryCount = len / (sizeof(mxfUInt32) + sizeof(mxfUInt64));
  fprintf(stdout, "  [ Number of entries = ");
  printDecField(stdout, entryCount );
  fprintf(stdout, " ]\n");
  fprintf(stdout, "       Partition           SID        Offset\n");
  for (mxfUInt32 i = 0; i < entryCount; i++) {
    mxfUInt32 sid;
    mxfUInt64 offset;
    readMxfUInt32(sid, infile);
    readMxfUInt64(offset, infile);
    fprintf(stdout, "    ");
    printDecField(stdout, i);
    fprintf(stdout, " :");
    fprintf(stdout, "    ");
    printDecField(stdout, sid);
    fprintf(stdout, "    ");
    if (base == 10) {
      printDecField(stdout, offset);
    } else {
      printHexFieldPad(stdout, offset);
    }
    fprintf(stdout, "\n");
  }
  mxfUInt32 length;
  readMxfUInt32(length, infile);
  fprintf(stdout, "  [ Overall length = ");
  printDecField(stdout, length);
  fprintf(stdout, " ]\n");
}

void printV10RandomIndex(mxfKey& k, mxfLength& len, mxfFile infile);

void printV10RandomIndex(mxfKey& k, mxfLength& len, mxfFile infile)
{
  printRandomIndex(k, len, infile);
}

void klvDumpFile(mxfFile infile)
{
  mxfUInt64 fileSize = size(infile);
  mxfKey k;
  while (readOuterMxfKey(k, infile)) {
    checkKey(k);
    mxfLength len;
    readMxfLength(len, infile);
    printKL(k, len);
    len = checkLength(len, fileSize, position(infile));
    printV(len, limitBytes, limit, infile);
  }
}

void setDumpFile(mxfFile infile)
{
  mxfUInt64 fileSize = size(infile);
  mxfKey k;
  while (readOuterMxfKey(k, infile)) {
    checkKey(k);
    mxfLength len;
    readMxfLength(len, infile);
    printKL(k, len);
    len = checkLength(len, fileSize, position(infile));

    if (isLocalSet(k)) {
      printLocalSet(k, len, infile);
    } else if (isFill(k)) {
      printFill(k, len, infile);
    } else if (isEssenceElement(k)) {
      printEssenceElement(k, len, limitBytes, limit, infile);
    } else {
      printV(len, false, 0, infile);
    }
  }
}

bool dumpDark = false;
bool dumpRunIn = false;

void mxfDumpKLV(mxfKey& k, mxfLength& len, mxfFile infile);

void mxfDumpKLV(mxfKey& k, mxfLength& len, mxfFile infile)
{
  if (memcmp(&OpenHeader, &k, sizeof(mxfKey)) == 0) {
    printHeaderPartition(k, len, infile);
  } else if (memcmp(&OpenCompleteHeader, &k, sizeof(mxfKey)) == 0) {
    printHeaderPartition(k, len, infile);
  } else if (memcmp(&ClosedHeader, &k, sizeof(mxfKey)) == 0) {
    printHeaderPartition(k, len, infile);
  } else if (memcmp(&ClosedCompleteHeader, &k, sizeof(mxfKey)) == 0) {
    printHeaderPartition(k, len, infile);
  } else if (memcmp(&OpenBodyPartition, &k, sizeof(mxfKey)) == 0) {
    printBodyPartition(k, len, infile);
  } else if (memcmp(&OpenCompleteBodyPartition, &k, sizeof(mxfKey)) == 0) {
    printBodyPartition(k, len, infile);
  } else if (memcmp(&ClosedBodyPartition, &k, sizeof(mxfKey)) == 0) {
    printBodyPartition(k, len, infile);
  } else if (memcmp(&ClosedCompleteBodyPartition, &k, sizeof(mxfKey)) == 0) {
    printBodyPartition(k, len, infile);
  } else if (memcmp(&Footer, &k, sizeof(mxfKey)) == 0) {
    printFooterPartition(k, len, infile);
  } else if (memcmp(&CompleteFooter, &k, sizeof(mxfKey)) == 0) {
    printFooterPartition(k, len, infile);
  } else if (memcmp(&Primer, &k, sizeof(mxfKey)) == 0) {
    printPrimer(k, len, infile);
    // The following are not yet dealt with explicitly, they are either
    // handled as their AAF equivalents or as generic local sets.
    //
    // Preface
    // Identification
    // ContentStorage
    // EssenceContainerData
    // MaterialPackage
    // SourcePackage
    // Track
    // Sequence
    // SourceClip
    // Timecode12MComponent
    // TimecodeStream12MComponent
    // DMSegment
    // DMSourceClip
    // FileDescriptor
    // GenPictureEssenceDesc
    // CDCIEssenceDescriptor
    // RGBAEssenceDescriptor
    // GenSoundEssenceDesc
    // GenDataEssenceDesc
    // MultipleDescriptor
    // NetworkLocator
    // TextLocator
    //
  } else if (memcmp(&SystemMetadata, &k, sizeof(mxfKey)) == 0) {
    printSystemMetadata(k, len, infile);
  } else if (memcmp(&ObjectDirectory, &k, sizeof(mxfKey)) == 0) {
    printObjectDirectory(k, len, infile);
  } else if (memcmp(&V10IndexTableSegment, &k, sizeof(mxfKey)) == 0) {
    printV10IndexTable(k, len, infile);
  } else if (memcmp(&IndexTableSegment, &k, sizeof(mxfKey)) == 0) {
    printIndexTable(k, len, infile);
  } else if (memcmp(&V10RandomIndexMetadata, &k, sizeof(mxfKey)) == 0) {
    printV10RandomIndex(k, len, infile);
  } else if (memcmp(&RandomIndexMetadata, &k, sizeof(mxfKey)) == 0) {
    printRandomIndex(k, len, infile);
  } else if (isFill(k)) {
    printFill(k, len, infile);
  } else if (isEssenceElement(k)) {
    printEssenceElement(k, len, limitBytes, limit, infile);
  } else {
    if (!isDark(k, mode) || dumpDark) {
      if (isLocalSet(k)) {
        printLocalSet(k, len, infile);
      } else if (darkKeysAsSets) {
        printLocalSet(k, len, infile);
      } else {
        printV(len, false, 0, infile);
      }
    } else {
      darkItems = darkItems + 1;
      skipV(len, infile);
    }
  }
}

void mxfDumpFile(mxfFile infile)
{
  mxfUInt64 fileSize = size(infile);
  mxfKey k;
  while (readOuterMxfKey(k, infile)) {
    checkKey(k);
    mxfLength len;
    readMxfLength(len, infile);
    if (isEssenceElement(k) || !isDark(k, mode) || dumpDark) {
      printKL(k, len);
    }
    len = checkLength(len, fileSize, position(infile));
    mxfDumpKLV(k, len, infile);
  }
}

void aafDumpKLV(mxfKey& k, mxfLength& len, mxfFile infile);

void aafDumpKLV(mxfKey& k, mxfLength& len, mxfFile infile)
{
  if (isAAFKey(k)) {
    printLocalSet(k, len, infile);
  } else {
    mxfDumpKLV(k, len, infile);
  }
}

void aafDumpFile(mxfFile infile)
{
  mxfUInt64 fileSize = size(infile);
  mxfKey k;
  while (readOuterMxfKey(k, infile)) {
    checkKey(k);
    mxfLength len;
    readMxfLength(len, infile);
    if (isEssenceElement(k) || !isDark(k, mode) || dumpDark) {
      printKL(k, len);
    }
    len = checkLength(len, fileSize, position(infile));
    aafDumpKLV(k, len, infile);
  }
}

void klvValidate(mxfFile infile);

void klvValidate(mxfFile infile)
{
  mxfUInt64 fileSize = size(infile);
  mxfKey k;
  while (readOuterMxfKey(k, infile)) {
    checkKey(k);
    mxfLength len;
    readMxfLength(len, infile);
    len = checkLength(len, fileSize, position(infile));
    skipV(len, infile);
  }

  fprintf(stderr,
          "%s : KLV validation       - ",
          programName());
  if (errors == 0) {
    fprintf(stderr, "passed.\n");
  } else {
    fprintf(stderr, "failed.\n");
  }
}

void setValidate(mxfFile infile);

void setValidate(mxfFile /* infile */)
{
  if (verbose) {
    fprintf(stderr,
            "%s : local set validation - not yet implemented.\n",
            programName());
  }
}

void mxfValidate(mxfFile infile);

void mxfValidate(mxfFile infile)
{
  PartitionList p;
  RandomIndex rip;
  mxfUInt64 footer = 0;
  mxfUInt64 fileSize = size(infile);
  mxfKey k;
  while (readOuterMxfKey(k, infile)) {
    checkKey(k);
    mxfLength len;
    readMxfLength(len, infile);
    mxfLength length = len;
    len = checkLength(len, fileSize, position(infile));
    if (memcmp(&Primer, &k, sizeof(mxfKey)) == 0) {
      checkPrimerLength(length);
      skipV(len, infile);
    } else if (isPartition(k)) {
      checkPartitionLength(length);
      if (isFooter(k)) {
        footer = checkFooterPosition(footer, keyPosition);
      }
      readPartition(p, len, infile);
    } else if (memcmp(&RandomIndexMetadata, &k, sizeof(mxfKey)) == 0) {
      readRandomIndex(rip, len, infile);
    } else {
      skipV(len, infile);
    }
  }

  if (footer == 0) {
    mxfError("No footer found.\n");
  }
  checkPartitions(p, footer);
  if (!rip.empty()) {
    checkRandomIndex(rip, p);
  } else {
    mxfWarning("No random index found.\n");
  }
  destroyPartitions(p);

  fprintf(stderr,
          "%s : MXF validation       - ",
          programName());
  if (errors == 0) {
    fprintf(stderr, "passed.\n");
  } else {
    fprintf(stderr, "failed.\n");
  }
}

void aafValidate(mxfFile infile);

void aafValidate(mxfFile /* infile */)
{
  if (verbose) {
    fprintf(stderr,
            "%s : AAF validation       - not yet implemented.\n",
            programName());
  }
}

void mxfValidateFile(Mode mode, mxfFile infile)
{
  switch (mode) {
  case aafValidateMode:
    aafValidate(infile);
    setPosition(infile, headerPosition);
    /* fall through */
  case mxfValidateMode:
    mxfValidate(infile);
    setPosition(infile, headerPosition);
    /* fall through */
  case setValidateMode:
    setValidate(infile);
    setPosition(infile, headerPosition);
    /* fall through */
  case klvValidateMode:
    klvValidate(infile);
    break;
  default:
    /* Invalid mode ? */
    break;
  }
}

void setMode(Mode m);

void setMode(Mode m)
{
  if (mode != unspecifiedMode) {
    error("Specify only one of "
          "--klv-dump, --set-dump, --mxf-dump, --aaf-dump, "
          "--aaf-validate, --mxf-validate, --set-validate, --klv-validate.\n");
    printUsage();
    exit(EXIT_FAILURE);
  }
  mode = m;
}

bool isDumpMode(Mode mode);

bool isDumpMode(Mode mode)
{
  bool result = false;
  if ((mode == klvMode) ||
      (mode == localSetMode) ||
      (mode == mxfMode) ||
      (mode == aafMode)) {
    result = true;
  }
  return result;
}

void checkDumpMode(const char* option);

void checkDumpMode(const char* option)
{
  if (!isDumpMode(mode)) {
    error("%s not valid with "
          "--aaf-validate, --mxf-validate, --set-validate, --klv-validate.\n",
          option);
    printUsage();
    exit(EXIT_FAILURE);
  }
}

bool getInteger(int& i, char* s);

bool getInteger(int& i, char* s)
{
  bool result;
  char* expectedEnd = &s[strlen(s)];
  char* end;
  int b = strtoul(s, &end, 10);
  if (end != expectedEnd) {
    result = false;
  } else {
    i = b;
    result = true;
  }
  return result;
}

int getIntegerOption(int currentArgument,
                     int argumentCount,
                     char* argumentVector[],
                     const char* label);

int getIntegerOption(int currentArgument,
                     int argumentCount,
                     char* argumentVector[],
                     const char* label)
{
  char* option = argumentVector[currentArgument];
  int result = 0;
  int optArg = currentArgument + 1;
  if ((optArg < argumentCount) && (*argumentVector[optArg] != '-' )) {
    char* value = argumentVector[optArg];
    if (!getInteger(result, value)) {
      error("\"%s\" is not a valid %s.\n", value, label);
      printUsage();
      exit(EXIT_FAILURE);
    }
  } else {
    error("\"%s\" must be followed by a %s.\n", option, label);
    printUsage();
    exit(EXIT_FAILURE);
  }
  return result;
}

void printSummary(void)
{
  if (verbose) {
    if ((errors != 0) || (warnings != 0)) {
      message("Encountered %"MXFPRIu32" errors and %"MXFPRIu32" warnings.\n",
               errors,
               warnings);
    }
  }
}

// Summary of options -
//
// -k --klv-dump
// -s --set-dump
// -m --mxf-dump
// -a --aaf-dump
// -v --verbose
// -f --show-fill
// -w --show-dark
//    --show-run-in
// -e --no-limit-bytes
// -l --limit-bytes
// -c --limit-entries
// -p --frames
// -i --limit-frames
// -j --key-addresses
//    --no-key-addresses
// -r --relative
// -b --absolute
// -t --decimal
// -x --hexadecimal
// -y --symbolic
// -n --no-symbolic
// -h --help
// -d --debug
// -u --show-dark-as-sets
//    --klv-validate
//    --set-validate
//    --mxf-validate
//    --aaf-validate

// Free letters - goqz

int main(int argumentCount, char* argumentVector[])
{
#if defined(MXF_USE_CONSOLE)
  argumentCount = ccommand(&argumentVector);
#endif
  atexit(printSummary);
  setProgramName(argumentVector[0]);
  checkSizes();
  initAAFKeyTable();
  int fileCount = 0;
  int fileArg = 0;
  char* p = 0;
  for (int i = 1; i < argumentCount; i++) {
    p = argumentVector[i];
    if ((strcmp(p, "--klv-dump") == 0) ||
        (strcmp(p, "-k") == 0)) {
      setMode(klvMode);
    } else if ((strcmp(p, "--set-dump") == 0) ||
               (strcmp(p, "-s") == 0)) {
      setMode(localSetMode);
    } else if ((strcmp(p, "--mxf-dump") == 0) ||
               (strcmp(p, "-m") == 0)) {
      setMode(mxfMode);
    } else if ((strcmp(p, "--aaf-dump") == 0) ||
               (strcmp(p, "-a") == 0)) {
      setMode(aafMode);
    } else if (strcmp(p, "--klv-validate") == 0) {
      setMode(klvValidateMode);
    } else if (strcmp(p, "--set-validate") == 0) {
      setMode(setValidateMode);
    } else if (strcmp(p, "--mxf-validate") == 0) {
      setMode(mxfValidateMode);
    } else if (strcmp(p, "--aaf-validate") == 0) {
      setMode(aafValidateMode);
    } else if ((strcmp(p, "--verbose") == 0) ||
               (strcmp(p, "-v") == 0)) {
      checkDumpMode(p);
      verbose = true;
    } else if ((strcmp(p, "--show-fill") == 0) ||
               (strcmp(p, "-f") == 0)) {
      checkDumpMode(p);
      dumpFill = true;
    } else if ((strcmp(p, "--show-dark") == 0) ||
               (strcmp(p, "-w") == 0)) {
      checkDumpMode(p);
      dumpDark = true;
    } else if (strcmp(p, "--show-run-in") == 0) {
      checkDumpMode(p);
      dumpRunIn = true;
    } else if ((strcmp(p, "--no-limit-bytes") == 0) ||
               (strcmp(p, "-e") == 0)) {
      checkDumpMode(p);
      lFlag = true;
      limitBytes = false;
    } else if ((strcmp(p, "--limit-bytes") == 0) ||
               (strcmp(p, "-l") == 0)) {
      checkDumpMode(p);
      lFlag = true;
      limitBytes = true;
      limit = getIntegerOption(i, argumentCount, argumentVector, "byte count");

      i = i + 1;
    } else if ((strcmp(p, "--limit-entries") == 0) ||
               (strcmp(p, "-c") == 0)) {
      checkDumpMode(p);
      maxIndex = getIntegerOption(i, argumentCount, argumentVector, "count");
      cFlag = true;
      i = i + 1;
    } else if (strcmp(p, "--no-limit-entries") == 0) {
      checkDumpMode(p);
      cFlag = false;
    } else if ((strcmp(p, "--frames") == 0) ||
               (strcmp(p, "-p") == 0)) {
      checkDumpMode(p);
      frames = true;
    } else if ((strcmp(p, "--limit-frames") == 0) ||
               (strcmp(p, "-i") == 0)) {
      checkDumpMode(p);
      maxFrames = getIntegerOption(i, argumentCount, argumentVector, "count");
      iFlag = true;
      i = i + 1;
    } else if ((strcmp(p, "--key-addresses") == 0) ||
               (strcmp(p, "-j") == 0)) {
      checkDumpMode(p);
      keyAddresses = true;
    } else if (strcmp(p, "--no-key-addresses") == 0) {
      checkDumpMode(p);
      keyAddresses = false;
    } else if ((strcmp(p, "--relative") == 0) ||
               (strcmp(p, "-r") == 0)) {
      checkDumpMode(p);
      relative = true;
    } else if ((strcmp(p, "--absolute") == 0) ||
               (strcmp(p, "-b") == 0)) {
      checkDumpMode(p);
      relative = false;
    } else if ((strcmp(p, "--decimal") == 0) ||
               (strcmp(p, "-t") == 0)) {
      checkDumpMode(p);
      base = 10;
    } else if ((strcmp(p, "--hexadecimal") == 0) ||
               (strcmp(p, "-x") == 0)) {
      checkDumpMode(p);
      base = 16;
    } else if ((strcmp(p, "--symbolic") == 0) ||
               (strcmp(p, "-y") == 0)) {
      checkDumpMode(p);
      symbolic = true;
    } else if ((strcmp(p, "--no-symbolic") == 0) ||
               (strcmp(p, "-n") == 0)) {
      checkDumpMode(p);
      symbolic = false;
    } else if ((strcmp(p, "-u") == 0) ||
               (strcmp(p, "--show-dark-as-sets") == 0)) {
      checkDumpMode(p);
      darkKeysAsSets = true;
      dumpDark = true;
    } else if ((strcmp(p, "--help") == 0) ||
               (strcmp(p, "-h") == 0)) {
      hFlag = true;
    } else if ((strcmp(p, "--debug") == 0) ||
               (strcmp(p, "-d") == 0)) {
      checkDumpMode(p);
      debug = true;
    } else if (*p == '-') {
      error("Invalid option \"%s\".\n", p);
      printUsage();
      exit(EXIT_FAILURE);
    } else {
      fileCount = fileCount + 1;
      fileArg = i;
    }
  }
  if (debug) {
    dumpKeyTable();
    dumpAAFKeyTable();
  }
  if (hFlag) {
    printHelp();
    exit(EXIT_SUCCESS);
  }
  if (mode == unspecifiedMode) {
    mode = mxfMode;
  }

  // Apply --limit-bytes default
  if (!lFlag) {
    if (mode == klvMode) {
      limitBytes = false;
    } else if (mode == localSetMode) {
      limitBytes = true;
      limit = 0;
    } else if (mode == mxfMode) {
      limitBytes = true;
      limit = 0;
    } else if (mode == aafMode) {
      limitBytes = true;
      limit = 0;
    }
  }

  if (mode == klvMode) {
    if (dumpFill) {
      error("--show-fill not valid with --klv-dump.\n");
      printUsage();
      exit(EXIT_FAILURE);
    }
  } else if (mode == localSetMode) {
    // No checks here yet
  } else if (mode == mxfMode) {
    // No checks here yet
  } else if (mode == aafMode) {
    // No checks here yet
  }
  int expectedFiles = 1;
  if (fileCount != expectedFiles) {
    error("Wrong number of arguments (%d).\n", fileCount);
    printUsage();
    exit(EXIT_FAILURE);
  }
  char* fileName = argumentVector[fileArg];
  if (verbose) {
    fprintf(stdout, "file = %s\n", fileName);
    if (limitBytes) {
      fprintf(stdout,
              "dumping only the first ");
      printDecField(stdout, limit);
      fprintf(stdout,
              " bytes of values.\n");
    }
  }

  mxfFile infile = openExistingRead(fileName);
  if (infile == NULL) {
    fatalError("File \"%s\" not found.\n", fileName);
  }

  if (!isMxfFile(infile, headerPosition)) {
    fatalError("File \"%s\" is not an MXF file.\n", fileName);
  }
  if (isDumpMode(mode)) {
    if (headerPosition != 0) {
      if (dumpRunIn) {
        printRunIn(headerPosition, limitBytes, limit, infile);
      } else {
        fprintf(stdout, "\n");
        fprintf(stdout,
                "[ Omitted %"MXFPRIu64" bytes of run-in ]\n",
                headerPosition);
      }
    }
  }
  setPosition(infile, headerPosition);

  if (mode == klvMode) {
    klvDumpFile(infile);
  } else if (mode == localSetMode) {
    setDumpFile(infile);
  } else if (mode == mxfMode) {
    mxfDumpFile(infile);
  } else if (mode == aafMode) {
    aafDumpFile(infile);
  } else if (mode == klvValidateMode) {
    mxfValidateFile(mode, infile);
  } else if (mode == setValidateMode) {
    mxfValidateFile(mode, infile);
  } else if (mode == mxfValidateMode) {
    mxfValidateFile(mode, infile);
  } else if (mode == aafValidateMode) {
    mxfValidateFile(mode, infile);
  }
  close(infile);

  int result = EXIT_SUCCESS;
  if ((errors != 0) || (warnings != 0)) {
    result = EXIT_FAILURE;
  }

  return result;
}
