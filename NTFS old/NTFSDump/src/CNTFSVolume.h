/*
 * CNTFSVolume.h
 *
 *  Created on: Nov 5, 2020
 *      Author: wade4
 */

#ifndef CNTFSVOLUME_H_
#define CNTFSVOLUME_H_

#include "NTFS.h"
#include "CSList.h"

class CPhysicalDrive;
class CFileRecord;

class CNTFSVolume {
        WORD sectorSize;
        DWORD clusterSize;
        DWORD fileRecordSize;
        DWORD indexBlockSize;
        ULONGLONG MFTAddr;
        BOOL volumeOK;
        CPhysicalDrive *drive;
        ULONGLONG partitionOffset;
        NTFSBootSector bootSector;
        CFileRecord *fileRecord;
        CFileRecord *MFTRecord;
        const CAttrBase *MFTData;
    public:
        CNTFSVolume(CPhysicalDrive *drive, int i);
        virtual ~CNTFSVolume();
        BOOL IsVolumeOK() const;
        WORD GetVersion() const;
        WORD getSectorSize() const;
        DWORD getClusterSize() const;
        DWORD getFileRecordSize() const;
        DWORD getIndexBlockSize() const;
        ULONGLONG getMFTAddr() const;
        CPhysicalDrive* getDrive() const;
        ULONGLONG getPartitionOffset() const;
        NTFSBootSector getBootSector() const;
        CFileRecord* getFileRecord() const;
        CFileRecord* getMFTRecord() const;
        const CAttrBase* getMFTData() const;
    private:
        BOOL OpenVolume(CPhysicalDrive *drive, int i);
};
template struct NTSLIST_ENTRY<CNTFSVolume> ;
template class CSList<CNTFSVolume> ;

#endif /* CNTFSVOLUME_H_ */
