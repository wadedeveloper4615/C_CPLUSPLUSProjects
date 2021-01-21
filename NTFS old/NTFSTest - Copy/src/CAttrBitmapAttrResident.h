/*
 * CAttrBitmapAttrResident.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CATTRBITMAPATTRRESIDENT_H_
#define CATTRBITMAPATTRRESIDENT_H_

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

#endif /* CATTRBITMAPATTRRESIDENT_H_ */
