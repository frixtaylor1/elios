#ifndef elios_HPP
#define elios_HPP

#include <stdbool.h>

#define Packed __attribute__((packed))
#define Public __attribute__((visibility("default")))
#define Private static
#define Constructor __attribute__((constructor))

#define Arr_Size(x) sizeof(x)/sizeof(x[0])
#define Arr_SafeIterate(item, items) (item < &items[Arr_Size(items)])
#define WhileTrue(x) while((x) == true)
#define WhileFalse(x) while((x) == false)
#define IfTrue(x) if ((x) == true)
#define IfFalse(x) if ((x) == false)
#define ForEach(T, ITEM, ITEMS) { T ITEM = ITEMS; int32 $s$i$z$e = Arr_Size(ITEMS);  WhileTrue(ITEM++ < &ITEMS[$s$i$z$e])
#define EForEach }

typedef const char *       cstring;
typedef unsigned char      int8;
typedef unsigned short     int16;
typedef unsigned int       int32;
typedef unsigned long long int64;
typedef int32              word;
typedef void               heap;
typedef int64              mask;

#define $int8 (int8 *)
#define $int16 (int16 *)
#define $int32 (int32 *)
#define $int64 (int64 *)
#define $int (int *)
#define $void (void *)

#define MAX_ENTITIES 1024
#define MAX_COMPONENTS_PER_ENTITY 64
#define NULL_ENTITY 0

#endif // elios_HPP
