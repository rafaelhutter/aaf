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
// The Initial Developer of the Original Code of this file and the
// Licensor of the AAF Association is Avid Technology.
// All rights reserved.
//
//=---------------------------------------------------------------------=

// @doc OMINTERNAL
#ifndef OMSSSSTOREDOBJECT_H
#define OMSSSSTOREDOBJECT_H

#include "OMSSSStructuredStorage.h"
#include "OMStoredObject.h"
#include "OMDataTypes.h"
#include "OMFile.h"

#include <stddef.h>

class OMStoredPropertySetIndex;
class OMProperty;
class OMPropertySet;
class OMPropertyTable;
class OMStoredVectorIndex;
class OMStoredSetIndex;

  // @class In-memory representation of an object persisted in a
  //        Microsoft Structured Storage (SSS) binary file.
  //   @cauthor Tim Bingham | tjb | Avid Technology, Inc.
class OMSSSStoredObject : public OMStoredObject {
public:
  // @access Static members.

    // @cmember Perform Microsoft Structured Storage specific initialization.
  static void initialize(void);

    // @cmember Perform Microsoft Structured Storage specific finalization.
  static void finalize(void);

    // @cmember Open the root <c OMSSSStoredObject> in the disk file
    //          <p fileName> for reading only.
    //   @devnote Soon to be obsolete.
  static OMSSSStoredObject* openRead(const wchar_t* fileName);

    // @cmember Open the root <c OMSSSStoredObject> in the disk file
    //          <p fileName> for modification.
    //   @devnote Soon to be obsolete.
  static OMSSSStoredObject* openModify(const wchar_t* fileName);

    // @cmember Create a new root <c OMSSSStoredObject> in the disk file
    //          <p fileName>. The byte order of the newly created root
    //          is given by <p byteOrder>.
    //   @devnote Soon to be obsolete.
  static OMSSSStoredObject* createModify(const wchar_t* fileName,
                                         const OMByteOrder byteOrder,
																				 const OMUniqueObjectIdentification& signature);

    // @cmember Open the root <c OMSSSStoredObject> in the raw storage
    //          <p rawStorage> for reading only.
  static OMSSSStoredObject* openRead(OMRawStorage* rawStorage);

    // @cmember Open the root <c OMSSSStoredObject> in the raw storage
    //          <p rawStorage> for modification.
  static OMSSSStoredObject* openModify(OMRawStorage* rawStorage);

    // @cmember Create a new root <c OMSSSStoredObject> in the raw storage
    //          <p rawStorage>. The byte order of the newly created root
    //          is given by <p byteOrder>.
  static OMSSSStoredObject* createModify(OMRawStorage* rawStorage,
                                         const OMByteOrder byteOrder,
																				 const OMUniqueObjectIdentification& signature);

  // @access Public members.

    // @cmember Destructor.
  virtual ~OMSSSStoredObject(void);

    // @cmember Create a new <c OMSSSStoredObject>, named <p name>,
    //          contained by this <c OMSSSStoredObject>.
    //   @devnote The name argument to this member function doesn't
    //            make sense for all derived instances of <c OMStoredObject>.
  virtual OMStoredObject* create(const wchar_t* name);

    // @cmember Open an exsiting <c OMSSSStoredObject>, named <p name>,
    //          contained by this <c OMSSSStoredObject>.
    //   @devnote The name argument to this member function doesn't
    //            make sense for all derived instances of <c OMStoredObject>.
  virtual OMStoredObject* open(const wchar_t* name);

    // @cmember Close this <c OMSSSStoredObject>.
  virtual void close(void);

    // @cmember The byte order of this <c OMSSSStoredObject>.
    //   @devnote This member function doesn't make sense for all
    //            derived instances of <c OMStoredObject>.
  virtual OMByteOrder byteOrder(void) const;

  virtual void save(OMFile& file);

  virtual void save(OMStorable& object);

  // Saving and restoring properties

    // @cmember Save the <c OMStoredObjectIdentification> <p id>
    //          in this <c OMSSSStoredObject>.
  virtual void save(const OMStoredObjectIdentification& id);

