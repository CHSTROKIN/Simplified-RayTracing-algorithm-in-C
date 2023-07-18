#include "map.h"

// utility to load textures
static texture *load_texture(char *path) {
    FILE *fp = fopen(path, "r");
    int height, width;
    fscanf(fp, "%d %d\n", &height, &width);
    printf("%d, %d\n", height, width);
    
    pixel *pixmap = malloc(sizeof(pixel) * height * width);
    for (int x = 0; x < width; x ++) {
        for (int y = 0; y < height; y ++) {
            // due to a quirk with PIL, this is in column-major order
            pixel *curpix = pixmap + x * height + y;

            int r, g, b;
            fscanf(fp, "%d %d %d\n", &r, &g, &b);

            // default non reflexive
            pixel readpix = { .r = r, .g = g, .b = b, .rfl = 0 };
            *curpix = readpix;
        }
    }

    fclose(fp);

    texture *texture_ptr = malloc(sizeof(texture));
    texture_ptr->height = height;
    texture_ptr->width = width;
    texture_ptr->pixelmap = pixmap;

    return texture_ptr;
}

// utility functions
static texture *make_texture(pixel *pixelmap, int height, int width) {
    texture *texture_ptr = malloc(sizeof(texture));
    texture_ptr->height = height;
    texture_ptr->width = width;
    texture_ptr->pixelmap = pixelmap;
    return texture_ptr;
}

static psource *make_psource_from_texture(texture *tex) {
    psource *psource_ptr = malloc(sizeof(psource));
    psource_ptr->source.texture = tex;
    psource_ptr->is_dynamic = false;
    return psource_ptr;
}

static texture *pure_color_texture(int r, int g, int b, double rfl, int height, int width) {
    pixel color = { .r = r, .g = g, .b = b, .rfl = rfl };
    pixel *pixelmap_color = malloc(sizeof(pixel) * height * width);
    for (int i = 0; i < height * width; i++) {
        pixelmap_color[i] = color;
    }
    return make_texture(pixelmap_color, height, width);
}

static voxel *uniform_voxel(char tile, psource *source) {
    voxel *voxel_ptr  = malloc(sizeof(voxel));
    voxel_ptr->front  = source;
    voxel_ptr->back   = source;
    voxel_ptr->left   = source;
    voxel_ptr->right  = source;
    voxel_ptr->top    = source;
    voxel_ptr->bottom = source;
    voxel_ptr->tile   = tile;
    return voxel_ptr;
}

// textures and pixel suppliers
static pixel get_gradient_pixels(double x, double y, double ax, double az) {
    pixel pix = {
        .r = x * 255,
        .g = y * 255,
        .b = 0,
        .rfl = 0
    };
    return pix;
}

static pixel angle_shaded_white(double x, double y, double ax, double az) {
    double scal = (1.0 + tanh(az)) / 2.0;
    pixel pix = { .r = 255 * scal, .g = 255 * scal, .b = 255 * scal, .rfl = 0 };
    return pix;
}

static pixel bordered_mirror(double x, double y, double ax, double az) {
    if (x < 0.1 || x > 0.9 || y < 0.1 || y > 0.9) {
        pixel border_pix = {
            .r = 100,
            .g = 100,
            .b = 0,
            .rfl = 0
        };
        return border_pix;
    } else {
        pixel mirror_pix = {
            .r = 200,
            .g = 200,
            .b = 200,
            .rfl = 0.8
        };
        return mirror_pix;
    }
}

