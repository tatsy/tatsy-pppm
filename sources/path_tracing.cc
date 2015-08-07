#define PATH_TRACING_EXPORT
#include "path_tracing.h"

#include <ctime>
#include <algorithm>

#include "halton.h"
#include "timer.h"

namespace {
    bool isTotalRef(const bool isIncoming,
                    const Vector3D& position,
                    const Vector3D& in,
                    const Vector3D& normal,
                    const Vector3D& orientNormal,
                    Vector3D* reflectDir,
                    Vector3D* refractDir,
                    double* fresnelRef,
                    double* fresnelTransmit) {

        *reflectDir = Vector3D::reflect(in, normal);

        // Snell's rule
        const double nnt = isIncoming ? IOR_VACCUM / IOR_OBJECT : IOR_OBJECT / IOR_VACCUM;
        const double ddn = Vector3D::dot(in, orientNormal);
        const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

        if (cos2t < 0.0) {
            // Total reflect
            *refractDir = Vector3D();
            *fresnelRef = 1.0;
            *fresnelTransmit = 0.0;
            return true;
        }

        *refractDir = (in * nnt - normal * (isIncoming ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t))).normalized();

        const double a = IOR_OBJECT - IOR_VACCUM;
        const double b = IOR_OBJECT + IOR_VACCUM;
        const double R0 = (a * a) / (b * b);

        const double c = 1.0 - (isIncoming ? -ddn : Vector3D::dot(*refractDir, -orientNormal));
        *fresnelRef = R0 + (1.0 - R0) * pow(c, 5.0);
        *fresnelTransmit = 1.0 - (*fresnelRef);

        return false;
    }
}

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

    // Process for BSSRDF
    double areaRadius = 0.0;
    if (enableBSSRDF) {
        // Compute sample span
        double avgArea = 0.0;
        for (int i = 0; i < scene.numTriangles(); i++) {
            avgArea += scene.getTriangle(i).area();
        }
        avgArea /= scene.numTriangles();

        // Initialize integrator
        areaRadius = sqrt(avgArea) * 0.5;
        _integrator = new SubsurfaceIntegrator();
    }

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
        if (enableBSSRDF) {
            _integrator->initialize(scene, params, areaRadius, 0.05);
        }

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

        printf("%7.2f sec: %6.2f %% processed ...\n", timer.stop(), 100.0 * (t + 1) / taskPerThread);
        if (timer.stop() > 875.0) {
            printf("About 15 mins have passed !!\n");
            // break;
        }
    }
    printf("Finish !!\n");

    delete[] rand;
    delete[] buffer;
}

Vector3D PathTracing::executePathTracing(const Scene& scene, const Camera& camera, double pixelX, double pixelY, RandomSequence& rseq) const {
    const double px = pixelX + rseq.pop() - 0.5;
    const double py = pixelY + rseq.pop() - 0.5;
    Ray ray = camera.getRay(px, py);
    return radiance(scene, ray, rseq, 0) * camera.sensitivity();
}

Vector3D PathTracing::radiance(const Scene& scene, const Ray& ray, RandomSequence& rseq, int bounces, int bounceLimit, int bounceMin) const {
    Intersection isect;
    if (!scene.intersect(ray, isect)) {
        return scene.envmap().sampleFromDir(ray.direction());
    }

    const double rands[3] = { rseq.pop(), rseq.pop(), rseq.pop() };

    const int objectID = isect.objectID();
    const BSDF& bsdf = scene.getBsdf(objectID);
    const Hitpoint& hitpoint = isect.hitpoint();
    const Vector3D orientNormal = Vector3D::dot(ray.direction(), hitpoint.normal()) < 0.0 ? hitpoint.normal() : -hitpoint.normal();

    if (bounces >= bounceLimit) {
        return bsdf.reflectance();
    }

    // Russian roulette
    double roulette = std::max(bsdf.reflectance().x(), std::max(bsdf.reflectance().y(), bsdf.reflectance().z()));
    if (bounces >= bounceMin) {
        if (roulette < rands[0]) {
            return Vector3D(0.0, 0.0, 0.0);
        }
    } else {
        roulette = 1.0;
    }

    Vector3D weight(1.0, 1.0, 1.0);
    Vector3D nextRadiance(1.0, 1.0, 1.0);
    if (bsdf.type() == BSDF_TYPE_BSSRDF) {
        Vector3D nextDir;
        bsdf.sample(ray.direction(), orientNormal, rands[1], rands[2], &nextDir);
        Ray reflectRay(hitpoint.position(), nextDir);
        Vector3D reflectR  = radiance(scene, reflectRay, rseq, bounces + 1, bounceLimit, bounceMin);
        Vector3D transmitR = _integrator->irradiance(hitpoint.position(), bsdf);
        weight = bsdf.reflectance() / roulette;
        nextRadiance = reflectR * (1.0 - REFLECT_PROBABILITY) + transmitR * REFLECT_PROBABILITY;
    } else {
        Vector3D nextDir;
        bsdf.sample(ray.direction(), orientNormal, rands[1], rands[2], &nextDir);
        Ray nextRay(hitpoint.position(), nextDir);    
        weight       = bsdf.reflectance() / roulette;
        nextRadiance = radiance(scene, nextRay, rseq, bounces + 1, bounceLimit, bounceMin);
    }

    return weight * nextRadiance;
}
