/*
 * CAttrVolInfo.h
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */

#ifndef CATTRVOLINFO_H_
#define CATTRVOLINFO_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrResident.h"
class CFileRecord;

class CAttr_VolInfo: public CAttrResident {
    public:
        CAttr_VolInfo(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_VolInfo();
    private:
        const ATTR_VOLUME_INFORMATION *VolInfo;

    public:
        // Get NTFS Volume Version
        WORD GetVersion();
};
template struct NTSLIST_ENTRY<CAttr_VolInfo> ;
template class CSList<CAttr_VolInfo> ;

#endif /* CATTRVOLINFO_H_ */