extern voxel **init_voxels() {
    // ascii to voxel map
    // voxel voxels[128];
    voxel **voxels = calloc(128, sizeof(voxel *));

    // pixel maps
    pixel red = { .r = 255, .g = 0, .b = 0, .rfl = 0 };
    pixel green = { .r = 0, .g = 255, .b = 0, .rfl = 0 };
    pixel *pixelmap_4sq = malloc(sizeof(pixel) * 4);
    pixelmap_4sq[0] = pixelmap_4sq[3] = red;
    pixelmap_4sq[1] = pixelmap_4sq[2] = green;

    // textures
    texture *four_squares_texture_ptr = malloc(sizeof(texture));
    four_squares_texture_ptr->height = 2;
    four_squares_texture_ptr->width = 2;
    four_squares_texture_ptr->pixelmap = pixelmap_4sq;

    texture *basic_wall_texture_ptr = load_texture("assets/brick_wall.c3et");
    texture *white_texture_ptr = pure_color_texture(255, 255, 255, 0, 128, 128);
    texture *green_texture_ptr = pure_color_texture(0, 200, 0, 0, 128, 128);
    texture *yellow_texture_ptr = pure_color_texture(200, 200, 0, 0, 128, 128);
    texture *red_texture_ptr = pure_color_texture(200, 0, 0, 128, 0, 128);
    texture *brown_texture_ptr = pure_color_texture(100, 100, 0, 0, 128, 128);

    // pixel sources
    psource *gradient_psource_ptr = malloc(sizeof(psource));
    gradient_psource_ptr->source.getter = &get_gradient_pixels;
    gradient_psource_ptr->is_dynamic = true;

    psource *shaded_psource_ptr = malloc(sizeof(psource));
    shaded_psource_ptr->source.getter = &angle_shaded_white;
    shaded_psource_ptr->is_dynamic = true;

    psource *mirror_psource_ptr = malloc(sizeof(psource));
    mirror_psource_ptr->source.getter = &bordered_mirror;
    mirror_psource_ptr->is_dynamic = true;

    psource *four_square_psource_ptr = make_psource_from_texture(four_squares_texture_ptr);
    psource *basic_wall_psource_ptr = make_psource_from_texture(basic_wall_texture_ptr);
    psource *white_psource_ptr = make_psource_from_texture(white_texture_ptr);
    psource *green_psource_ptr = make_psource_from_texture(green_texture_ptr);
    psource *yellow_psource_ptr = make_psource_from_texture(yellow_texture_ptr);
    psource *red_psource_ptr = make_psource_from_texture(red_texture_ptr);
    psource *brown_psource_ptr = make_psource_from_texture(brown_texture_ptr);

    // available voxels
    voxel *gradient_wall_ptr = uniform_voxel('#', gradient_psource_ptr);
    voxel *shaded_wall_ptr = uniform_voxel('S', shaded_psource_ptr);
    voxel *four_square_wall_ptr = uniform_voxel('4', four_square_psource_ptr);
    voxel *basic_wall_ptr = uniform_voxel('W', basic_wall_psource_ptr);
    voxel *white_wall_ptr = uniform_voxel('C', white_psource_ptr);
    voxel *green_wall_ptr = uniform_voxel('G', green_psource_ptr);
    voxel *yellow_wall_ptr = uniform_voxel('Y', yellow_psource_ptr);

    voxel *mirror_ptr = uniform_voxel('M', brown_psource_ptr);
    mirror_ptr->front = mirror_psource_ptr;

    // green_wall_ptr->front = gradient_psource_ptr;
    // green_wall_ptr->right = red_psource_ptr;
    // green_wall_ptr->front = red_psource_ptr;
    // green_wall_ptr->back = red_psource_ptr;
    // green_wall_ptr->top = red_psource_ptr;

    voxels['#'] = gradient_wall_ptr;
    voxels['S'] = shaded_wall_ptr;
    voxels['4'] = four_square_wall_ptr;
    voxels['W'] = basic_wall_ptr;
    voxels['C'] = white_wall_ptr;
    voxels['G'] = green_wall_ptr;
    voxels['Y'] = yellow_wall_ptr;
    voxels['M'] = mirror_ptr;

    return voxels;
}

extern tile map_get_tile(tilemap map, int x, int y, int z) {
    return *(map.tiles + (z * (map.max_x * map.max_y) + y * map.max_x + x));
}
