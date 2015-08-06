#include "renderer.h"

#include <cstdio>
#include <string>

static const int IMAGE_WIDTH = 800;
static const int IMAGE_HEIGHT = 600;

int main(int argc, char** argv) {
    // Load mesh
    Trimesh trimesh;
    trimesh.load(ASSET_DIRECTORY + "gargoil.ply");
    trimesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    trimesh.buildAccel();

    // Load environment map
    Envmap envmap(ASSET_DIRECTORY + "gold_room.hdr");
    
    // Set scene
    Scene scene;
    scene.add(trimesh, LambertianBRDF::factory(Vector3D(0.75, 0.75, 0.75)), false);
    scene.setEnvmap(envmap);

    // Set camera
    Camera camera(Vector3D(0.0, 0.0, 100.0), Vector3D(0.0, 0.0, -1.0), 50.0, 50.0, IMAGE_WIDTH, IMAGE_HEIGHT);

    // Set render parameters
    RenderParameters params(1000000, 16);

    // Set renderer
    ProgressivePhotonMapping ppm;
    ppm.render(scene, camera, params);
}
