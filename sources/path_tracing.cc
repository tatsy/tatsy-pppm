#define PATH_TRACING_EXPORT
#include "path_tracing.h"

#include <ctime>
#include <algorithm>

#include "halton.h"
#include "timer.h"

PathTracing::PathTracing()
    : _result()
{
}

PathTracing::~PathTracing()
{
}

void PathTracing::render(const Scene& scene, const Camera& camera, const RenderParameters& params) {
    const int width = camera.imagesize().width();
    const int height = camera.imagesize().height();

    Halton* rand   = new Halton[OMP_NUM_CORE];
    Image*  buffer = new Image[OMP_NUM_CORE];
    for (int i = 0; i < OMP_NUM_CORE; i++) {
        rand[i]   = Halton(200, true, i);
        buffer[i] = Image(width, height);
    }

    const int taskPerThread = (params.spp() + OMP_NUM_CORE - 1) / OMP_NUM_CORE;

    Timer timer;
    timer.start();

    _result.resize(width, height);
    for (int t = 0; t < taskPerThread; t++) {
        ompfor (int threadID = 0; threadID < OMP_NUM_CORE; threadID++) {
            RandomSequence rseq;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    rand[threadID].request(rseq, 200);
                    buffer[threadID].pixel(x, height - y - 1) += executePathTracing(scene, camera, x, y, rseq);
                }
            }
        }

        _result.fill(Vector3D(0.0, 0.0, 0.0));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int k = 0; k < OMP_NUM_CORE; k++) {
                    _result.pixel(x, y) += buffer[k](x, y) / ((t + 1) * OMP_NUM_CORE);
                }
            }
        }

        char filename[512];
        sprintf(filename, (RESULT_DIRECTORY + "path_tracing_%03d.bmp").c_str(), t + 1);
        _result.gamma(2.2, true);
        _result.saveBMP(filename);

        printf("%7.2f sec: %6.2f %% processed ...\r", timer.stop(), 100.0 * (t + 1) / taskPerThread);
        if (timer.stop() > 875.0) {
            printf("\nAbout 15 mins have passed !!");
            break;
        }
    }
    printf("\nFinish !!\n");

    delete[] rand;
    delete[] buffer;
}

Vector3D PathTracing::executePathTracing(const Scene& scene, const Camera& camera, double pixelX, double pixelY, RandomSequence& rseq) {
    const double px = pixelX + rseq.pop() - 0.5;
    const double py = pixelY + rseq.pop() - 0.5;
    Ray ray = camera.getRay(px, py);
    return radiance(scene, ray, rseq, 0) * camera.sensitivity();
}

Vector3D PathTracing::radiance(const Scene& scene, const Ray& ray, RandomSequence& rseq, int bounces, int bounceLimit, int bounceMin) {
    Intersection isect;
    if (!scene.intersect(ray, isect)) {
        return scene.envmap().sampleFromDir(ray.direction());
    }

    double rands[3] = { rseq.pop(), rseq.pop(), rseq.pop() };

    const int objectID = isect.objectID();
    const BRDF& brdf = scene.getBrdf(objectID);
    const Hitpoint& hitpoint = isect.hitpoint();
    const Vector3D orientNormal = Vector3D::dot(ray.direction(), hitpoint.normal()) < 0.0 ? hitpoint.normal() : -hitpoint.normal();

    if (bounces >= bounceLimit) {
        return brdf.reflectance();
    }

    // Russian roulette
    double roulette = std::max(brdf.reflectance().x(), std::max(brdf.reflectance().y(), brdf.reflectance().z()));
    if (bounces > bounceMin) {
        if (roulette < rands[0]) {
            return Vector3D(0.0, 0.0, 0.0);
        }
    } else {
        roulette = 1.0;
    }

    Vector3D weight(1.0, 1.0, 1.0);
    Vector3D nextRadiance(1.0, 1.0, 1.0);

    Vector3D nextDir;
    brdf.sample(ray.direction(), orientNormal, rands[1], rands[2], &nextDir);
    Ray nextRay(hitpoint.position(), nextDir);
    weight = weight * brdf.reflectance() / roulette;
    nextRadiance = radiance(scene, nextRay, rseq, bounces + 1, bounceLimit, bounceMin);

    return weight * nextRadiance;
}
