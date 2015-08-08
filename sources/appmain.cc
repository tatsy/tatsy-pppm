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
    trimesh.load(ASSET_DIRECTORY + "dragon.ply");
    trimesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    trimesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));
    trimesh.translate(Vector3D(-5.0, 0.0, 5.0));

    // Marble BSSRDF (from [Jensen et al. 2001])
    const Vector3D sigmap_s = Vector3D(2.19, 2.62, 3.00);
    const Vector3D sigma_a  = Vector3D(0.0021, 0.0041, 0.0071);

    BSDF meshBsdf = SpecularBRDF::factory(Vector3D(0.50, 0.50, 0.50));
    BSSRDF meshBssrdf = DipoleBSSRDF::factory(sigma_a, sigmap_s, 1.5);
    meshBsdf.setBssrdf(meshBssrdf);

    // Load title
    Trimesh titleMesh;
    titleMesh.load(ASSET_DIRECTORY + "rt3.ply");
    titleMesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    titleMesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));
    titleMesh.translate(Vector3D(20.0, 0.0, -5.0));

    // Load environment map
    Envmap envmap(ASSET_DIRECTORY + "subway.hdr");
    
    // Set scene
    Scene scene;
    scene.add(trimesh, meshBsdf);
    scene.add(titleMesh, LambertianBRDF::factory(Vector3D(0.25, 0.75, 0.75)));
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
            Vector3D color = (i + j) % 2 == 0 ? Vector3D(0.9, 0.9, 0.9) : Vector3D(0.2, 0.2, 0.2);
            BSDF     bsdf  = (i + j) % 2 == 0 ? PhongBRDF::factory(color, 128.0) : SpecularBRDF::factory(color); 
            scene.add(Triangle(p00, p11, p01), bsdf);
            scene.add(Triangle(p00, p10, p11), bsdf);
        }
    }

    // Set accelerator
    scene.setAccelerator();

    // Set camera
    Vector3D eye(-20.0, 5.0, -20.0);
    Camera camera(eye, -eye.normalized(), Vector3D(0.0, 1.0, 0.0), 45.0, IMAGE_WIDTH, IMAGE_HEIGHT, 0.5);

    // Set render parameters
    RenderParameters params(2000000, 10240, 16.0);

    // Set renderer
    // ProgressivePhotonMapping ppm;
    // ppm.render(scene, camera, params, true);
    PathTracing pathtrace;
    pathtrace.render(scene, camera, params, true);
}
