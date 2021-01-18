/*
 * CIndexBlock.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CINDEXBLOCK_H_
#define CINDEXBLOCK_H_

class CIndexBlock: public CIndexEntryList {
    public:
        CIndexBlock();
        virtual ~CIndexBlock();
    private:
        INDEX_BLOCK *IndexBlock;
    public:
        INDEX_BLOCK* AllocIndexBlock(DWORD size);
};
// CIndexBlock

#endif /* CINDEXBLOCK_H_ */
