/*
 * CAttrBitmapAttrNonResident.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRBITMAPATTRNONRESIDENT_H_
#define CATTRBITMAPATTRNONRESIDENT_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrNonResident.h"

class CAttrBitmapAttrNonResident: public CAttrNonResident {
        ULONGLONG BitmapSize;   // Bitmap data size
        BYTE *BitmapBuf;        // Bitmap data buffer
        LONGLONG CurrentCluster;
    public:
        CAttrBitmapAttrNonResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttrBitmapAttrNonResident();
        BOOL IsClusterFree(ULONGLONG &cluster);
};
template struct NTSLIST_ENTRY<CAttrBitmapAttrNonResident> ;
template class CSList<CAttrBitmapAttrNonResident> ;
#endif /* CATTRBITMAPATTRNONRESIDENT_H_ */
