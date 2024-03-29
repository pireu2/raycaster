//
// Created by sebi on 3/29/2024.
//

#ifndef RAYCASTER_VEC2I_H
#define RAYCASTER_VEC2I_H
#include "macros.h"
#include "math.h"

typedef struct{
    i32 x, y;
} vec2i;

f64 length_vec2i(vec2i v);
i32 dot_vec2i(vec2i v0, vec2i v1);
vec2i add_vec2i(vec2i v0, vec2i v1);
vec2i mul_vec2i(vec2i v0, i32 s);


#endif //RAYCASTER_VEC2I_H
