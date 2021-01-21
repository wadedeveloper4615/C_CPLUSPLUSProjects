/*
 * CAttrBitmapAttrResident.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRBITMAPATTRRESIDENT_H_
#define CATTRBITMAPATTRRESIDENT_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrResident.h"

class CAttrBitmapAttrResident: public CAttrResident {

        ULONGLONG BitmapSize;   // Bitmap data size
        BYTE *BitmapBuf;        // Bitmap data buffer
        LONGLONG CurrentCluster;
    public:
        CAttrBitmapAttrResident(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttrBitmapAttrResident();
        BOOL IsClusterFree(ULONGLONG &cluster);
};
template struct NTSLIST_ENTRY<CAttrBitmapAttrResident> ;
template class CSList<CAttrBitmapAttrResident> ;
#endif /* CATTRBITMAPATTRRESIDENT_H_ */