    // @cmember Save the <c OMPropertySet> <p properties> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMPropertySet& properties);

    // @cmember Save the <c OMSimpleProperty> <p property> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMSimpleProperty& property);

    // @cmember Save the <c OMStrongReference> <p singleton> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMStrongReference& singleton);

    // @cmember Save the <c OMStrongReferenceVector> <p vector> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMStrongReferenceVector& vector);

    // @cmember Save the <c OMStrongReferenceSet> <p set> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMStrongReferenceSet& set);

    // @cmember Save the <c OMWeakReference> <p singleton> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMWeakReference& singleton);

    // @cmember Save the <c OMWeakReferenceVector> <p vector> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMWeakReferenceVector& vector);

    // @cmember Save the <c OMWeakReferenceSet> <p set> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMWeakReferenceSet& set);

    // @cmember Save the <c OMPropertyTable> <p table> in this
    //          <c OMSSSStoredObject>.
    //   @devnote Does this member function make sense for all
    //            derived instances of <c OMStoredObject> ?
  virtual void save(const OMPropertyTable* table);

    // @cmember Save the <c OMDataStream> <p stream> in this
    //          <c OMSSSStoredObject>.
  virtual void save(const OMDataStream& stream);

  virtual OMRootStorable* restore(OMFile& file);

  virtual OMStorable* restoreObject(const OMStrongObjectReference& reference);

   // @cmember Restore the <c OMStoredObjectIdentification>
    //          of this <c OMSSSStoredObject> into <p id>.
  virtual void restore(OMStoredObjectIdentification& id);

    // @cmember Restore the <c OMPropertySet> <p properties> into
    //          this <c OMSSSStoredObject>.
  virtual void restore(OMPropertySet& properties);

    // @cmember Restore the <c OMSimpleProperty> <p property> into this
    //          <c OMSSSStoredObject>.
    //   @devnote The externalSize argument to this member function doesn't
    //            make sense for all derived instances of <c OMStoredObject>.
  virtual void restore(OMSimpleProperty& property,
                       size_t externalSize);

    // @cmember Restore the <c OMStrongReference> <p singleton> into this
    //          <c OMSSSStoredObject>.
  virtual void restore(OMStrongReference& singleton,
                       size_t externalSize);

    // @cmember Restore the <c OMStrongReferenceVector> <p vector> into this
    //          <c OMSSSStoredObject>.
  virtual void restore(OMStrongReferenceVector& vector,
                       size_t externalSize);

    // @cmember Restore the <c OMStrongReferenceSet> <p set> into this
    //          <c OMSSSStoredObject>.
  virtual void restore(OMStrongReferenceSet& set,
                       size_t externalSize);

    // @cmember Restore the <c OMWeakReference> <p singleton> into this
    //          <c OMSSSStoredObject>.
  virtual void restore(OMWeakReference& singleton,
                       size_t externalSize);

    // @cmember Restore the <c OMWeakReferenceVector> <p vector> into this
    //          <c OMSSSStoredObject>.
  virtual void restore(OMWeakReferenceVector& vector,
                       size_t externalSize);

    // @cmember Restore the <c OMWeakReferenceSet> <p set> into this
    //          <c OMSSSStoredObject>.
  virtual void restore(OMWeakReferenceSet& set,
                       size_t externalSize);

    // @cmember Restore the <c OMPropertyTable> in this <c OMSSSStoredObject>.
    //   @devnote Does this member function make sense for all
    //            derived instances of <c OMStoredObject> ?
  virtual void restore(OMPropertyTable*& table);

    // @cmember Restore the <c OMDataStream> <p stream> into this
    //          <c OMSSSStoredObject>.
  virtual void restore(OMDataStream& stream,
                       size_t externalSize);

  // Stream manipulation

    // @cmember Open the <c OMStoredStream> representing the property
    //          <p stream> contained within this <c OMSSSStoredObject>.
  virtual OMStoredStream* openStoredStream(const OMDataStream& property);

