#ifndef GRAPHDEF
#define GRAPHDEF

#include <math.h>
#include <stdio.h>

#include "vectors.h"

// #include "rays.h"
typedef struct {
    int width;
    int height;
    vector camera_pos;
    double x_rotation;
    double z_rotation;
} view;

typedef struct {
    double r;
    double g;
    double b;
    double rfl;
} pixel;

typedef struct {
    pixel *pixelmap;
    int height;
    int width;
} texture;

// convert from hue-color-saturation to RGB color mode
extern pixel from_hsb(int h, int s, int b);

// convert angles of view and view specification into camera position,
// setting the camera position structure of the provided view
extern void set_camera_position(double h_angle, double v_angle, view *viewport);

#endif
