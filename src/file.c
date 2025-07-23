#include <utility/file.h>

Elios_Public void close_file(FILE **fp) {
    IfTrue ((bool) fp && (bool) *fp) fclose(*fp);
}