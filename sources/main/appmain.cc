#include "../renderer.h"

#include <iostream>
#include <cstdio>
#include <string>

//! Hard-coded render scene
void setScene(Scene* scene, Camera* camera, int imageWidth, int imageHeight);

//! Main
int main(int argc, char** argv) {
    const int imageWidth  = argc >= 2 ? atoi(argv[1]) : 1920;
    const int imageHeight = argc >= 3 ? atoi(argv[2]) : 1080;
    const int spp         = argc >= 4 ? atoi(argv[3]) : 10240;

    Scene scene;
    Camera camera;
    setScene(&scene, &camera, imageWidth, imageHeight);

    // Set render parameters
    RenderParameters params(2000000, spp, 128, 16.0);

    // Set renderer
    ProgressivePhotonMappingProb ppmapa;
    ppmapa.render(scene, camera, params);
}

//! Hard-coded render scene
void setScene(Scene* scene, Camera* camera, int imageWidth, int imageHeight) {
    std::cout << "Preparing the scene -> ";

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
    const Vector3D sigmap_s = Vector3D(2.19, 2.62, 3.00);
    const Vector3D sigma_a  = Vector3D(0.0021, 0.0041, 0.0071);

    // Skin BSSRDF (from [Jensen et al. 2001])
    // const Vector3D sigmap_s = Vector3D(1.09, 1.59, 1.79);
    // const Vector3D sigma_a  = Vector3D(0.013, 0.070, 0.145);

    // Skim milk BSSRDF
    // const Vector3D sigmap_s(0.70, 1.22, 1.90);
    // const Vector3D sigma_a(0.0014, 0.0025, 0.0142);

    // Whole milk BSSRDF
    // const Vector3D sigmap_s(2.55, 3.21, 3.77);
    // const Vector3D sigma_a(0.0011, 0.0024, 0.014);

    // Cream BSSRDF
    // const Vector3D sigmap_s(7.38, 5.47, 3.15);
    // const Vector3D sigma_a(0.0002, 0.0028, 0.0163);

    BSDF meshBsdf = RefractionBSDF::factory(Vector3D(0.99, 0.99, 0.99));
    BSSRDF meshBssrdf = DipoleBSSRDF::factory(sigma_a, sigmap_s, 1.5, 1.0);
    meshBsdf.setBssrdf(meshBssrdf);

    // Load title
    Trimesh titleMesh;
    titleMesh.load(ASSET_DIRECTORY + "rt3.ply");
    titleMesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    titleMesh.scale(1.5);
    titleMesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));
    titleMesh.translate(Vector3D(20.0, 0.0, -5.0));

    // Bunny mesh
    Trimesh bunnyMesh;
    bunnyMesh.load(ASSET_DIRECTORY + "bunny.ply");
    bunnyMesh.fitToBBox(BBox(-5.0, -5.0, -5.0, 5.0, 5.0, 5.0));
    bunnyMesh.scale(0.8);
    bunnyMesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));
    bunnyMesh.translate(Vector3D(5.0, 0.0, -10.0));

    // Load torus mesh
    Trimesh torusMesh;
    torusMesh.load(ASSET_DIRECTORY + "torus.ply");
    torusMesh.fitToBBox(BBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0));
    torusMesh.scale(0.7);
    torusMesh.putOnPlane(Plane(10.0, Vector3D(0.0, 1.0, 0.0)));

    // Load environment map
    Envmap envmap(ASSET_DIRECTORY + "sunrise.hdr");
    
    // Set scene
    scene->add(trimesh, meshBsdf);
    scene->add(titleMesh, LambertianBRDF::factory(Vector3D(0.70, 0.30, 0.30)));
    scene->add(torusMesh, RefractionBSDF::factory(Vector3D(0.75, 0.75, 0.99))); 
    scene->add(bunnyMesh, LambertianBRDF::factory(Vector3D(0.75, 0.75, 0.75)));
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
            Vector3D color = (i + j) % 2 == 0 ? Vector3D(0.9, 0.9, 0.9) 
                                              : Vector3D(0.2, 0.2, 0.2);
            BSDF     bsdf  = (i + j) % 2 == 0 ? PhongBRDF::factory(color, 128.0)
                                              : SpecularBRDF::factory(color); 
            scene->add(Triangle(p00, p11, p01), bsdf);
            scene->add(Triangle(p00, p10, p11), bsdf);
        }
    }

    // Set accelerator
    scene->setAccelerator();

    // Set camera
    Vector3D eye(-25.0, 5.0, -18.0);
    *camera = Camera(eye, -eye.normalized(), Vector3D(0.0, 1.0, 0.0), 45.0, imageWidth, imageHeight, 1.0);

    std::cout << "OK" << std::endl;
}