//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: comma.cpp                                      **
//**********************************************************
#include "deduce.h"

#define DOT     _T('.')
#define COMMA   _T(',')
#define MAX     50

static TCHAR commas[MAX]; // Where the result is

TCHAR* addCommas(ULONGLONG f) {
    TCHAR tmp[MAX];            // temp area
    _stprintf(tmp, _T("%I64u"), f);    // refine %f if you need
    TCHAR *dot = _tcschr(tmp, DOT); // do we have a DOT?
    TCHAR *src, *dst; // source, dest

    if (dot) {              // Yes
        dst = commas + MAX - _tcslen(dot) - 1; // set dest to allow the fractional part to fit
        _tcscpy(dst, dot);               // copy that part
        *dot = 0;       // 'cut' that frac part in tmp
        src = --dot;    // point to last non frac char in tmp
        dst--;          // point to previous 'free' char in dest
    } else {                 // No
        src = tmp + _tcslen(tmp) - 1;    // src is last char of our float string
        dst = commas + MAX - 1;         // dst is last char of commas
    }

    int len = _tcslen(tmp);        // len is the mantissa size
    int cnt = 0;                  // char counter

    do {
        if (*src <= '9' && *src >= '0') {    // add comma is we added 3 digits already
            if (cnt && !(cnt % 3))
                *dst-- = COMMA;
            cnt++; // mantissa digit count increment
        }
        *dst-- = *src--;
    } while (--len);
    return dst + 1; // return pointer to result
}

