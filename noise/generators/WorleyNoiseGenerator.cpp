#include <generators/Generators.h>

WorleyNoiseGenerator::WorleyNoiseGenerator(int grid_max, double prob_next, int grid_point_max) {
    this->hash_max = RAND_MAX;
    this->grid_max = grid_max;
    this->prob_next = prob_next;
    this->grid_point_max = grid_point_max;

#ifdef WORLEY_DEBUG
    std::vector<vec2d> lattices;
    // print all lattice points
    for (int i = -1; i <= 10; i++) {
        for (int j = -1; j <= 10; j++) {
            auto points_int = get_grid_points(i, j);
            for (auto &point_int : points_int) {
                vec2d point = point_int.cast_to<double>() / (double)hash_max;
                vec2d actual = point * grid_max + vec2i(i, j).cast_to<double>() * grid_max;

                printf("%lf,%lf,%lf, %d, %d\n", actual.x(), actual.y()
                        , get_point_intensity(vec2i(i,j), point_int), i, j);
                lattices.push_back(actual);
            }
        }
    }

    int width = 20;
    int height = 20;
    double* noise_data = new double[width * height];
    memset(noise_data, 0, sizeof(double)* width * height);

    for (auto& l : lattices) {
        vec2i scr_coord = (l * (width / grid_max)).cast_to<int>();
        noise_data[std::clamp(scr_coord.x(), 0, width - 1) * height + std::clamp(scr_coord.y(), 0, height - 1) ] = 1;
    }


    uint8_t* stbi_data = new uint8_t[width * height];
    for (size_t i = 0; i < width * height; i++)
        stbi_data[i] = static_cast<unsigned char>(std::clamp(noise_data[i] * 255, 0.0, 255.0));
    stbi_write_png("noise_worley_lattice.png", width, height, 1, stbi_data, width);
    delete [] noise_data;
    delete [] stbi_data;
#endif
}

double WorleyNoiseGenerator::get(int x_in, int y_in) {

    vec2d self(x_in, y_in);

    // convert to grid coord
    double x = (double)x_in / grid_max;
    double y = (double)y_in / grid_max;

    // x_0, y_0 are used as indices
    int x_0 = (int)std::floor(x);
    int y_0 = (int)std::floor(y);

    // find nearest neighbor point from 9-way
    // - though 25 lattice is in fact needed
    std::array<vec2i, 9> possible_grid = {
        vec2i(x_0-1, y_0+1), vec2i(x_0, y_0+1), vec2i(x_0+1, y_0+1), 
        vec2i(x_0-1, y_0)  , vec2i(x_0, y_0)  , vec2i(x_0+1, y_0), 
        vec2i(x_0-1, y_0-1), vec2i(x_0, y_0-1), vec2i(x_0+1, y_0-1)
    };

    // 2 neighbor for now
    // - also works best, I think
    std::array<vec2i, 2> dist_points = {
        vec2i(0, 0),  vec2i(0, 0)
    };

    std::array<vec2i, 2> dist_base = {
        vec2i(0, 0),  vec2i(0, 0)
    };

    std::array<double, 2> dist_array = {
        1e20, 1e20
    };

    for (auto &grid_coord : possible_grid) {
        // get all points in grid
        auto points = get_grid_points(grid_coord.x(), grid_coord.y());
        for (auto &point_int : points) {
            // get the actual position of the point
            vec2d point = point_int.cast_to<double>() / (double)hash_max;
            vec2d actual = (point + grid_coord.cast_to<double>()) * grid_max ;
            double curr_dist = std::sqrt((self - actual).squared_norm());
            if (curr_dist < dist_array[0]) {
                dist_points[1] = dist_points[0];
                dist_array[1] = dist_array[0];
                dist_base[1] = dist_base[0];

                dist_points[0] = point_int;
                dist_array[0] = curr_dist;
                dist_base[0] = grid_coord;
            } else if (curr_dist < dist_array[1]) {
                dist_points[1] = point_int;
                dist_array[1] = curr_dist;
                dist_base[1] = grid_coord;
            }
        }
    }

    assert(dist_array[0] < 1e10 && dist_array[1] < 1e10);

    double weight_sum = dist_array[0] + dist_array[1];
    std::array<double, 2> weight_array = {
        dist_array[0] / weight_sum, dist_array[1] / weight_sum
    };

#ifdef WORLEY_DEBUG
    vec2d nearest = dist_base[0].cast_to<double>() * grid_max + dist_points[0].cast_to<double>() / (double)hash_max * grid_max;
    printf("%lf,%lf,%lf,%lf\n", (double)x_in, (double)y_in, nearest.x(), nearest.y());
#endif
        
    //return get_point_intensity(dist_base[0], dist_points[0]) * weight_array[0] 
    //    + get_point_intensity(dist_base[1], dist_points[1]) * weight_array[1];
    //return get_point_intensity(dist_base[0], dist_points[0]) * weight_array[0];
    //return get_point_intensity(dist_base[0], dist_points[0]) / std::sqrt(dist_array[0]);

    //return (1 - (dist_array[0] / grid_max)) ;

    return sorder_ease(dist_array[0], 1.0 /grid_max) * 0.5;

    //double dist_factor = dist_array[0] / grid_max;

    ///* Steps to ensure smoothness:
    // * - Add ease function at border
    // * - dist. punishment
    // */
    //double dist_punish = 1;
    //const double dist_punish_thres = 0;
    //if (dist_array[0] > dist_punish_thres) {
    //    dist_punish = 1.0 / std::sqrt((dist_array[0] - dist_punish_thres));
    //}

    //double near[2] = { get_point_intensity(dist_base[0], dist_points[0]), get_point_intensity(dist_base[1], dist_points[1]) };
    //double delta_dist = std::abs(dist_array[0] - dist_array[1]);
    //const double thres = 8;
    //if (delta_dist < thres) {
    //    double weight_1 = ease((delta_dist / thres) / 2 + 0.5);
    //    double weight_2 = 1 - weight_1;
    //    return (near[0] * weight_1 + near[1] * weight_2) * dist_punish * dist_factor;
    //}
    //else {
    //    return near[0] * dist_punish * dist_factor;
    //}
}

