#define PATH_TRACING_EXPORT
#include "path_tracing.h"

#include <ctime>
#include <algorithm>

#include "halton.h"
#include "timer.h"

PathTracing::PathTracing()
    : _result()
    , _integrator(NULL)
{
}

PathTracing::~PathTracing()
{
    delete _integrator;
}

void PathTracing::render(const Scene& scene, const Camera& camera, const RenderParameters& params, bool enableBSSRDF) {
    const int width = camera.imagesize().width();
    const int height = camera.imagesize().height();

    // Preprocess to account for subsurface scattering
    double areaRadius = 0.0;
    if (enableBSSRDF) {
        // Computing spans of dert throwing
        double avgArea = 0.0;
        for (int i = 0; i < scene.numTriangles(); i++) {
            avgArea += scene.getTriangle(i).area();
        }
        avgArea /= scene.numTriangles();

        // Initializing subsurface scattering integrator
        areaRadius = sqrt(avgArea) * 1.0;
        _integrator = new SubsurfaceIntegrator();
    }

    // Preparing random number generators and image buffers for parallel processing
    Halton* rand   = new Halton[OMP_NUM_CORE];
    Image*  buffer = new Image[OMP_NUM_CORE];
    for (int i = 0; i < OMP_NUM_CORE; i++) {
        rand[i]   = Halton(200, true, i);
        buffer[i] = Image(width, height);
    }

    const int taskPerThread = (params.spp() + OMP_NUM_CORE - 1) / OMP_NUM_CORE;

    Timer timer;
    timer.start();

    // Rendering
    _result.resize(width, height);
    for (int t = 0; t < taskPerThread; t++) {
        if (enableBSSRDF) {
            _integrator->initialize(scene, params, areaRadius, 0.05);
        }

        // Tracing rays for each pixel
        ompfor (int threadID = 0; threadID < OMP_NUM_CORE; threadID++) {
            RandomSequence rseq;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    rand[threadID].request(rseq, 200);
                    buffer[threadID].pixel(x, height - y - 1) += executePathTracing(scene, camera, x, y, rseq);
                }
            }
        }

        // Processing accumulated pixel colors for saving intermediate results
        _result.fill(Vector3D(0.0, 0.0, 0.0));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int k = 0; k < OMP_NUM_CORE; k++) {
                    _result.pixel(x, y) += buffer[k](x, y) / ((t + 1) * OMP_NUM_CORE);
                }
            }
        }

        char filename[512];
        sprintf(filename, (RESULT_DIRECTORY + "path_tracing_%03d.png").c_str(), t + 1);
        _result.gamma(2.2, true);
        _result.save(filename);

        // Displaing text message
        printf("%.2f sec: %d / %d\n", timer.stop(), t + 1, params.spp());
        if (timer.stop() > 875.0) {
            printf("About 15 mins have passed !!\n");
            _result.save(RESULT_DIRECTORY + "final_result.png");
            // break;
        }
    }
    printf("Finish !!\n");

    // Deallocating memories
    delete[] rand;
    delete[] buffer;
}

Vector3D PathTracing::executePathTracing(const Scene& scene, const Camera& camera, double pixelX, double pixelY, RandomSequence& rseq, int maxBounces) const {
    const double px = pixelX + rseq.pop() - 0.5;
    const double py = pixelY + rseq.pop() - 0.5;
    
    Ray ray = camera.getRay(px, py);
    Vector3D weight(1.0, 1.0, 1.0);
    Vector3D throughput(0.0, 0.0, 0.0);

    for (int bounces = 0; bounces < maxBounces; bounces++) {
        Intersection isect;
        bool foundIntersection = scene.intersect(ray, isect);
        
        // If not found intersection, directly sample environment map
        if (!foundIntersection) {
            throughput += weight * scene.envmap().sampleFromDir(ray.direction());
            break;
        }

        // Request random numbers for this iteration
        const double rands[3] = { rseq.pop(), rseq.pop(), rseq.pop() };

        // Compute orienting normal
        const Hitpoint& hitpoint = isect.hitpoint();
        const Vector3D orientingNormal = Vector3D::dot(ray.direction(), hitpoint.normal()) ? hitpoint.normal() : -hitpoint.normal();

        // Acount for subsurface scattering
        const int triangleIndex = isect.objectID();
        const BSDF& bsdf = scene.getBsdf(triangleIndex);
        if (bsdf.type() == BSDF_TYPE_BSSRDF) {
            // Account for the indirect subsurface scattering
            throughput += weight * _integrator->irradiance(hitpoint.position(), bsdf);
            break;
        }

        // Find new direction from BRDF
        double pdf = 1.0;
        Vector3D nextdir;
        bsdf.sample(ray.direction(), orientingNormal, rands[0], rands[1], &nextdir, &pdf);
        weight *= bsdf.reflectance() / pdf;
        ray = Ray(hitpoint.position(), nextdir);

        // Possibly terminate the path
        if (bounces > 3) {
            const double roulette = std::max(weight.x(), std::max(weight.y(), weight.z()));
            if (rands[2] > roulette) break;
            weight /= roulette;
        }
    }

    return throughput;
}

