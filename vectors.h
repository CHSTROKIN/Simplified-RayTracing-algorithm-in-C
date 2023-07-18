#include <math.h>

#ifndef VECDEF
#define VECDEF

// quick and dirty solution to some significant issues the implementation has
#define VEC_NORM 1

typedef struct {
    double x;
    double y;
    double z;
} vector;

// computes the l2 norm for the vector in question
extern double vec_norm(vector vec);

// normalize the provided vector
extern vector vec_normalize(vector vec);

// negate a copy the provided vector
extern vector vec_negate(vector vec);

// add two vectors together
extern vector vec_add(vector vec1, vector vec2);

// multiply a vector with a scalar
extern vector vec_mul(vector vec, double scalar);

#endif
