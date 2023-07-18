#include "graphics.h"

extern void set_camera_position(double h_angle, double v_angle, view *viewport) {
    // double half_h_angle = h_angle / 2;
    double half_v_angle = v_angle / 2;

    // printf("%f %f\n", half_h_angle, half_v_angle);

    double half_height = viewport->height/2;
    double half_width  = viewport->width/2;

    // hmm ... the focal lengths might not sync up
    double focal_length_v = half_height / tan(half_v_angle);
    // double focal_length_h = half_width / tan(half_h_angle);

    // printf("%f %f\n", focal_length_v, focal_length_h);

    viewport->camera_pos.x = half_width;
    viewport->camera_pos.z = half_height;
    viewport->camera_pos.y = focal_length_v;
}