    // @cmember Create an <c OMStoredStream> representing the property
    //          <p stream> contained within this <c OMSSSStoredObject>.
  virtual OMStoredStream* createStoredStream(const OMDataStream& property);

protected:
  // @access Protected members.

    // @cmember Constructor.
  OMSSSStoredObject(Storage* s);

    // @cmember Constructor.
  OMSSSStoredObject(RootStorage* s);

    // @cmember Check that the <c OMPropertySet> <p propertySet> is
    //          consistent with the <c OMStoredPropertySetIndex>
    //          propertySetIndex.
  void validate(const OMPropertySet* propertySet,
                const OMStoredPropertySetIndex* propertySetIndex) const;

    // @cmember Save the <c OMStoredVectorIndex> <p vector> in this
    //          <c OMSSSStoredObject>, the vector is named <p vectorName>.
  void save(const OMStoredVectorIndex* vector, const wchar_t* vectorName);

    // @cmember Save the <c OMStoredSetIndex> <p set> in this
    //          <c OMSSSStoredObject>, the set is named <p setName>.
  void save(const OMStoredSetIndex* set, const wchar_t* setName);

    // @cmember Save a single weak reference.
  void save(OMPropertyId propertyId,
            OMStoredForm storedForm,
            const OMUniqueObjectIdentification& id,
            OMPropertyTag tag,
            OMPropertyId keyPropertyId);

    // @cmember Save a collection (vector/set) of weak references.
  void save(const wchar_t* collectionName,
            const OMUniqueObjectIdentification* index,
            size_t count,
            OMPropertyTag tag,
            OMPropertyId keyPropertyId);

  void saveStream(OMPropertyId pid,
                  OMStoredForm storedForm,
                  const wchar_t* name,
                  OMByteOrder byteOrder);

    // @cmember Restore the vector named <p vectorName> into this
    //          <c OMSSSStoredObject>.
  void restore(OMStoredVectorIndex*& vector, const wchar_t* vectorName);

    // @cmember Restore the set named <p setName> into this
    //          <c OMSSSStoredObject>.
  void restore(OMStoredSetIndex*& set, const wchar_t* setName);

    // @cmember Restore a single weak reference.
  void restore(OMPropertyId propertyId,
               OMStoredForm storedForm,
               OMUniqueObjectIdentification& id,
               OMPropertyTag& tag,
               OMPropertyId& keyPropertyId);

    // @cmember Restore a collection (vector/set) of weak references.
  void restore(const wchar_t* collectionName,
               OMUniqueObjectIdentification*& index,
               size_t &count,
               OMPropertyTag& tag,
               OMPropertyId& keyPropertyId);

  void restoreStream(OMPropertyId pid,
                     OMStoredForm storedForm,
                     size_t size,
                     wchar_t** name,
                     OMByteOrder* byteOrder);

    // @cmember Write a property value to this <c OMSSSStoredObject>. The
    //          property value to be written occupies <p size> bytes at
    //          the address <p start>. The property id is <p propertyId>.
    //          The property type is <p type>.
  void write(OMPropertyId propertyId,
             OMStoredForm storedForm,
             void* start,
             size_t size);

    // @cmember Read a property value from this <c OMSSSStoredObject>.
    //          The property value is read into a buffer which occupies
    //          <p size> bytes at the address <p start>. The property id
    //          is <p propertyId>. The property type is <p type>.
  void read(OMPropertyId propertyId,
            OMStoredForm storedForm,
            void* start,
            size_t size);

    // @cmember Open a stream called <p streamName> contained within
    //          this <c OMSSSStoredObject>.
  Stream* openStream(const wchar_t* streamName);

    // @cmember Create a stream called <p streamName> contained within
    //          this <c OMSSSStoredObject>.
  Stream* createStream(const wchar_t* streamName);

    // @cmember Read <p size> bytes from <p stream> into the buffer at
    //          address <p data>.
  void readFromStream(Stream* stream, void* data, size_t size);

