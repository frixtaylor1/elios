#ifndef file_H
#define file_H

#include <elios.h>
#include <stdio.h>

#define Elios_Safe_File Elios_Defer_With(close_file) FILE *

Elios_Public void close_file(FILE **fp);

#endif // file_H