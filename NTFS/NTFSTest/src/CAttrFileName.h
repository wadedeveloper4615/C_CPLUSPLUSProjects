/*
 * CAttrFileName.h
 *
 *  Created on: Nov 6, 2020
 *      Author: wade4
 */

#ifndef CATTRFILENAME_H_
#define CATTRFILENAME_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CAttrResident.h"
#include "CFileName.h"

class CAttr_FileName: public CAttrResident, public CFileName {
        // File permission and time in $FILE_NAME only updates when the filename changes
        // So hide these functions to prevent user from getting the error information
        // Standard Information and IndexEntry keeps the most recent file time and permission infomation
        void GetFileTime(FILETIME *writeTm, FILETIME *createTm = NULL, FILETIME *accessTm = NULL) const;
        DWORD GetFilePermission();
        BOOL IsReadOnly() const;
        BOOL IsHidden() const;
        BOOL IsSystem() const;
        BOOL IsCompressed() const;
        BOOL IsEncrypted() const;
        BOOL IsSparse() const;
    public:
        CAttr_FileName(const ATTR_HEADER_COMMON *ahc, const CFileRecord *fr);
        virtual ~CAttr_FileName();
};
template struct NTSLIST_ENTRY<CAttr_FileName> ;
template class CSList<CAttr_FileName> ;

#endif /* CATTRFILENAME_H_ */
