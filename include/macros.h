//
// Created by sebi on 3/29/2024.
//

#ifndef RAYCASTER_MACROS_H
#define RAYCASTER_MACROS_H

typedef float f32;
typedef double f64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long int i64;

#define max(a,b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b);_a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b); _a < _b ? _a : _b; })
#define sign(a) ({ __typeof__(a) _a = (a); _a > 0 ? 1 : _a < 0 ? -1 : 0; })

#endif //RAYCASTER_MACROS_H
