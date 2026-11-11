 /*
  * Copyright 2026 Alan Crispin <crispinalan@gmail.com>
  * SPDX-License-Identifier: GPL-2.0-or-later
  */

#ifndef DIPHONE_H
#define DIPHONE_H

#include <string>
#include <vector>

// The structure is fine as is
typedef struct {
    const char* name;
    const unsigned char* data;
    const unsigned int size;
} DiphoneEntry;

class Diphone {
public:
    Diphone();
    // This function finds the entry in the table
    static DiphoneEntry* findDiphoneEntry(const char* diphoneStr);

private:
    // We only declare that the table exists somewhere else
    static const DiphoneEntry diphone_table[];
    static const unsigned int diphoneTableSize;
};

#endif

