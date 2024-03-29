//
// Created by sebi on 3/29/2024.
//

#include "vec2.h"

f64 length_vec2(vec2 v) {
    return sqrt((v.x * v.x) + (v.y * v.y));
}

vec2 normalize_vec2(vec2 v) {
    f64 l = length_vec2(v);
    return (vec2){v.x / l, v.y / l};
}

f64 dot_vec2(vec2 v0, vec2 v1) {
    return (v0.x * v1.x) + (v0.y * v1.y);
}

vec2 add_vec2(vec2 v0, vec2 v1){
    return (vec2) {v0.x + v1.x, v0.y + v1.y};
}
