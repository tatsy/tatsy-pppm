#include "renderer.h"

#include <cstdio>
#include <string>

static const int    IMAGE_WIDTH  = 1280;
static const int    IMAGE_HEIGHT = 720;
static const int    TILES        = 8;
static const double TILE_SIZE    = 8.0;

int main(int argc, char** argv) {
    // Load mesh
    Trimesh trimesh;
    trimesh.load(ASSET_DIRECTORY + "gargoil.ply");
    trimesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    trimesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));

    // Load environment map
    Envmap envmap(ASSET_DIRECTORY + "gold_room.hdr");
    
    // Set scene
    Scene scene;
    scene.add(trimesh, LambertianBRDF::factory(Vector3D(0.7, 0.7, 0.7)));
    scene.setEnvmap(envmap);

    // Set floor
    for (int i = 0; i < TILES; i++) {
        for (int j = 0; j < TILES; j++) {
            double ii = (i - TILES / 2) * TILE_SIZE;
            double jj = (j - TILES / 2) * TILE_SIZE;
            Vector3D p00(ii, -10.0, jj);
            Vector3D p01(ii + TILE_SIZE, -10.0, jj);
            Vector3D p10(ii, -10.0, jj + TILE_SIZE);
            Vector3D p11(ii + TILE_SIZE, -10.0, jj + TILE_SIZE);
            Vector3D color = (i + j) % 2 == 0 ? Vector3D(0.7, 0.7, 0.7) : Vector3D(0.3, 0.3, 0.3);
            //scene.add(Triangle(p00, p11, p01), LambertianBRDF::factory(color));
            //scene.add(Triangle(p00, p10, p11), LambertianBRDF::factory(color));
            scene.add(Triangle(p00, p11, p01), PhongBRDF::factory(color, 32.0));
            scene.add(Triangle(p00, p10, p11), PhongBRDF::factory(color, 32.0));
        }
    }

    // Set accelerator
    scene.setAccelerator();

    // Set camera
    Vector3D eye(-30.0, 10.0, -40.0);
    Camera camera(eye, -eye.normalized(), Vector3D(0.0, 1.0, 0.0), 45.0, IMAGE_WIDTH, IMAGE_HEIGHT, 0.5);

    // Set render parameters
    RenderParameters params(2000000, 1024);

    // Set renderer
    // ProgressivePhotonMapping ppm;
    // ppm.render(scene, camera, params);
    PathTracing pathtrace;
    pathtrace.render(scene, camera, params);
}
