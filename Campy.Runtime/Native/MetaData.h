// Copyright (c) 2012 DotNetAnywhere
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "Types.h"
#include "RVA.h"
#if defined(CUDA)
#include <crt/host_defines.h>
#endif

#define MAX_TABLES 48

struct tMetaDataStrings_ {
	// The start of the string heap
	unsigned char *pStart;
};
typedef struct tMetaDataStrings_ tMetaDataStrings;

struct tMetaDataBlobs_ {
	// The start of the blob heap
	unsigned char *pStart;
};
typedef struct tMetaDataBlobs_ tMetaDataBlobs;

struct tMetaDataUserStrings_ {
	// The start of the user string heap
	unsigned char *pStart;
};
typedef struct tMetaDataUserStrings_ tMetaDataUserStrings;

struct tMetaDataGUIDs_ {
	// The total number of GUIDs
	unsigned int numGUIDs;
	// Pointer to the first GUID
	unsigned char *pGUID1;
};
typedef struct tMetaDataGUIDs_ tMetaDataGUIDs;

typedef struct tTables_ tTables;
struct tTables_ {
	// The number of rows in each table
	unsigned int numRows[MAX_TABLES];
	// The table data itself. 64 pointers to table data
	// See MetaDataTables.h for each table structure
	void* data[MAX_TABLES];

	// Should each coded index lookup type use 16 or 32 bit indexes?
	unsigned char codedIndex32Bit[13];
};

typedef struct tMetaData_ tMetaData;
struct tMetaData_ {
	tMetaDataStrings strings;
	tMetaDataBlobs blobs;
	tMetaDataUserStrings userStrings;
	tMetaDataGUIDs GUIDs;
	tTables tables;
	unsigned char index32BitString, index32BitBlob, index32BitGUID;
	char * file_name;
};

#define TYPEATTRIBUTES_INTERFACE 0x20

#define METHODATTRIBUTES_STATIC 0x10
#define METHODATTRIBUTES_VIRTUAL 0x40
#define METHODATTRIBUTES_NEWSLOT 0x100
#define METHODATTRIBUTES_PINVOKEIMPL 0x2000

#define METHODIMPLATTRIBUTES_CODETYPE_MASK 0x3
#define METHODIMPLATTRIBUTES_CODETYPE_RUNTIME 0x3
#define METHODIMPLATTRIBUTES_INTERNALCALL 0x1000

#define FIELDATTRIBUTES_STATIC 0x10
#define FIELDATTRIBUTES_LITERAL 0x40 // compile-time constant
#define FIELDATTRIBUTES_HASFIELDRVA 0x100

#define SIG_METHODDEF_GENERIC 0x10
#define SIG_METHODDEF_HASTHIS 0x20

#define IMPLMAP_FLAGS_CHARSETMASK 0x0006
#define IMPLMAP_FLAGS_CHARSETNOTSPEC 0x0000
#define IMPLMAP_FLAGS_CHARSETANSI 0x0002
#define IMPLMAP_FLAGS_CHARSETUNICODE 0x0004
#define IMPLMAP_FLAGS_CHARSETAUTO 0x0006

#define TYPE_ISARRAY(pType) ((pType)->pArrayElementType != NULL)
#define TYPE_ISINTERFACE(pType) ((pType)->flags & TYPEATTRIBUTES_INTERFACE)
#define TYPE_ISGENERICINSTANCE(pType) ((pType)->pGenericDefinition != NULL)

#define METHOD_ISVIRTUAL(pMethod) ((pMethod)->flags & METHODATTRIBUTES_VIRTUAL)
#define METHOD_ISSTATIC(pMethod) ((pMethod)->flags & METHODATTRIBUTES_STATIC)
#define METHOD_ISNEWSLOT(pMethod) ((pMethod)->flags & METHODATTRIBUTES_NEWSLOT)

#define FIELD_HASFIELDRVA(pField) ((pField)->flags & FIELDATTRIBUTES_HASFIELDRVA)
#define FIELD_ISLITERAL(pField) ((pField)->flags & FIELDATTRIBUTES_LITERAL)
#define FIELD_ISSTATIC(pField) ((pField)->flags & FIELDATTRIBUTES_STATIC)

