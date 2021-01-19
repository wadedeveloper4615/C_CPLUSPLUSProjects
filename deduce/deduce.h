//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: deduce.h                                       **
//**********************************************************
#ifndef DEDUCE_H_
#define DEDUCE_H_

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <locale.h>
#include <tchar.h>
#include <shellapi.h>
#include "format.h"
#include "list.h"
#include "file.h"

#define LO(x)   ((x) & 0xFF)
#define HI(x)   (((x)>>8) & 0xFF)

typedef struct _MasterBootRecordData {
        int bootablePartitionIndex;
        int dataPartitionIndex;
        MasterBootRecord mbr;
        LinkedList<NTFSBootSector> bootSectors;
} MasterBootRecordData, *MasterBootRecordDataPtr, **MasterBootRecordDataPtrPtr;

//comma.cpp
TCHAR* addCommas(ULONGLONG f);
//deduce.cpp
void gatherDataFromMasterBootRecord(MasterBootRecordDataPtr recordData);
void outputConclusionFromMasterBootRecord(MasterBootRecordDataPtr recordData);
void deduce(TCHAR *VolumeName);
//error.cpp
void exitWithLastError(TCHAR *format, ...);
//print.cpp
void printMasterBootRecord(TCHAR *text, PartitionEntryPtr entry, int i);
void printNTFSBootSector(NTFSBootSectorPtr bootSector);
void readAndPrintMasterBootSector(VolumeIO volume, ULONGLONG offset, int i);
void readAndPrintNTFSBootSector(VolumeIO volume, ULONGLONG offset, int i);
void handleMasterBootRecord(VolumeIO volume, ULONGLONG start, TCHAR *text, MasterBootRecordPtr mbr);
void readNTFSBootSector(VolumeIO volume, ULONGLONG offset, int i, NTFSBootSectorPtr bootSector);
void loadBootRecord(VolumeIO volume, ULONGLONG start, MasterBootRecordDataPtr recordData);
#endif /* DEDUCE_H_ */
