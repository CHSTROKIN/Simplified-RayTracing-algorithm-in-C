#include "rays.h"
// #include <stdlib.h>
#define EPSILON 0.00001
#define TOFFSET 0.001

static double sign(double x) {
    return (x < 0.0 ? -1.0 : 1.0);
}

// static double abs(double x) {
//     return sign(x) * x;
// }

// Implements method outlined in
// https://gamedev.stackexchange.com/questions/169076/collision-detection-in-arkanoid-like-game-at-intersections/169101#169101

// voxel **voxels, vector start, vector vec, tilemap map,
        // double x_rotation, double z_rotation, face *faceptr, int limit,
        // bool debug_print

// render an entire view
extern pixel *render(voxel **voxels, view view, vector start, tilemap map, bool debug) {
    int midz = view.height / 2;
    int midx = view.width / 2;
    pixel *ret = calloc(1, sizeof(pixel) * view.height * view.width);

    for (int vz = 0; vz < view.height; vz ++) {
        for (int vx = 0; vx < view.width; vx ++) {

            bool debug_print = false;
            bool in_center = abs(midz - vz) + abs(midx - vx) < 5;

            if (debug && in_center) {
                debug_print = in_center;
                if (debug_print) {
                    printf("rendering pixel at (%d, %d)\n", vz, vx);
                }
            }

            vector delta = get_delta_from_camera(view, vx, vz);
            pixel pix = shoot(voxels, start, delta, map,
                view.x_rotation, view.z_rotation, debug);
            ret[vz * view.width + vx] = pix;

            if (in_center) {
                ret[vz * view.width + vx].r = 255;
                ret[vz * view.width + vx].g = 0;
                ret[vz * view.width + vx].b = 0;
            }
        }
    }
    
    return ret;
}

// calculate the delta from a relative camera position
extern vector get_delta_from_camera(view view, int x, int z) {
    vector delta = {
        .x = ((double) x) - view.camera_pos.x,
        .y = 0.0 - view.camera_pos.y,
        .z = view.camera_pos.z - ((double) z)
    };
    return delta;
}

static face determine_face(double cur_x, double cur_y, double cur_z,
                           double next_border_x, double next_border_y, double next_border_z,
                           double dir_x, double dir_y, double dir_z) {
    // decide the face
    double x_dist = fabs(next_border_x - cur_x);
    double y_dist = fabs(next_border_y - cur_y);
    double z_dist = fabs(next_border_z - cur_z);

    if (x_dist < y_dist && x_dist < z_dist) {
        // we hit a left/right face
        if (dir_x < 0) return VX_RIGHT; // travelling right to left
        return VX_LEFT; // traveling left to right
    } else if (y_dist < x_dist && y_dist < z_dist) {
        // we hit a front/back face
        if (dir_y < 0) return VX_BACK; // travelling back to front
        return VX_FRONT; // traveling front to back
    } else if (z_dist < x_dist && z_dist < y_dist) {
        // we hit a top/bottom face
        if (dir_z < 0) return VX_TOP; // travelling top to bottom
        return VX_BOTTOM;
    }

    return VX_FRONT;
}

