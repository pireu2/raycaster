//
// Created by sebi on 3/29/2024.
//

#ifndef RAYCASTER_VEC2_H
#define RAYCASTER_VEC2_H
#include "math.h"
#include "macros.h"


typedef struct {
    f64 x, y;
} vec2;

f64 length_vec2(vec2 v);
vec2 normalize_vec2(vec2 v);
f64 dot_vec2(vec2 v0, vec2 v1);
vec2 add_vec2(vec2 v0, vec2 v1);


#endif //RAYCASTER_VEC2_H
