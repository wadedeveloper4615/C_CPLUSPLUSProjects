/*
 * CPhysicalDrive.h
 *
 *  Created on: Nov 12, 2020
 *      Author: wade4
 */

#ifndef CPHYSICALDRIVE_H_
#define CPHYSICALDRIVE_H_

#include "NTFS.h"
#include "CNTFSVolume.h"

class CPhysicalDrive {
        TCHAR *VolumeName;
        HANDLE VolumeHandle;
        MasterBootRecord mbr;
        CNTFSVolume *partition[4];
    public:
        CPhysicalDrive(TCHAR *VolumeName);
        virtual ~CPhysicalDrive();
        void open(TCHAR *VolumeName);
        void close();
        TCHAR* getName();
        LONGLONG seek(LONGLONG distance, DWORD MoveMethod);
        WINBOOL read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
        MasterBootRecordPtr getMasterBootRecord();
        CNTFSVolume** getPartitions();
        void printMasterBootRecordPartition(PartitionEntryPtr entry, int i);
        void printMasterBootRecord();
        HANDLE getVolumeHandle();
    private:
        void readMasterBootRecord();
};

#endif /* CPHYSICALDRIVE_H_ */
