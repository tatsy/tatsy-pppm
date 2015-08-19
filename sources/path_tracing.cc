#define PATH_TRACING_EXPORT
#include "path_tracing.h"

#include <ctime>
#include <algorithm>

#include "timer.h"
#include "random.h"
#include "halton.h"
#include "reflectance.h"

PathTracing::PathTracing()
    : _result()
    , _integrator(NULL)
{
}

PathTracing::~PathTracing()
{
    delete _integrator;
}

void PathTracing::render(const Scene& scene, const Camera& camera, const RenderParameters& params, RandomSamplerType randomSamplerType) {
    const int width = camera.imagesize().width();
    const int height = camera.imagesize().height();

    // Start timer
    Timer timer;
    timer.start();

    // Preprocess to account for subsurface scattering
    bool enableBssrdf = false;
    double areaRadius = 0.0;
    double avgArea = 0.0;
    for (int i = 0; i < scene.numTriangles(); i++) {
        if (scene.getBsdf(i).type() & BSDF_TYPE_BSSRDF) {
            enableBssrdf = true;
            avgArea += scene.getTriangle(i).area();
        }
    }
    avgArea /= scene.numTriangles();

    if (enableBssrdf) {
        areaRadius = sqrt(avgArea) * 1.0;
        _integrator = new SubsurfaceIntegrator();
    }

    // Preparing random number generators and image buffers for parallel processing
    RandomSampler* rsamplers = new RandomSampler[OMP_NUM_CORE];
    for (int i = 0; i < OMP_NUM_CORE; i++) {
        switch (randomSamplerType) {
        case RANDOM_SAMPLER_PSEUDO_RANDOM:
            rsamplers[i] = Random::generateSampler(i);
            break;

        case RANDOM_SAMPLER_QUASI_MONTE_CARLO:
            rsamplers[i] = Halton::generateSampler(200, true, i);
            break;

        default:
            std::cerr << "[ERROR] unknown random sampler type !!" << std::endl;
            std::abort();
        }
    }

    // Distribute tasks
    const int taskPerThread = (height + OMP_NUM_CORE - 1) / OMP_NUM_CORE;
    std::vector<std::vector<int> > tasks(OMP_NUM_CORE);
    for (int y = 0; y < height; y++) {
        tasks[y % OMP_NUM_CORE].push_back(y);
    }

    // Rendering
    bool isFinish = false;
    _result.resize(width, height);
    Image buffer = Image(width, height);
    buffer.fill(Vector3D(0.0, 0.0, 0.0));
    for (int i = 0; i < params.spp(); i++) {
        if (enableBssrdf) {
            _integrator->initialize(scene, params, areaRadius, 0.05);
        }

        // Tracing rays for each pixel
        for (int t = 0; t < taskPerThread; t++) {
            ompfor (int threadID = 0; threadID < OMP_NUM_CORE; threadID++) {
                RandomSequence rseq;
                if (t < tasks[threadID].size()) {
                    const int y = tasks[threadID][t];
                    for (int x = 0; x < width; x++) {
                        rsamplers[threadID].request(200, &rseq);
                        buffer.pixel(x, height - y - 1) += executePathTracing(scene, camera, params, x, y, rseq);
                    }
                }
            }
            printf("%6.2f %% processed ...\r", 100.0 * (t + 1) / taskPerThread);
        }
        printf("\n");

        // Processing accumulated pixel colors for saving intermediate results
        _result.fill(Vector3D(0.0, 0.0, 0.0));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                _result.pixel(x, y) = buffer(x, y) / (i + 1);
            }
        }

        char filename[512];
        sprintf(filename, (RESULT_DIRECTORY + "%03d.png").c_str(), i + 1);
        _result.gamma(2.2, true);
        _result.save(filename);
        printf("%.2f sec: %d / %d\n", timer.stop(), i + 1, params.spp());

        if (timer.stop() > 875.0 && !isFinish) {
            printf("About 15 mins have passed!!\n");
            _result.save(RESULT_DIRECTORY + "final_result.png");
            isFinish = true;
#ifdef __ONSITE__
            break;
#endif
        }
    }
    printf("Finish !!\n");

    // Deallocating memories
    delete[] rsamplers;
}

Vector3D PathTracing::executePathTracing(const Scene& scene, const Camera& camera, const RenderParameters& params, int pixelX, int pixelY, RandomSequence& rseq, int bounceLimit) const {
    const double px = pixelX + rseq.pop() - 0.5;
    const double py = pixelY + rseq.pop() - 0.5;
    Ray ray = camera.getRay(px, py);

    return radiance(scene, ray, params, rseq, 0, bounceLimit);
}

Vector3D PathTracing::radiance(const Scene& scene, const Ray& ray, const RenderParameters& params, RandomSequence& rseq, int bounces, int bounceLimit) const {
    // Terminate trace if the bounces reach limit or not intersect the scene
    Intersection isect;
    if (bounces >= bounceLimit || !scene.intersect(ray, isect)) {
        return scene.envmap().sampleFromDir(ray.direction());
    }

    // Request random numbers
    const double rands[3] = { rseq.pop(), rseq.pop(), rseq.pop() };

    // Next bounce
    const int objectID = isect.objectID();
    const Hitpoint& hitpoint = isect.hitpoint();
    const BSDF& bsdf = scene.getBsdf(objectID);

    double roulette  = std::max(bsdf.reflectance().x(), std::max(bsdf.reflectance().y(), bsdf.reflectance().z()));
    if (bounces >= 3) {
        if (rands[0] > roulette) {
            return Vector3D(0.0, 0.0, 0.0);
        }
    } else {
        roulette = 1.0;
    }

    Vector3D throughput(0.0, 0.0, 0.0);

    // Account for subsurface scattering
    if (bsdf.type() & BSDF_TYPE_BSSRDF) {
        if (bsdf.type() & BSDF_TYPE_REFRACTION) {
            bool into = Vector3D::dot(hitpoint.normal(), ray.direction()) < 0.0;
            const Vector3D orieintingNormal = into ? hitpoint.normal() : -hitpoint.normal();
            Vector3D reflectDir, transmitDir;
            double fresnelRe, fresnelTr;
            if (checkTotalReflection(into, ray.direction(), hitpoint.normal(), orieintingNormal, &reflectDir, &transmitDir, &fresnelRe, &fresnelTr)) {
                const Ray nextRay(hitpoint.position(), reflectDir);
                return radiance(scene, nextRay, params, rseq, bounces + 1, bounceLimit);
            } else {
                const double probability = 0.25 + REFLECT_PROBABILITY * 0.5;
                if (rands[1] < probability) {
                    // Reflection
                    const Ray nextRay(hitpoint.position(), reflectDir);
                    return bsdf.reflectance() * radiance(scene, nextRay, params, rseq, bounces, bounceLimit) * (fresnelRe / probability);
                } else {
                    // Transmit
                    return _integrator->irradiance(hitpoint.position(), bsdf) * (fresnelTr / (1.0 - probability));
                }
            }
        } else {
            const double probability = 0.25 + REFLECT_PROBABILITY * 0.5;
            Vector3D irad = _integrator->irradiance(hitpoint.position(), bsdf);
            throughput += irad * (1.0 - probability);
        }
    }

    double pdf = 1.0;
    Vector3D nextDir;
    bsdf.sample(ray.direction(), hitpoint.normal(), rands[1], rands[2], &nextDir, &pdf);

    Ray nextRay(hitpoint.position(), nextDir);
    throughput += bsdf.reflectance() * radiance(scene, nextRay, params, rseq, bounces + 1, bounceLimit) / (pdf * roulette);

    return throughput;
}
