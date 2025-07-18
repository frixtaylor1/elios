#ifndef elios_HPP
#define elios_HPP

#include <stdbool.h>
#include <stdlib.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define Elios_Packed  __attribute__((packed))
#define Elios_Public __attribute__((visibility("default")))
#define Elios_Private static
#define Elios_Constructor __attribute__((constructor))
#define Elios_Destructor __attribute__((destructor))

#define Arr_Size(x) sizeof(x)/sizeof(x[0])
#define Arr_SafeIterate(item, items) (item < &items[Arr_Size(items)])
#define While(cond_expr, block)                  \
    {                                            \
        goto Elios_Loop_Start;                   \
        Elios_Loop_Start:                        \
        IfFalse (cond_expr) goto Elios_Loop_End; \
        block;                                   \
        goto Elios_Loop_Start;                   \
        Elios_Loop_End:;                         \
    }
#define WhileTrue(x) while((x) == true)
#define WhileFalse(x) while((x) == false)
#define IfTrue(x) if ((x) == true)
#define IfFalse(x) if ((x) == false)
#define ForEach(T, ITEM, ITEMS) { T ITEM = ITEMS; int32 $s$i$z$e = Arr_Size(ITEMS);  WhileTrue(ITEM++ < &ITEMS[$s$i$z$e]) {
#define EForEach }}
#define ForRange(T, ITEM, FROM, TO) { T ITEM = FROM; WhileTrue(++ITEM < TO) {
#define EForRange }}

#define ThrowErr(errNumber, errMessage, ...)        \
    do {                                            \
        fprintf(stderr, "ERROR %d:", errNumber);    \
        fprintf(stderr, errMessage, ##__VA_ARGS__); \
        fprintf(stderr, "\n");                      \
        exit(errNumber);                            \
    } while (0)

typedef const char *       c_string;
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

#define MAX_ENTITIES 7000
#define MAX_COMPONENTS_PER_ENTITY 64
#define NULL_ENTITY 0

#endif // elios_HPP