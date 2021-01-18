/*
 * NTFS_Common.h
 *
 *  Created on: Nov 3, 2020
 *      Author: wade4
 */

#ifndef NTFS_COMMON_H_
#define NTFS_COMMON_H_

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <crtdbg.h>

#include "NTFS_DataType.h"
#include "CAttrBase.h"

#define ATTR_NUMS       16              // Attribute Types count
#define ATTR_INDEX(at)  (((at)>>4)-1)   // Attribute Type to Index, eg. 0x10->0, 0x30->2
#define ATTR_MASK(at)   (((DWORD)1)<<ATTR_INDEX(at))    // Attribute Bit Mask

// Bit masks of Attributes
#define MASK_STANDARD_INFORMATION   ATTR_MASK(ATTR_TYPE_STANDARD_INFORMATION)
#define MASK_ATTRIBUTE_LIST         ATTR_MASK(ATTR_TYPE_ATTRIBUTE_LIST)
#define MASK_FILE_NAME              ATTR_MASK(ATTR_TYPE_FILE_NAME)
#define MASK_OBJECT_ID              ATTR_MASK(ATTR_TYPE_OBJECT_ID)
#define MASK_SECURITY_DESCRIPTOR    ATTR_MASK(ATTR_TYPE_SECURITY_DESCRIPTOR)
#define MASK_VOLUME_NAME            ATTR_MASK(ATTR_TYPE_VOLUME_NAME)
#define MASK_VOLUME_INFORMATION     ATTR_MASK(ATTR_TYPE_VOLUME_INFORMATION)
#define MASK_DATA                   ATTR_MASK(ATTR_TYPE_DATA)
#define MASK_INDEX_ROOT             ATTR_MASK(ATTR_TYPE_INDEX_ROOT)
#define MASK_INDEX_ALLOCATION       ATTR_MASK(ATTR_TYPE_INDEX_ALLOCATION)
#define MASK_BITMAP                 ATTR_MASK(ATTR_TYPE_BITMAP)
#define MASK_REPARSE_POINT          ATTR_MASK(ATTR_TYPE_REPARSE_POINT)
#define MASK_EA_INFORMATION         ATTR_MASK(ATTR_TYPE_EA_INFORMATION)
#define MASK_EA                     ATTR_MASK(ATTR_TYPE_EA)
#define MASK_LOGGED_UTILITY_STREAM  ATTR_MASK(ATTR_TYPE_LOGGED_UTILITY_STREAM)

#define MASK_ALL                    ((DWORD)-1)

#define NTFS_TRACE(t1)                  _RPT0(_CRT_WARN, t1)
#define NTFS_TRACE1(t1, t2)             _RPT1(_CRT_WARN, t1, t2)
#define NTFS_TRACE2(t1, t2, t3)         _RPT2(_CRT_WARN, t1, t2, t3)
#define NTFS_TRACE3(t1, t2, t3, t4)     _RPT3(_CRT_WARN, t1, t2, t3, t4)
#define NTFS_TRACE4(t1, t2, t3, t4, t5) _RPT4(_CRT_WARN, t1, t2, t3, t4, t5)

// User defined Callback routines to process raw attribute data
// Set bDiscard to TRUE if this Attribute is to be discarded
// Set bDiscard to FALSE to let CFileRecord process it
typedef void (*ATTR_RAW_CALLBACK)(const ATTR_HEADER_COMMON *attrHead, BOOL *bDiscard);

// User defined Callback routine to handle CFileRecord parsed attributes
// Will be called by CFileRecord::TraverseAttrs() for each attribute
// attrClass is the according attribute's wrapping class, CAttr_xxx
// Set bStop to TRUE if don't want to continue
// Set bStop to FALSE to continue processing
class CAttrBase;
typedef void (*ATTRS_CALLBACK)(const CAttrBase *attr, void *context, BOOL *bStop);

// User defined Callback routine to handle Directory traversing
// Will be called by CFileRecord::TraverseSubEntries for each sub entry
class CIndexEntry;
typedef void (*SUBENTRY_CALLBACK)(const CIndexEntry *ie);

void exitWithLastError(TCHAR *format, ...);

typedef struct tagDataRun_Entry {
        LONGLONG LCN;        // -1 to indicate sparse data
        ULONGLONG Clusters;
        ULONGLONG StartVCN;
        ULONGLONG LastVCN;
} DataRun_Entry;

#endif /* NTFS_COMMON_H_ */
