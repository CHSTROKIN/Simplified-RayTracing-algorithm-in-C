#include <math.h>
#include <stdio.h>

#include <string.h>

#include "map.h"
#include "vectors.h"
#include "graphics.h"

// raycasting 'shoot' method
extern pixel shoot(voxel **voxels, vector start, vector vec, tilemap map,
    double x_rotation, double z_rotation,
    bool debug_print);

// calculate the delta from a relative camera position
extern vector get_delta_from_camera(view view, int x, int z);

// render an entire view
extern pixel *render(voxel **voxels, view view, vector start, tilemap map, bool debug);
