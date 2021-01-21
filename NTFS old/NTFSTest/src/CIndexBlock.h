/*
 * CIndexBlock.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CINDEXBLOCK_H_
#define CINDEXBLOCK_H_

#include "NTFS_DataType.h"
#include "NTFS_Attribute.h"
#include "CSList.h"
#include "CIndexEntry.h"

class CIndexBlock: public CIndexEntryList {
    public:
        CIndexBlock();
        virtual ~CIndexBlock();

    private:
        INDEX_BLOCK *IndexBlock;
    public:
        INDEX_BLOCK* AllocIndexBlock(DWORD size);
};
template struct NTSLIST_ENTRY<CIndexBlock> ;
template class CSList<CIndexBlock> ;

#endif /* CINDEXBLOCK_H_ */
