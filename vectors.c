#include "vectors.h"
#include <stdio.h>

// computes the l2 norm for the vector in question
extern double vec_norm(vector vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

// normalize the provided vector
extern vector vec_normalize(vector vec) {
    double norm = vec_norm(vec) / VEC_NORM;
    vector norm_vec = {
        .x = vec.x / norm,
        .y = vec.y / norm,
        .z = vec.z / norm
    };
    return norm_vec;
}

// negate a copy the provided vector
extern vector vec_negate(vector vec) {
    vector neg_vec = {
        .x = -1 * vec.x,
        .y = -1 * vec.y,
        .z = -1 * vec.z
    };
    return neg_vec;
}

// add two vectors together
extern vector vec_add(vector vec1, vector vec2) {
    vector sum_vec = {
        .x = vec1.x + vec2.x,
        .y = vec1.y + vec2.y,
        .z = vec1.z + vec2.z
    };
    return sum_vec;
}

extern vector vec_mul(vector vec, double scalar) {
    vector scal_vec = {
        .x = vec.x * scalar,
        .y = vec.y * scalar,
        .z = vec.z * scalar
    };
    return scal_vec;
}
