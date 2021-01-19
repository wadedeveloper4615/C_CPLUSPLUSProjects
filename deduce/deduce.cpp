//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: deduce.cpp                                      **
//**********************************************************
#include "deduce.h"

void gatherDataFromMasterBootRecord(MasterBootRecordDataPtr recordData) {
    for (int i = 0; i <= 3; i++) {
        PartitionEntryPtr entry = &recordData->mbr.partionTable[i];
        if (entry->fileSystem == 0x07 && entry->bootType == 0x80) {
            recordData->bootablePartitionIndex = i;
        }
        if (entry->fileSystem == 0x07 && entry->bootType == 0x00) {
            recordData->dataPartitionIndex = i;
        }
    }
}

void outputConclusionFromMasterBootRecord(MasterBootRecordDataPtr recordData) {
    _tprintf(_T("Bootable Partition is partition   : %d\n"), recordData->bootablePartitionIndex);
    _tprintf(_T("Drive Data Partition is partition : %d\n"), recordData->dataPartitionIndex);
}

void deduce(TCHAR *VolumeName) {
    VolumeIO volume;
    MasterBootRecordData recordData;
    DWORD read;

    _tprintf(_T("Processing Volume '%s'\n"), VolumeName);
    volume.open(VolumeName);
    volume.seek(0, FILE_BEGIN);
    _tprintf(_T("Reading Master Boot Record...\n"), VolumeName);
    volume.read(&recordData.mbr, sizeof(MasterBootRecord), &read);
    if (read != sizeof(MasterBootRecord)) {
        exitWithLastError(_T("read in %ld instead of %d in dumpDrive (%lx)\n"), read, sizeof(MasterBootRecord), GetLastError());
    } else {
        _tprintf(_T("Read %d bytes of the Master Boot Record...\n"), read);
    }
    loadBootRecord(volume, 0, &recordData);
    gatherDataFromMasterBootRecord(&recordData);
    outputConclusionFromMasterBootRecord(&recordData);
    _tprintf(_T("Processing Complete for Volume '%s'\n"), VolumeName);
    volume.close();
}

int main(int argc, char *argv[]) {
    _tsetlocale(LC_NUMERIC, _T(""));
    deduce(_T("\\\\.\\PhysicalDrive0"));
    return 0;
}