    // @cmember Attempt to read <p bytes> bytes from <p stream> into
    //          the buffer at address <p data>. The actual number of
    //          bytes read is returned in <p bytesRead>.
  void readFromStream(Stream* stream,
                      OMByte* data,
                      const OMUInt32 bytes,
                      OMUInt32& bytesRead);

    // @cmember Write <p size> bytes from the buffer at address
    //          <p data> to <p stream>.
  void writeToStream(Stream* stream, void* data, size_t size);

    // @cmember Attempt to write <p bytes> bytes from the buffer at
    //          address <p data> to <p stream>. The actual number of
    //          bytes written is returned in <p bytesWritten>.
  void writeToStream(Stream* stream,
                     const OMByte* data,
                     const OMUInt32 bytes,
                     OMUInt32& bytesWritten);

    // @cmember Read an OMUInt8 from <p stream> into <p i>.
  void readUInt8FromStream(Stream* stream, OMUInt8& i);

    // @cmember Write an OMUInt8 from <p i> to <p stream>.
  void writeUInt8ToStream(Stream* stream, OMUInt8& i);

    // @cmember Read an OMUInt16 from <p stream> into <p i>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void readUInt16FromStream(Stream* stream, OMUInt16& i, bool reorderBytes);

    // @cmember Write an OMUInt16 from <p i> to <p stream>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void writeUInt16ToStream(Stream* stream, OMUInt16& i, bool reorderBytes);

    // @cmember Reorder the OMUInt16 <p i>.
  static void reorderUInt16(OMUInt16& i);

    // @cmember Read an OMUInt32 from <p stream> into <p i>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void readUInt32FromStream(Stream* stream, OMUInt32& i, bool reorderBytes);

    // @cmember Write an OMUInt32 from <p i> to <p stream>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void writeUInt32ToStream(Stream* stream, OMUInt32& i, bool reorderBytes);

    // @cmember Reorder the OMUInt32 <p i>.
  static void reorderUInt32(OMUInt32& i);

    // @cmember Read an OMUInt64 from <p stream> into <p i>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void readUInt64FromStream(Stream* stream, OMUInt64& i, bool reorderBytes);

    // @cmember Write an OMUInt64 from <p i> to <p stream>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void writeUInt64ToStream(Stream* stream, OMUInt64& i, bool reorderBytes);

    // @cmember Reorder the OMUInt64 <p i>.
  static void reorderUInt64(OMUInt64& i);

