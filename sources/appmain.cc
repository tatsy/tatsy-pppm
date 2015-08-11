#include "renderer.h"

#include <cstdio>
#include <string>

void setScene(Scene* scene, Camera* camera, int imageWidth, int imageHeight) {
    // Parameters
    const int tiles = 8;
    const double tileSize = 8.0;

    // Load mesh
    Trimesh trimesh;
    trimesh.load(ASSET_DIRECTORY + "dragon.ply");
    trimesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    trimesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));
    trimesh.translate(Vector3D(-5.0, 0.0, 5.0));

    // Marble BSSRDF (from [Jensen et al. 2001])
    const Vector3D sigmap_s = Vector3D(2.19, 2.62, 3.00) * 0.02;
    const Vector3D sigma_a  = Vector3D(0.0021, 0.0041, 0.0071) * 0.02;

    // BSDF meshBsdf = PhongBRDF::factory(Vector3D(0.5, 0.5, 0.5), 16.0);
    BSDF meshBsdf = LambertianBRDF::factory(Vector3D(0.75, 0.75, 0.75));
    BSSRDF meshBssrdf = DipoleBSSRDF::factory(sigma_a, sigmap_s, 1.5);
    meshBsdf.setBssrdf(meshBssrdf);

    // Load title
    Trimesh titleMesh;
    titleMesh.load(ASSET_DIRECTORY + "rt3.ply");
    titleMesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    titleMesh.scale(1.2);
    titleMesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));
    titleMesh.translate(Vector3D(20.0, 0.0, -5.0));

    // Load torus mesh
    Trimesh torusMesh;
    torusMesh.load(ASSET_DIRECTORY + "torus.ply");
    torusMesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    torusMesh.scale(0.7);
    torusMesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));

    // Load environment map
    Envmap envmap(ASSET_DIRECTORY + "subway.hdr");
    
    // Set scene
    scene->add(trimesh, meshBsdf);
    scene->add(titleMesh, LambertianBRDF::factory(Vector3D(0.70, 0.30, 0.30)));
    scene->add(torusMesh, RefractionBSDF::factory(Vector3D(0.50, 0.50, 0.95)));
    scene->setEnvmap(envmap);

    // Set floor
    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < tiles; j++) {
            double ii = (i - tiles / 2) * tileSize;
            double jj = (j - tiles / 2) * tileSize;
            Vector3D p00(ii, -10.0, jj);
            Vector3D p01(ii + tileSize, -10.0, jj);
            Vector3D p10(ii, -10.0, jj + tileSize);
            Vector3D p11(ii + tileSize, -10.0, jj + tileSize);
            Vector3D color = (i + j) % 2 == 0 ? Vector3D(0.9, 0.9, 0.9) : Vector3D(0.2, 0.2, 0.2);
            BSDF     bsdf  = (i + j) % 2 == 0 ? PhongBRDF::factory(color, 128.0) : SpecularBRDF::factory(color); 
            scene->add(Triangle(p00, p11, p01), bsdf);
            scene->add(Triangle(p00, p10, p11), bsdf);
        }
    }

    // Set accelerator
    scene->setAccelerator();

    // Set camera
    Vector3D eye(-20.0, 5.0, -20.0);
    *camera = Camera(eye, -eye.normalized(), Vector3D(0.0, 1.0, 0.0), 45.0, imageWidth, imageHeight, 1.0);
}

int main(int argc, char** argv) {

    const int imageWidth  = argc >= 2 ? atoi(argv[1]) : 1280;
    const int imageHeight = argc >= 3 ? atoi(argv[2]) : 720;
    const int spp         = argc >= 4 ? atoi(argv[3]) : 10240;

    Scene scene;
    Camera camera;
    setScene(&scene, &camera, imageWidth, imageHeight);

    // Set render parameters
    RenderParameters params(2000000, spp, 16.0);

    // Set renderer
    ProgressivePhotonMappingProb ppmapa;
    ppmapa.render(scene, camera, params);
    // ProgressivePhotonMapping ppm;
    // ppm.render(scene, camera, params);
    // PathTracing pathtrace;
    // pathtrace.render(scene, camera, params, true);
}
