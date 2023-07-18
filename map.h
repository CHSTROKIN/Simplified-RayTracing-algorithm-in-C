#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "graphics.h"

#ifndef MAPDEF
#define MAPDEF

#define TILE_EMPTY  ' '
#define TILE_FLOOR  '.'
#define TILE_WALL   '#'
#define TILE_FRONT  'F'
#define TILE_BACK   'B'
#define TILE_LEFT   'L'
#define TILE_RIGHT  'R'
#define TILE_CORNER 'C'

typedef char tile;
// ax and az are incidence angles - the clockwise x offset
typedef pixel (*get_pixelf)(double, double, double, double);
typedef struct {
    union {
        texture    *texture;
        get_pixelf getter;
    } source;
    bool is_dynamic; // true -> use source.getter
} psource;

typedef struct {
    tile* tiles;
    int max_x;
    int max_y;
    int max_z;
    int wall_height;
} tilemap;

typedef struct {
    psource *left;
    psource *right;
    psource *back;
    psource *front;
    psource *top;
    psource *bottom;
    char tile;
} voxel;

typedef enum {
    VX_LEFT, VX_RIGHT, VX_BACK, VX_FRONT, VX_TOP, VX_BOTTOM
} face;

extern voxel **init_voxels();

extern tile map_get_tile(tilemap map, int x, int y, int z);

#endif
