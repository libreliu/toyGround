#include <stb_image_write.h>
#include <stb_image.h>

#include <NoiseWriter.h>
#include <generators/Generators.h>

class PictureBlender {
private:
    std::vector<std::string> names;
    std::vector<double> weights;
    std::string output_name;
public:
    PictureBlender(const char* file_out) {
        output_name = file_out;
    }

    PictureBlender& add(const char* filename, double weight) {
        names.push_back(filename);
        weights.push_back(weight);
        
        return *this;
    }

    bool run() {
        int w_init, h_init, n_init;
        stbi_uc* buf = stbi_load(names[0].c_str(), &w_init, &h_init, &n_init, 0);

        if (buf == nullptr) {
            return false;
        }

        assert(n_init == 1);

        for (int i = 0; i < h_init; i++) {
            for (int j = 0; j < w_init; j++) {
                // todo: change to double to avoid unnecessary precision loss
                buf[i * w_init * n_init + j * n_init + 0] *= weights[0];
            }
        }

        for (int m = 1; m < names.size(); m++) {
            int w, h, n;
            stbi_uc* buf_imm = stbi_load(names[m].c_str(), &w, &h, &n, 0);

            assert(w == w_init && h == h_init && n == n_init);

            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    buf[i * w * n + j * n + 0] += weights[m] * buf_imm[i * w * n + j * n + 0];
                }
            }

            stbi_image_free(buf_imm);
        }

        stbi_write_png(output_name.c_str(), w_init, h_init, n_init, buf, w_init * n_init);

        stbi_image_free(buf);
        
        return true;
    }


    bool run_direct() {
        int w_init, h_init, n_init;
        stbi_uc* buf = stbi_load(names[0].c_str(), &w_init, &h_init, &n_init, 0);

        if (buf == nullptr) {
            return false;
        }

        assert(n_init == 1);

        for (int m = 1; m < names.size(); m++) {
            int w, h, n;
            stbi_uc* buf_imm = stbi_load(names[m].c_str(), &w, &h, &n, 0);

            assert(w == w_init && h == h_init && n == n_init);

            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    buf[i * w * n + j * n + 0] = 
                        std::clamp((int)((double)buf[i * w * n + j * n + 0] / 255 * buf_imm[i * w * n + j * n + 0]), 0, 255);
                }
            }

            stbi_image_free(buf_imm);
        }

        stbi_write_png(output_name.c_str(), w_init, h_init, n_init, buf, w_init * n_init);

        stbi_image_free(buf);

        return true;
    }

private:

};

int main(void) {
    NoiseWriter<UniformNoiseGenerator> uniform("noise_uniform.png", 1024, 1024);
    uniform.run();

    NoiseWriter<PerlinNoiseGenerator2D> perlin("noise_perlin.png", 1024, 1024, 200, 1);
    perlin.run();

    NoiseWriter<PerlinNoiseGenerator2D> perlin_dense("noise_perlin_dense.png", 1024, 1024, 100, 1);
    perlin_dense.run();

#ifdef WORLEY_DEBUG
    srand(0);
#endif
    NoiseWriter<WorleyNoiseGenerator> worley("noise_worley.png", 1024, 1024, 50, 0.3, 5, 1);
    //NoiseWriter<WorleyNoiseGenerator> worley("noise_worley.png", 1024, 1024, 100, 0.3, 5);
    worley.run();

    //{

    //    PictureBlender blender("noise_perlin_blended.png");
    //    blender.add("noise_perlin.png", 0.7).add("noise_perlin_dense.png", 0.3).run();
    //}

    PictureBlender blender("noise_blended.png");
    blender.add("noise_worley.png", 0.2).add("noise_perlin.png", 0.8).run_direct();

    //NoiseWriter<FractualNoiseGenerator2D<PerlinNoiseGenerator2D>> fractual("noise_fractual.png", 1024, 1024, 5, 400);
    //fractual.run();

    NoiseWriter<CompositeGenerator<FractualNoiseGenerator2D<PerlinNoiseGenerator2D>>> composite_fractual("noise_fractual.png", 1024, 1024, 10, 5, 400, 1);
    composite_fractual.run();

    {

        PictureBlender blender("noise.png");
        blender.add("noise_fractual.png", 0.2).add("noise_worley.png", 0.8).run_direct();
    }
}