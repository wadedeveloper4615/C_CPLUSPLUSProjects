/*
 * CIndexBlock.cpp
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */
#include "NTFS_Common.h"
#include "CNTFSVolume.h"
#include "CAttrBase.h"
#include "CAttrResident.h"
#include "CAttrNonResident.h"
#include "CAttrStdInfo.h"
#include "NTFS_Attribute.h"
#include "CFileRecord.h"
#include "CAttrBitmapAttrNonResident.h"
#include "CAttrBitmapAttrResident.h"
#include "CIndexBlock.h"

CIndexBlock::CIndexBlock() {
    NTFS_TRACE("Index Block\n");

    IndexBlock = NULL;
}

CIndexBlock::~CIndexBlock() {
    NTFS_TRACE("IndexBlock deleted\n");

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