#define IMPLMAP_ISCHARSET_NOTSPEC(pImplMap) (((pImplMap)->mappingFlags & IMPLMAP_FLAGS_CHARSETMASK) == IMPLMAP_FLAGS_CHARSETNOTSPEC)
#define IMPLMAP_ISCHARSET_ANSI(pImplMap) (((pImplMap)->mappingFlags & IMPLMAP_FLAGS_CHARSETMASK) == IMPLMAP_FLAGS_CHARSETANSI)
#define IMPLMAP_ISCHARSET_UNICODE(pImplMap) (((pImplMap)->mappingFlags & IMPLMAP_FLAGS_CHARSETMASK) == IMPLMAP_FLAGS_CHARSETUNICODE)
#define IMPLMAP_ISCHARSET_AUTO(pImplMap) (((pImplMap)->mappingFlags & IMPLMAP_FLAGS_CHARSETMASK) == IMPLMAP_FLAGS_CHARSETAUTO)

#define TABLE_ID(index) ((index) >> 24)
#define TABLE_OFS(index) ((index) & 0x00ffffff)
#define MAKE_TABLE_INDEX(table, index) ((IDX_TABLE)(((table) << 24) | ((index) & 0x00ffffff)))

typedef struct tParameter_ tParameter;
typedef struct tInterfaceMap_ tInterfaceMap;

#include "MetaDataTables.h"

struct tParameter_ {
	// The type of the parameter
	tMD_TypeDef *pTypeDef;
	// The offset for this parameter into the paramater stack (in bytes)
	U32 offset;
	// The size of this value on the parameter stack (in bytes)
	U32 size;
};

struct tInterfaceMap_ {
	// The interface this is implementing
	tMD_TypeDef *pInterface;
	// The vTable for this interface implementation
	U32 *pVTableLookup;
	// The direct method table for this interface. This is only used for special auto-generated interfaces
	tMD_MethodDef **ppMethodVLookup;
};

// static functions
function_space_specifier void MetaData_Init();
function_space_specifier unsigned int MetaData_DecodeUnsigned32BitInteger(SIG *pSig);
function_space_specifier unsigned int MetaData_DecodeUnsigned8BitInteger(SIG *pSig);
function_space_specifier IDX_TABLE MetaData_DecodeSigEntryToken(SIG *pSig);
function_space_specifier unsigned int MetaData_DecodeHeapEntryLength(unsigned char **ppHeapEntry);

function_space_specifier void MetaData_GetHeapRoots(tHeapRoots *pHeapRoots, tMetaData *pMetaData);

// Meta-data filling extra information

function_space_specifier void MetaData_Fill_TypeDef(tMD_TypeDef *pTypeDef, tMD_TypeDef **ppClassTypeArgs, tMD_TypeDef **ppMethodTypeArgs);
function_space_specifier void MetaData_Fill_FieldDef(tMD_TypeDef *pParentType, tMD_FieldDef *pFieldDef, U32 memOffset, tMD_TypeDef **ppClassTypeArgs);
function_space_specifier void MetaData_Fill_MethodDef(tMD_TypeDef *pParentType, tMD_MethodDef *pMethodDef, tMD_TypeDef **ppClassTypeArgs, tMD_TypeDef **ppMethodTypeArgs);

// Meta-data searching

