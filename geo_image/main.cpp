#include <tiny_gltf.h>
#include <UHEMesh/HEMesh.h>
#include <iostream>
#include <cstdio>
#include <string>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s glTF-model-path mesh-number\n");
        return 1;
    }
    tinygltf::TinyGLTF loader;

    tinygltf::Model model;
    {
        std::string err;
        std::string warn;
        bool res = loader.LoadASCIIFromFile(&model, &err, &warn, std::string(argv[1]));
        if (!warn.empty()) {
            std::cout << "WARN: " << warn << std::endl;
        }

        if (!err.empty()) {
            std::cout << "ERR: " << err << std::endl;
        }

        if (!res) {
            std::cout << "Failed to load glTF: " << argv[1] << std::endl;
        } else {
            std::cout << "Loaded glTF: " << argv[1] << std::endl;
        }
    }

    tinygltf::Mesh mesh;
    {
        int mesh_num;
        int res = sscanf(argv[2], "%d", &mesh_num);
        if (res != 1) {
            fprintf(stderr, "Error while reading mesh number, integer required\n");
            return 1;
        }
        mesh = model.meshes[mesh_num];
    }

    
}