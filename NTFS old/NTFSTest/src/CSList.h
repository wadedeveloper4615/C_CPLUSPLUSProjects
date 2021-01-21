/*
 * CSList.h
 *
 *  Created on: Nov 4, 2020
 *      Author: wade4
 */

#ifndef CSLIST_H_
#define CSLIST_H_

// List Entry
template<class ENTRY_TYPE>
struct NTSLIST_ENTRY {
        NTSLIST_ENTRY *Next;
        ENTRY_TYPE *Entry;
};

template<class ENTRY_TYPE>
class CSList {
        int EntryCount;
        NTSLIST_ENTRY<ENTRY_TYPE> *ListHead;
        NTSLIST_ENTRY<ENTRY_TYPE> *ListTail;
        NTSLIST_ENTRY<ENTRY_TYPE> *ListCurrent;
    public:
        CSList() {
            ListHead = ListTail = NULL;
            ListCurrent = NULL;
            EntryCount = 0;
        }

        virtual ~CSList() {
            RemoveAll();
        }

        // Get entry count
        int GetCount() const {
            return EntryCount;
        }

        // Insert to tail
        BOOL InsertEntry(ENTRY_TYPE *entry) {
            NTSLIST_ENTRY<ENTRY_TYPE> *le = new NTSLIST_ENTRY<ENTRY_TYPE>;
            if (!le)
                return FALSE;

            le->Entry = entry;
            le->Next = NULL;

            if (ListTail == NULL)
                ListHead = le;      // Empty list
            else
                ListTail->Next = le;

            ListTail = le;

            EntryCount++;
            return TRUE;
        }

        // Remove all entries
        void RemoveAll() {
            while (ListHead) {
                ListCurrent = ListHead->Next;
                delete ListHead->Entry;
                delete ListHead;

                ListHead = ListCurrent;
            }

            ListHead = ListTail = NULL;
            ListCurrent = NULL;
            EntryCount = 0;
        }

        // Find first entry
        ENTRY_TYPE* FindFirstEntry() const {
            ((CSList<ENTRY_TYPE>*) this)->ListCurrent = ListHead;

            if (ListCurrent)
                return ListCurrent->Entry;
            else
                return NULL;
        }

        // Find next entry
        ENTRY_TYPE* FindNextEntry() const {
            if (ListCurrent)
                ((CSList<ENTRY_TYPE>*) this)->ListCurrent = ListCurrent->Next;

            if (ListCurrent)
                return ListCurrent->Entry;
            else
                return NULL;
        }

        // Throw all entries
        // Caution! All entries are just thrown without free
        void ThrowAll() {
            ListHead = ListTail = NULL;
            ListCurrent = NULL;
            EntryCount = 0;
        }
};
#endif /* CSLIST_H_ */
