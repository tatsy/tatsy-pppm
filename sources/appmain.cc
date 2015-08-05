#include "photon_diffusion.h"

#include <cstdio>
#include <string>

int main(int argc, char** argv) {
    // Load mesh
    Trimesh trimesh;
    trimesh.load(ASSET_DIRECTORY + "gargoil.ply");
    
    // Set scene
    Scene scene;
    scene.add(trimesh, LambertianBRDF::factory(Vector3D(0.75, 0.75, 0.75)), false);
}