    // @cmember Read a UniqueObjectIdentification from <p stream>
    //          into <p id>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void readUniqueObjectIdentificationFromStream(
                                              Stream* stream,
                                              OMUniqueObjectIdentification& id,
                                              bool reorderBytes);

    // @cmember Write a UniqueObjectIdentification from <p id>
    //          to <p stream>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void writeUniqueObjectIdentificationToStream(
                                              Stream* stream,
                                              OMUniqueObjectIdentification& id,
                                              bool reorderBytes);

    // @cmember Read a UniqueMaterialIdentification from <p stream>
    //          into <p id>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void readUniqueMaterialIdentificationFromStream(
                                            Stream* stream,
                                            OMUniqueMaterialIdentification& id,
                                            bool reorderBytes);

    // @cmember Write a UniqueMaterialIdentification from <p id>
    //          to <p stream>.
    //          If <p reorderBytes> is true then the bytes are reordered.
  void writeUniqueMaterialIdentificationToStream(
                                            Stream* stream,
                                            OMUniqueMaterialIdentification& id,
                                            bool reorderBytes);

    // @cmember Reorder the UniqueObjectIdentification <p id>.
  static void reorderUniqueObjectIdentification(
                                             OMUniqueObjectIdentification& id);

    // @cmember Reorder the UniqueMaterialIdentification <p id>.
  static void reorderUniqueMaterialIdentification(
                                           OMUniqueMaterialIdentification& id);

    // @cmember Size of <p stream> in bytes.
  OMUInt64 streamSize(Stream* stream) const;

    // @cmember Set the size, in bytes, of <p stream>
  void streamSetSize(Stream* stream, const OMUInt64 newSize);

    // @cmember The current position for <f readFromStream()> and
    //          <f writeToStream()>, as an offset in bytes from the
    //          begining of the data stream.
  OMUInt64 streamPosition(Stream* stream) const;

    // @cmember Set the current position for <f readFromStream()> and
    //          <f writeToStream()>, as an offset in bytes from the
    //          begining of the data stream.
  void streamSetPosition(Stream* stream, const OMUInt64 offset);

    // @cmember Close <p stream>.
  void closeStream(Stream*& stream);

  void writeName(const wchar_t* name);

    // @cmember The persisted value of <p property> is its name.
    //          Write the property name and enter it into the property index.
  void saveName(const OMProperty& property, const wchar_t* name);

    // @cmember The persisted value of <p property> is its name.
    //          Read (and check) the property name.
  void restoreName(OMProperty& property, const wchar_t* name, size_t size);

  void writeName(OMPropertyId pid,
                 OMStoredForm storedForm,
                 const wchar_t* name);

  wchar_t* readName(OMPropertyId pid,
                    OMStoredForm storedForm,
                    size_t size);

  static void reorderString(OMCharacter* string,
                            size_t characterCount);

  static void internalizeString(OMCharacter* externalString,
                                wchar_t* internalString,
                                size_t characterCount);

  static void externalizeString(const wchar_t* internalString,
                                OMCharacter* externalString,
                                size_t characterCount);

  static void reorderUInt16Array(OMUInt16* array,
                                 size_t elementCount);

  static void internalizeUInt16Array(OMUInt16* externalArray,
                                     OMUInt16* internalArray,
                                     size_t elementCount);

  static void externalizeUInt16Array(const OMUInt16* internalArray,
                                     OMUInt16* externalArray,
                                     size_t elementCount);

private:
  // @access Private members.

  static OMSSSStoredObject* openFile(const wchar_t* fileName,
                                     const OMFile::OMAccessMode mode);
  static OMSSSStoredObject* createFile(const wchar_t* fileName,
																			 const OMUniqueObjectIdentification& signature);
  static OMSSSStoredObject* openFile(OMRawStorage* rawStorage,
                                     const OMFile::OMAccessMode mode);
  static OMSSSStoredObject* createFile(OMRawStorage* rawStorage,
																			 const OMUniqueObjectIdentification& signature);

  void create(const OMByteOrder byteOrder);
  void open(const OMFile::OMAccessMode mode);

  void save(OMStoredPropertySetIndex *index);
  OMStoredPropertySetIndex* restore(void);

    // @cmember The stream name for the index of a collection
    //          named <p collectionName>.
  wchar_t* collectionIndexStreamName(const wchar_t* collectionName);

  Stream* createStream(Storage* storage, const wchar_t* streamName);

  Stream* openStream(Storage* storage, const wchar_t* streamName);

  Stream* createBufferedStream(Storage* storage, const wchar_t* streamName);

  Stream* openBufferedStream(Storage* storage, const wchar_t* streamName);

  Storage* createStorage(Storage* storage, const wchar_t* storageName);
  Storage* openStorage(Storage* storage,
                        const wchar_t* storageName,
                        const OMFile::OMAccessMode mode);

  void closeStorage(Storage*& storage);
  void closeStorage(RootStorage*& root);

  void setClass(Storage* storage, const OMClassId& cid);
  void getClass(Storage* storage, OMClassId& cid);

  Storage* _storage;
	RootStorage* _root;

  OMStoredPropertySetIndex* _index;
  Stream* _properties;
  size_t _offset;

  bool _open;
  OMFile::OMAccessMode _mode;
  OMByteOrder _byteOrder;
  bool _reorderBytes;

#if defined(OM_DEBUG)
  static void incrementOpenStreamCount(void);
  static void decrementOpenStreamCount(void);
  static void incrementOpenStorageCount(void);
  static void decrementOpenStorageCount(void);

  static size_t _openStorages;
  static size_t _maxOpenStorages;
  static size_t _openStreams;
  static size_t _maxOpenStreams;
#endif
};

#endif