static pixel recursive_shoot(voxel **voxels, vector start, vector vec, tilemap map,
        double x_rotation, double z_rotation, int limit,
        bool debug_print) {
    
    // double norm = vec_norm(vec);

    vector ray_rz = {
        .x = vec.x,
        .y = (cos(z_rotation) * vec.y - sin(z_rotation) * vec.z),
        .z = (sin(z_rotation) * vec.y + cos(z_rotation) * vec.z),
    };
    
    vector ray = {
        .x = (cos(x_rotation) * ray_rz.x - sin(x_rotation) * ray_rz.y),
        .y = (sin(x_rotation) * ray_rz.x + cos(x_rotation) * ray_rz.y),
        .z = (ray_rz.z)
    };

    ray = vec_normalize(ray);

    // printf("x=%f, y=%f, z=%f\n", ray.x, ray.y, ray.z);

    // move the start position in the reverse direction
    // vector moved = vec_add(start, vec_negate(vec_mul(ray, 3)));
    vector moved = start;
    
    // better shoot method that does not rely on small norms
    double dir_x = sign(ray.x);
    double dir_y = sign(ray.y);
    double dir_z = sign(ray.z);

    double cur_x = moved.x;
    double cur_y = moved.y;
    double cur_z = moved.z;

    // printf("cur_z: %f\n", cur_z);

    double next_border_x = floor(cur_x) + (dir_x > 0 ? 1 : 0);
    double next_border_y = floor(cur_y) + (dir_y > 0 ? 1 : 0);
    double next_border_z = floor(cur_z) + (dir_z > 0 ? 1 : 0);

    double x_interval = fabs(next_border_x - cur_x) / (ray.x + EPSILON);
    double y_interval = fabs(next_border_y - cur_y) / (ray.y + EPSILON);
    double z_interval = fabs(next_border_z - cur_z) / (ray.z + EPSILON);

    tile hit_tile = '\0';

    // doesn't need to be an infinite loop, but I'd argue it's actually easier to read
    for(;;) {

        // calculate tile collision
        int tile_x = floor(cur_x);
        int tile_y = floor(cur_y);
        int tile_z = floor(cur_z);

        if (tile_x < 0 || tile_x >= map.max_x) break;
        if (tile_y < 0 || tile_y >= map.max_y) break;
        if (tile_z < 0 || tile_z >= map.max_z) break;

        tile cur_tile = map_get_tile(map, tile_x, tile_y, tile_z);
        if (cur_tile != TILE_EMPTY) {
            hit_tile = cur_tile;
            break;
        }

        next_border_x = floor(cur_x) + (dir_x > 0 ? 1 : 0);
        next_border_y = floor(cur_y) + (dir_y > 0 ? 1 : 0);
        next_border_z = floor(cur_z) + (dir_z > 0 ? 1 : 0);

        x_interval = fabs(next_border_x - cur_x) / fabs(ray.x + EPSILON);
        y_interval = fabs(next_border_y - cur_y) / fabs(ray.y + EPSILON);
        z_interval = (next_border_z - cur_z) / (ray.z + EPSILON);

        double actual_interval = (x_interval < y_interval ? x_interval : y_interval);
        actual_interval = (actual_interval < z_interval ? actual_interval : z_interval);
        actual_interval += TOFFSET;

        cur_x = cur_x + ray.x * actual_interval;
        cur_y = cur_y + ray.y * actual_interval;
        cur_z = cur_z + ray.z * actual_interval;

    }

    face hit_face;

    // determine the face hit by the ray
    hit_face = determine_face(cur_x, cur_y, cur_z,
                           next_border_x, next_border_y, next_border_z,
                           dir_x, dir_y, dir_z);

    double delta_texture_x = 0;
    double delta_texture_y = 0;
    double delta_texture_z = 0;

    // in absolute map terms, not texture terms
    double refl_x, refl_y, refl_z;
    double hit_x, hit_y, ax, az;

    // determine x-offset on the tile
    if (hit_face == VX_FRONT || hit_face == VX_BACK) {
        // the relevant position is cur_x and cur_z
        hit_x = cur_x - floor(cur_x);
        hit_y = cur_z - floor(cur_z);
        
        delta_texture_x = fabs(cur_x - start.x);
        delta_texture_y = fabs(cur_z - start.z);
        delta_texture_z = fabs(cur_y - start.y);

        refl_x = ray.x;
        refl_y = -1 * ray.y;
        refl_z = ray.z;
    } else if (hit_face == VX_LEFT || hit_face == VX_RIGHT) {
        // the relevant position is cur_y and cur_z
        hit_x = cur_y - floor(cur_y);
        hit_y = cur_z - floor(cur_z);

        delta_texture_x = fabs(cur_y - start.y);
        delta_texture_y = fabs(cur_z - start.z);
        delta_texture_z = fabs(cur_x - start.x);

        refl_x = -1 * ray.x;
        refl_y = ray.y;
        refl_z = ray.z;
    } else if (hit_face == VX_TOP || hit_face == VX_BOTTOM) {
        hit_x = cur_x - floor(cur_x);
        hit_y = cur_y - floor(cur_y);

        delta_texture_x = fabs(cur_x - start.x);
        delta_texture_y = fabs(cur_y - start.y);
        delta_texture_z = fabs(cur_z - start.z);

        refl_x = ray.x;
        refl_y = ray.y;
        refl_z = -1 * ray.z;
    }

    // calculate the hit angle
    ax = atan(delta_texture_y / delta_texture_x);
    az = atan(delta_texture_z / delta_texture_x);
    
    if (hit_tile != '\0') {
         // figure out the voxel
        voxel hit_voxel = *(voxels[(int) hit_tile]);
        // printf("%c\n", hit_tile);
        psource *pixsrc;

        switch (hit_face) {
            case VX_FRONT:  pixsrc = hit_voxel.front;  break;
            case VX_BACK:   pixsrc = hit_voxel.back;   break;
            case VX_LEFT:   pixsrc = hit_voxel.left;   break;
            case VX_RIGHT:  pixsrc = hit_voxel.right;  break;
            case VX_TOP:    pixsrc = hit_voxel.top;    break;
            case VX_BOTTOM: pixsrc = hit_voxel.bottom; break;
        }

        pixel cur_pixel;

        if (pixsrc->is_dynamic) {
            cur_pixel = (*(pixsrc->source.getter))(hit_x, hit_y, ax, az);
        } else {
            int width = pixsrc->source.texture->width;
            int height = pixsrc->source.texture->height;

            int texture_x = floor(hit_x * width);
            int texture_y = floor(hit_y * height);
            
            cur_pixel = *(pixsrc->source.texture->pixelmap + (width * texture_y + texture_x));
        }

        if (debug_print) {
            printf("ht_tile: %c\n", hit_tile);
            printf("ht_face: %d\n", hit_face);
            printf("o_color: %f, %f, %f\n", cur_pixel.r, cur_pixel.g, cur_pixel.b);
            printf("ori_ray: %f, %f, %f\n", ray.x, ray.y, ray.z);
        }

        // for the purposes of this test, everything has a reflexivity of .5
        double reflexivity = cur_pixel.rfl;
        if (limit > 0) {
            double nhx, nhy, nax, nay;
            face nf;
            vector new_start = { .x = cur_x - ray.x * 2 * TOFFSET,
                                 .y = cur_y - ray.y * 2 * TOFFSET,
                                 .z = cur_z - ray.z * 2 * TOFFSET };
            vector new_ray = { .x = refl_x, .y = refl_y, .z = refl_z };

            if (debug_print) {
                double ray_norm = vec_norm(new_ray);
        
                
                printf("new_ray: %f, %f, %f\n", refl_x / ray_norm,
                                                refl_y / ray_norm,
                                                refl_z / ray_norm);
            }
        
            pixel next_pixel = recursive_shoot(voxels, new_start, new_ray, map,
                                            0, 0, limit - 1,
                                            debug_print);

            if (debug_print) {
                printf("rfl_col: %f, %f, %f\n", next_pixel.r, next_pixel.g, next_pixel.b); 
            }          

            cur_pixel.r = (1 - reflexivity) * cur_pixel.r + reflexivity * next_pixel.r;
            cur_pixel.g = (1 - reflexivity) * cur_pixel.g + reflexivity * next_pixel.g;
            cur_pixel.b = (1 - reflexivity) * cur_pixel.b + reflexivity * next_pixel.b;
            
        }

        return cur_pixel;


    } else if (cur_z <= 0) {
        // return TILE_FLOOR;
        pixel green_ground = {.r = 0, .g = 128, .b = 0};
        return green_ground;
    } else {
        // return TILE_EMPTY;
        pixel blue_sky = {.r = 0, .g = 128, .b = 255};
        return blue_sky;
    }
}

// raycasting 'shoot' method
extern pixel shoot(voxel **voxels, vector start, vector vec, tilemap map,
        double x_rotation, double z_rotation,
        bool debug_print) {
    return recursive_shoot(voxels, start, vec, map, x_rotation, z_rotation,
            1, debug_print);
}
