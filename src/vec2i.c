//
// Created by sebi on 3/29/2024.
//
#include "vec2i.h"

f64 length_vec2i(vec2i v) {
    return sqrt((v.x * v.x) + (v.y * v.y));
}
i32 dot_vec2i(vec2i v0, vec2i v1) {
    return (v0.x * v1.x) + (v0.y * v1.y);
}
vec2i add_vec2i(vec2i v0, vec2i v1){
    return (vec2i) {v0.x + v1.x, v0.y + v1.y};
}
vec2i mul_vec2i(vec2i v0, i32 s){
    return (vec2i) {v0.x * s, v0.y * s};
}