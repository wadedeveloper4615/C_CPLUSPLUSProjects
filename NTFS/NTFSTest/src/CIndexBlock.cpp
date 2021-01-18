/*
 * CIndexBlock.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CIndexBlock.h"

CIndexBlock::CIndexBlock() {
    printf("Index Block\n");

    IndexBlock = NULL;
}

CIndexBlock::~CIndexBlock() {
    if (IndexBlock)
        delete IndexBlock;
}

INDEX_BLOCK* CIndexBlock::AllocIndexBlock(DWORD size) {
    if (GetCount() > 0)
        RemoveAll();
    if (IndexBlock)
        delete IndexBlock;

    IndexBlock = (INDEX_BLOCK*) new BYTE[size];

    return IndexBlock;
}
