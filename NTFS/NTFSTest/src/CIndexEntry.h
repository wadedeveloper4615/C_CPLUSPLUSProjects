/*
 * CIndexEntry.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CINDEXENTRY_H_
#define CINDEXENTRY_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CFileName.h"

class CIndexEntry: public CFileName {
    public:
        CIndexEntry();
        CIndexEntry(const INDEX_ENTRY *ie);
        virtual ~CIndexEntry();
    private:
        BOOL IsDefault;

    protected:
        const INDEX_ENTRY *IndexEntry;

    public:
        // Use with caution !
        CIndexEntry& operator =(const CIndexEntry &ieClass);
        ULONGLONG GetFileReference() const;
        BOOL IsSubNodePtr() const;
        ULONGLONG GetSubNodeVCN() const;
};
template struct NTSLIST_ENTRY<CIndexEntry> ;
template class CSList<CIndexEntry> ;
typedef class CSList<CIndexEntry> CIndexEntryList;

#endif /* CINDEXENTRY_H_ */