function_space_specifier U32 MetaData_CompareNameAndSig(STRING name, BLOB_ sigBlob, tMetaData *pSigMetaData, tMD_TypeDef **ppSigClassTypeArgs, tMD_TypeDef **ppSigMethodTypeArgs, tMD_MethodDef *pMethod, tMD_TypeDef **ppMethodClassTypeArgs, tMD_TypeDef **ppMethodMethodTypeArgs);
function_space_specifier tMetaData* MetaData_GetResolutionScopeMetaData(tMetaData *pMetaData, IDX_TABLE resolutionScopeToken, tMD_TypeDef **ppInNestedType);
function_space_specifier PTR MetaData_GetTypeMethodField(tMetaData *pMetaData, IDX_TABLE token, U32 *pObjectType, tMD_TypeDef **ppClassTypeArgs, tMD_TypeDef **ppMethodTypeArgs);
function_space_specifier tMD_TypeDef* MetaData_GetTypeDefFromName(tMetaData *pMetaData, STRING nameSpace, STRING name, tMD_TypeDef *pInNestedClass);
function_space_specifier tMD_TypeDef* MetaData_GetTypeDefFromFullName(STRING assemblyName, STRING nameSpace, STRING name);
function_space_specifier tMD_TypeDef* MetaData_GetTypeDefFromFullNameAndNestedType(STRING assemblyName, STRING nameSpace, STRING name, tMD_TypeDef* nested);
function_space_specifier tMD_TypeDef* MetaData_GetTypeDefFromDefRefOrSpec(tMetaData *pMetaData, IDX_TABLE token, tMD_TypeDef **ppClassTypeArgs, tMD_TypeDef **ppMethodTypeArgs);
function_space_specifier tMD_TypeDef* MetaData_GetTypeDefFromMethodDef(tMD_MethodDef *pMethodDef);
function_space_specifier tMD_TypeDef* MetaData_GetTypeDefFromFieldDef(tMD_FieldDef *pFieldDef);
function_space_specifier tMD_MethodDef* MetaData_GetMethodDefFromDefRefOrSpec(tMetaData *pMetaData, IDX_TABLE token, tMD_TypeDef **ppClassTypeArgs, tMD_TypeDef **ppMethodTypeArgs);
function_space_specifier tMD_FieldDef* MetaData_GetFieldDefFromDefOrRef(tMetaData *pMetaData, IDX_TABLE token, tMD_TypeDef **ppClassTypeArgs, tMD_TypeDef **ppMethodTypeArgs);
function_space_specifier tMD_ImplMap* MetaData_GetImplMap(tMetaData *pMetaData, IDX_TABLE memberForwardedToken);
function_space_specifier STRING MetaData_GetModuleRefName(tMetaData *pMetaData, IDX_TABLE memberRefToken);

// instance functions
function_space_specifier tMetaData* MetaData();
function_space_specifier void MetaData_LoadStrings(tMetaData *pThis, void *pStream, unsigned int streamLen);
function_space_specifier void MetaData_LoadBlobs(tMetaData *pThis, void *pStream, unsigned int streamLen);
function_space_specifier void MetaData_LoadUserStrings(tMetaData *pThis, void *pStream, unsigned int streamLen);
function_space_specifier void MetaData_LoadGUIDs(tMetaData *pThis, void *pStream, unsigned int streamLen);
function_space_specifier void MetaData_LoadTables(tMetaData *pThis, tRVA *pRVA, unsigned char *pStream, unsigned int streamLen);
function_space_specifier PTR MetaData_GetBlob(BLOB_ blob, U32 *pBlobLength);
function_space_specifier STRING2 MetaData_GetUserString(tMetaData *pThis, IDX_USERSTRINGS index, unsigned int *pStringLength);
function_space_specifier void* MetaData_GetTableRow(tMetaData *pThis, IDX_TABLE index);
function_space_specifier void MetaData_GetConstant(tMetaData *pThis, IDX_TABLE idx, PTR pResultMem);



// Not sure where to put this, but this seems best.
function_space_specifier void MetaData_SetField(HEAP_PTR object, tMD_FieldDef * pField, HEAP_PTR value);
function_space_specifier void * MetaData_GetField(HEAP_PTR object, tMD_FieldDef * pField);
function_space_specifier void MetaData_GetFields(tMD_TypeDef * pTypeDef, tMD_FieldDef*** out_buf, int * out_len);
function_space_specifier char * MetaData_GetFieldName(tMD_FieldDef * pFieldDef);
function_space_specifier tMD_TypeDef * MetaData_GetFieldType(tMD_FieldDef * pFieldDef);

// Probably shouldn't be here but oh well...
function_space_specifier unsigned int GetU32(unsigned char *pSource);
function_space_specifier unsigned int GetU16(unsigned char *pSource);
function_space_specifier unsigned long long GetU64(unsigned char *pSource);
function_space_specifier void MetaData_PrintMetaData(tMetaData * meta);
function_space_specifier void * MetaData_GetMethodJit(void * object_ptr, int table_ref);
function_space_specifier void MetaData_SetMethodJit(void * method_ptr, void * bcl_type, int table_ref);
